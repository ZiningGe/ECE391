#include "lib.h"
#include "page.h"
#include "systemcall.h"
#include "terminal.h"
#include "keyboard.h"


/* void init_page()
 * Description: initialize page directory table and page table
 * Inputs:   NA   
 * Return Value: NA
 * Function: initialize present and other control bits for pdt and pt entries */
void init_page() {
    int i;
    // first initialize PDE values
    for(i = 0; i < PAGE_SIZE; i++){
        pde[i].val = 0;
        pte[i].val = 0;
    }
    // initilize page directory table for 4 kb
    // there's only one existing Page Table 
    pde[0].p_d = 1;
    pde[0].rd_wt_d = 1;
    pde[0].user_sup_d = 1;
    pde[0].page_size_d = 0;
    pde[0].addr_31_12_d = ((uint32_t) pte >> 12);

    // initilize page directory table for 4mb
    pde[1].p_d = 1;
    pde[1].rd_wt_d = 1;
    pde[1].page_size_d = 1;
    pde[1].addr_31_22_d = KERNEL_ADDRESS >> 22;
    

    for(i = 0; i < 4; i++){
        // video mem idx = B8
        pte[VIDEO_MEM_IDX+i].p_t = 1;
        pte[VIDEO_MEM_IDX+i].rd_wt_t = 1;
        pte[VIDEO_MEM_IDX+i].user_sup_t = 1;
        /*
        To keep things simple, the kernel and video memory 
        will be at the same location in virtual memory as 
        they are in physical memory.
        */
        pte[VIDEO_MEM_IDX+i].addr_31_12_t = (VIDEO_MEM_ADDR >> 12) + i;

    }
    
    for(i = 0 ; i < 1024; i++) {
        video_pte[i].p_t = 0;
    }

    
}


/* void enble_page()
 * Description: enable page directory table and page table
 * Inputs: page_dir_entry* pde -- pointer to the page directory table
 * Return Value: NA
 * Function: load the address of pdt to cr3 and set some control bits */
void enble_page(page_dir_entry* pde) {
    // question for 80000001 or 0
    asm volatile("                 \n\
        movl %0, %%eax             \n\
        movl %%eax, %%cr3          \n\
        movl %%cr4, %%eax          \n\
        orl $0x00000010, %%eax     \n\
        movl %%eax, %%cr4          \n\
        movl %%cr0, %%eax          \n\
        orl $0x80000001, %%eax     \n\
        movl %%eax, %%cr0          \n\
        "         
        :
        : "r" (pde)
        : "eax"
    );
}



/* void set_process_page()
 * Description: set or restore the page for the process
 * Inputs: pid -- the process id number
 * Return Value: NA
 * Function: set the mapping from VM to Physical Mem*/
void set_process_page(uint32_t pid){
    pde[USER_PROGRAM_START >> 22].p_d = 1;
    pde[USER_PROGRAM_START >> 22].rd_wt_d = 1;
    pde[USER_PROGRAM_START >> 22].page_size_d = 1;
    pde[USER_PROGRAM_START >> 22].user_sup_d = 1;
    pde[USER_PROGRAM_START >> 22].addr_31_22_d = (uint32_t)((KERNEL_BUTTOM >> 22) + pid);

    asm volatile(
            "mov %%cr3, %%eax    \n\
            mov %%eax, %%cr3     \n"
            :
            :
            : "eax"
    ); 
}

/* void set_video_page()
 * Description: set the page for the user video memory
 * Inputs: screen_start -- the address to store the starting address of user video mem
 * Return Value: NA
 * Function: set the mapping for a 4kb video memory from VM to Physical Mem*/
void set_video_page(uint8_t ** screen_start){
    // set 132MB page directory entry
    pde[(USER_PROGRAM_START + fourMB) >> 22].p_d = 1;
    pde[(USER_PROGRAM_START + fourMB) >> 22].rd_wt_d = 1;
    pde[(USER_PROGRAM_START + fourMB) >> 22].user_sup_d = 1;
    pde[(USER_PROGRAM_START + fourMB) >> 22].addr_31_12_d = (uint32_t)video_pte >> 12;

    // set first page table starting at 132MB
    video_pte[curr_rid].p_t = 1;
    video_pte[curr_rid].rd_wt_t = 1;
    video_pte[curr_rid].user_sup_t = 1; 
    if (curr_rid == curr_kid)   
        video_pte[curr_rid].addr_31_12_t = VIDEO_MEM_ADDR >> 12;
    else  
        video_pte[curr_rid].addr_31_12_t = (VIDEO_MEM_ADDR + (1 + curr_rid) * fourKB) >> 12;

    // flush TLB
    asm volatile(
            "mov %%cr3, %%eax    \n\
            mov %%eax, %%cr3     \n"
            :
            :
            : "eax"
    ); 

    *screen_start = (uint8_t *)(USER_PROGRAM_START + (curr_rid) * fourKB + fourMB); 
}

/* void clear_video_page()
 * Description: clear the page for the user video memory and flush TLB
 * Inputs: NA
 * Return Value: NA
 * Function: clear the mapping for a 4kb video memory from VM to Physical Mem*/
void clear_video_page(){
    video_pte[curr_rid].p_t = 0;

    asm volatile(
            "mov %%cr3, %%eax    \n\
            mov %%eax, %%cr3     \n"
            :
            :
            : "eax"
    ); 
}

/* update_video_page_switch
 * Description: update the video page during screen switching 
 * Inputs: dst: index of dstination scr: index of source in pte
 * Return Value: NA
 * Function: update the video page during screen switching */
void update_video_page_switch(uint32_t dst, uint32_t src){
    video_pte[dst].addr_31_12_t = VIDEO_MEM_ADDR >> 12;
    video_pte[src].addr_31_12_t = (VIDEO_MEM_ADDR + (1 + src) * fourKB)>> 12;

    // flush TLB
    asm volatile(
            "mov %%cr3, %%eax    \n\
            mov %%eax, %%cr3     \n"
            :
            :
            : "eax"
    ); 
}


/* copy_video_page
 * Description: copy the video page from terminal stored page to screen stores page in memoey 
 * Inputs: dst: index of dstination scr: index of source in pte
 * Return Value: NA
 * Function: helper function for switch handler in keyboard.c */
void copy_video_page(uint32_t dst, uint32_t src){
    // memcpy(VIDEO_MEM_ADDR + (src)*fourKB, VIDEO_MEM_ADDR, 4096);
    memcpy((uint8_t *)VIDEO_MEM_ADDR + (dst)*fourKB, 
           (uint8_t *)VIDEO_MEM_ADDR + (src)*fourKB, 
           4096);
}


