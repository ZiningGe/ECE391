#include"systemcall.h"
#include "lib.h"
#include "page.h"
#include "defaultcall.h"

//helper function here
static uint8_t get_cur_pid(void);
static void delete_pid(uint8_t p_id);
// static PCB* previous_PCB;
uint8_t pit_flag = 0;
static uint8_t process_id[6] = {0,0,0,0,0,0}; // 0 is able to use ; 1 is not;


// fops jumptables for different file types
static fops_jmp_table_t stdin_fops = { &default_open, &terminal_read, &default_write, &default_close}; 
static fops_jmp_table_t stdout_fops = {&default_open, &default_read, &terminal_write, &default_close};
static fops_jmp_table_t fs_fops = {&fs_open, &fs_read, &fs_write, &fs_close};
static fops_jmp_table_t dir_fops = {&dir_open, &dir_read, &dir_write, &dir_close};
static fops_jmp_table_t rtc_fops = {&rtc_open, &rtc_read, &rtc_write, &rtc_close};

/*
MAGIC NUMBER
0: 0x7f; 1: 0x45; 2: 0x4c; 3: 0x46
*/
uint8_t magic_num[4] = {0x7F, 0x45, 0x4C, 0x46};


//function list 
/* system_execute
 * Description: execute a executable file
 * Inputs:  command -- the commond to be parsed
 * Return Value:0 if success -1 if not
 * Function: create PCB and prepare for context switch
 */
int32_t system_execute(const uint8_t * command){
    uint8_t program_num;  
    // uint32_t flags;
    int entry_point;
    uint8_t arg[FILENAME_SIZE+1];
    int8_t filename[FILENAME_SIZE + 1] = {'\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0', '\0','\0',
                                        '\0', '\0','\0'};
    uint8_t* program_start;
    dir_entry curr_dentry;
    uint8_t header_buf[HEADBUF_SIZE];
    inode *cur_inode;
    cli();
    // ========== Parse args ==========
    argument_parser((int8_t *)command, arg, (int8_t *)filename);
    if(is_executable(filename)==-1){
        return -1;
    }

    // ========== Set up paging ==========
    //update id 
    program_num = get_cur_pid();
    if(program_num == FK_UP) {
        terminal_printf("processes reach to maximum\n");
        return 0;
    }
    set_process_page(program_num);
    flush_TLB();
    
    // Load file into memory
    if(read_dentry_by_name(filename, &curr_dentry) == -1){
        return -1;
    }
    if(read_data(curr_dentry.inode_num, 0, header_buf, 40) == -1){
        return -1;
    } 

    // header buff + 24 is the entry of executable file
    entry_point = (uint32_t)(header_buf[MAGNUM1]) | (uint32_t)(header_buf[MAGNUM2]) << 8 | (uint32_t)(header_buf[MAGNUM3]) <<16 | (uint32_t)(header_buf[MAGNUM4]) << 24;
    //Setup process paging
    //Load file to 128MB virtual memory, copy length = 4MB, start from 0x08048000
    program_start = (uint8_t*)PROGRAM_START;    
    // only need to indicate 4MB here, cuz the read_data() will help us to truncate extra length

    cur_inode = (inode*) getboot_block() + curr_dentry.inode_num + 1; 
    if(read_data(curr_dentry.inode_num, 0, program_start, cur_inode->length) == -1) {
        delete_pid(program_num);
        return -1;
    }

    PCB *pcb = (PCB *)(KERNEL_MAX - KERNEL_PROCESS_SIZE * (program_num + 1));

    // pre_PCB[curr_rid] = curr_PCB[curr_rid];

    //check whether it is the first pcb 
    if(curr_PCB[curr_rid] == NULL){
        pcb->father = NULL;
    }
    else{
        if (pit_flag)
            pcb->father = curr_PCB[curr_rid];
        else
            pcb->father = curr_PCB[curr_kid]; //need change
            pcb->father->isActive = 0;
    }

    strcpy((char*)pcb->arg, (char*)arg);


    if(pit_flag == 1) {
        curr_PCB[curr_rid] = pcb;
        pit_flag = 0;
    } else {
        curr_PCB[curr_kid] = pcb; //need change
    }
    
    pcb->pid = program_num;
    pcb->isActive = 1;
    
    asm volatile("movl %%esp, %0"
            : "=g"(pcb->esp));
    asm volatile("movl %%ebp, %0"
            : "=g"(pcb->ebp));


    asm volatile("movl %%esp, %0"
               : "=g"(pcb->esp2));
    asm volatile("movl %%ebp, %0"
               : "=g"(pcb->ebp2));
    
//-----------------------
    // init stdin and stdout section, qustionable (?
    fd_init(pcb);
    
    // ============== Prepare for Context Switch ==============
    // setup the esp0 and ss in 
    tss.esp0 = KERNEL_MAX - (KERNEL_PROCESS_SIZE * program_num) - 4;
    tss.ss0 = KERNEL_DS;

    // ============== Push IRET context to kernel stack & IRET ==============
    // needed for IRET: SS, ESP, FL, CS, EIP
    // see details in systemcall.S
    sti();
    asm volatile(
        "pushl  %0 \n\
         pushl  %1 \n\
         pushfl     \n\
         pushl  %2  \n\
         pushl  %3  \n\
         iret       \n"
         :
         : "r"(USER_DS), "r"(USER_BOTTOM), "r"(USER_CS), "r"(entry_point)
         :"memory"
    );

    return 0;
}

/* system_halt
 * Description: halt the process and return to the parent process
 * Inputs:  status -- the return code
 * Return Value:0 if success -1 if not
 * Function: restore esp and ebp for parent process
 */
int32_t system_halt(uint8_t status){
    // cli();
    int32_t prev_ebp, prev_esp, prev_program_num; //, prev_esp0;
    // uint32_t flags;
    if (curr_PCB[curr_rid] == NULL){
        // sti();
        return -1; // shell can't be halted since it's the starter program
    }

    // ====== close any relavant FDs =======
    fd_close(curr_PCB[curr_rid]);
    curr_PCB[curr_rid]->isActive = 0;
    if(curr_PCB[curr_rid]->father == NULL) {
        delete_pid(curr_PCB[curr_rid]->pid);
        // restore_flags(flags);
        curr_PCB[curr_rid] = NULL;
        // sti();
        system_execute((uint8_t*)"shell");
    }
   
    // ====== Restore parent data =======
    
    prev_program_num = curr_PCB[curr_rid]->father->pid;

     // ====== free pid =======

    if (prev_program_num == -1) {
        delete_pid(curr_PCB[curr_rid]->pid);
        curr_PCB[curr_rid] = NULL;
        // sti();
        system_execute((uint8_t*)"shell");
    } else {
        delete_pid(curr_PCB[curr_rid]->pid);
        clear_video_page();
        // ====== Jump to execute return =======
        tss.esp0 = KERNEL_MAX - (KERNEL_PROCESS_SIZE * prev_program_num) - 4;
        tss.ss0 = KERNEL_DS;
        
        // ====== Restore parent paging =======
        set_process_page(prev_program_num);
        flush_TLB(); 
        prev_ebp = curr_PCB[curr_rid]->ebp; // ->father
        prev_esp = curr_PCB[curr_rid]->esp; // ->father
        curr_PCB[curr_rid] = curr_PCB[curr_rid]->father;
        curr_PCB[curr_rid]->isActive = 1;
        // sti();
        asm volatile("  \n\
            movl %%ebx, %%esp   \n\
            movl %%ecx, %%ebp   \n\
            movl %%edx, %%eax   \n\
            leave               \n\
            ret                 \n"
            :
            :"b"(prev_esp), "c"(prev_ebp), "d"((uint32_t)status)
            :"eax", "ebp", "esp"
        );    
        // asm volatile("jmp return_back");
        // while(1);
        
        return (uint32_t)status;
    }
    // sti();
    return (uint32_t)status;
}

/*

	                   _ooOoo_
	                  o8888888o
	                  88" . "88
	                  (| -_- |)
	                  O\  =  /O
	               ____/`---'\____
	             .'  \\|     |//  `.
	            /  \\|||  :  |||//  \
	           /  _||||| -:- |||||-  \
	           |   | \\\  -  /// |   |
	           | \_|  ''\-/''  |   |
	           \  .-\__  `-`  ___/-. /
	         ___`. .'  /-.-\  `. . __
	      ."" '<  `.___\_<|>_/___.'  >'"".
	     | | :  `- \`.;`\ _ /`;.`/ - ` : | |
	     \  \ `-.   \_ __\ /__ _/   .-` /  /
	======`-.____`-.___\_____/___.-`____.-'======
	                   `=-='
                                                           
88                              88  
""              ,d              88  
                88              88  
88 8b,dPPYba, MM88MMM ,adPPYba, 88  
88 88P'   `"8a  88   a8P_____88 88  
88 88       88  88   8PP""""""" 88  
88 88       88  88,  "8b,   ,aa 88  
88 88       88  "Y888 `"Ybbd8"' 88  
*/


/* system_read
 * Description: read a file
 * Inputs:  fd -- the index of the file descriptor
 * Return Value:0 if success -1 if not
 * Function: update the file position
 */
int32_t system_read(int32_t fd, void * buf, int32_t nbytes){
    int retval;

    // Sanity Check
    if (buf == NULL)
        return -1;
    if (fd < 0 || fd > 7)
        return -1;
    if ((curr_PCB[curr_rid] == NULL) || (curr_PCB[curr_rid]->fd_arr[fd].flags==0))
        return -1;

        
    retval = curr_PCB[curr_rid]->fd_arr[fd].file_ops->read(fd, buf, nbytes);
    if (retval != -1)
        curr_PCB[curr_rid]->fd_arr[fd].file_position += retval;
    return retval;
    
}

/* system_open
 * Description: handle the open operation for system calls
 * Inputs:   filename: name of the file
 * Return Value: the # of fd for current opening file if success and -1 if not
 * Function: update the file open operarion*/
int32_t system_open(const uint8_t * filename){
    int idx;
    //check the validity of filename
    if (FILENAME_SIZE < sizeof(*filename)) return -1;
    dir_entry cur_dir; //varialbe for current file directory
    //find the dir
    if(read_dentry_by_name((int8_t*)filename,&cur_dir) == -1) return -1;
    //find each inode corrrespding to the normal 6 files
    for (idx = START_FILE_IDX; idx <= END_FILE_IDX; ++idx) {
        if((curr_PCB[curr_rid]->fd_arr[idx].flags & 0x1) == 0) break;
    }
    if (idx == MAX_FILE_SIZE) return -1;

    // assign the opern operation
    
    curr_PCB[curr_rid]->fd_arr[idx].inode = cur_dir.inode_num;
    curr_PCB[curr_rid]->fd_arr[idx].file_position = 0;
    curr_PCB[curr_rid]->fd_arr[idx].flags = 1;
    //match to correct file operations
    switch(cur_dir.file_type){
        case RTC_NUM:
            curr_PCB[curr_rid]->fd_arr[idx].file_ops = &rtc_fops;
            break;
        case DIR_NUM:
            curr_PCB[curr_rid]->fd_arr[idx].file_ops = &dir_fops;
            break;
        case REG_NUM:
            curr_PCB[curr_rid]->fd_arr[idx].file_ops = &fs_fops;
            break;
        default: return -1;
    }
    curr_PCB[curr_rid]->fd_arr[idx].file_ops->open((int8_t *)filename);
    //return the result of idx
    return idx;
}

/*
system_write()
    INPUT: 
        - fd: idx of fd in fd_array
        - buf: buf that contains data needs to be written
        - nbytes: num of btyes to write
    OUTPUT:
        - return val for different write function in jumptable
    DESCRIPTION:
        - do write operation to the file in fd array
*/
int32_t system_write(int32_t fd, const void * buf, int32_t nbytes){
    int flags;
    int write_return;
    cli_and_save(flags);
    // NULL check
    if(buf == NULL) return -1;  // invalid input buffer
    if(fd < 0 || fd > END_FILE_IDX) return -1; // invalid fd idx (0-7 supposely)
    if(curr_PCB[curr_rid]->fd_arr[fd].flags == 0) return -1; // unpresent file
    // "regular file" check not applied here, all included in filesystem.c
    write_return = curr_PCB[curr_rid]->fd_arr[fd].file_ops->write(fd, buf, nbytes);
    restore_flags(flags);
    sti();
    return write_return;
}


/*
system_close()
    INPUT: 
        - fd: idx of fd in fd_array
    OUTPUT:
        - return val for different close function in jumptable
    DESCRIPTION:
        - do close operation to the file in fd array
*/
int32_t system_close(int32_t fd){
    int close_return;
    // NULL check
    if(fd < START_FILE_IDX || fd > END_FILE_IDX) return -1; // invalid fd idx (0-7 supposely)
    if(curr_PCB[curr_rid]->fd_arr[fd].flags == 0) return -1; // unpresent file
    // "regular file" check not applied here, all included in filesystem.c
    close_return = curr_PCB[curr_rid]->fd_arr[fd].file_ops->close(fd);
    curr_PCB[curr_rid]->fd_arr[fd].flags = 0;
    return close_return;

}


//helper function list 
/* fd_init
 * Description: initialize the file descriptor
 * Inputs:   pcb: the current pcb
 * Return Value: 0 if success -1 if not
 * Function: set the file descriptors to present*/
int32_t fd_init(PCB* pcb){
    // Ben: unsure about stdin and stdout
    int i;
    pcb->fd_arr[0].file_ops = &stdin_fops;
    pcb->fd_arr[0].flags = 1;  // file present
    pcb->fd_arr[1].file_ops = &stdout_fops;
    pcb->fd_arr[1]. flags= 1;  // file present

    for (i = 2; i < 8; i++){
        pcb->fd_arr[i].flags = 0;
    }
    return 1;
}


/* fd_close
 * Description: close the file descriptor
 * Inputs:   pcb: the current pcb
 * Return Value: 0 if success -1 if not
 * Function: set the file descriptors to unpresent*/
int32_t fd_close(PCB* pcb){
    // Ben: unsure about stdin and stdout
    int i;
    pcb->fd_arr[0].file_ops = NULL;
    pcb->fd_arr[0].flags = 0;  // file present
    pcb->fd_arr[1].file_ops = NULL;
    pcb->fd_arr[1].flags = 0;  // file present

    // mark all fds as not present
    for (i = 2; i < 8; i++){
        if (pcb->fd_arr[i].flags){
            pcb->fd_arr[i].file_ops->close(i);
            pcb->fd_arr[i].flags = 0;
        }
    }
    return 1;
}

/* is_executable
 * Description: check whether the file is executable
 * Inputs:   filename: the filename to be checked
 * Return Value: 0 if success -1 if not
 * Function: check the magic numbers*/
int32_t is_executable(int8_t* filename){
    int32_t read_data_retVal;
    int i;
    // uint32_t flags; unused variable
    if (!filename) return -1;

    dir_entry new_dir_entry;
    // read_by_name_retVal = read_dentry_by_name(filename, &new_dir_entry);
    // // read file unsucessful
    // if(read_by_name_retVal == -1) return -1;
    if (read_dentry_by_name(filename, &new_dir_entry) == -1) {
        return -1;
    }
    
    uint8_t buf[4];
    read_data_retVal = read_data(new_dir_entry.inode_num, 0, buf, 4);
    if(read_data_retVal == -1) return -1;

    // check each byte in the first 4 byte corresponds to executable magic number
    for(i = 0; i < 4; i++){
        if(buf[i] != magic_num[i]){
            return -1;
        }
    }
    // // If read was successful, then advance the cursor the number of bytes read
    // fd->file_position += read_data_retVal;
    
    return 0;
}


/* argument_parser
 * Description: parse the command
 * Inputs:   command: the command to be parsed
             arg: the argument of the command
             filename: the filename for operation
 * Return Value: 0 if success -1 if not
 * Function: set the filename and argument buffer*/
int32_t argument_parser(const int8_t* command, uint8_t* arg, int8_t* filename){
    int cmdIdx;
    int isSep = 0;
    if(command == NULL) return -1; // NULL CHECK
    // check filename in first section
    for(cmdIdx = 0; cmdIdx < strlen((char*)command); cmdIdx++){
        if(command[cmdIdx] == ' '){
            isSep = 1;
            break;
        }else{
            if(command[cmdIdx] == '\0' || command[cmdIdx] == '\n'){
                break;
            }            
            filename[cmdIdx] = command[cmdIdx];
        }
    }
    
    cmdIdx ++;
    if(isSep != 0 && strlen((int8_t *)(command + cmdIdx)) <= 32){
        strcpy((int8_t *)arg, (int8_t *)(command + cmdIdx));
        return 0;
    }
    return -1;
}



/* flush_TLB
 * Description: flush the tlb
 * Inputs:  None
 * Return Value: None
 * Function: reload the cr3 register*/
void flush_TLB(){
    asm volatile(
            "mov %%cr3, %%eax    \n\
            mov %%eax, %%cr3     \n"
            :
            :
            : "eax"
        );
}

/* system_getargs
 * Description: get the arguments from the command
 * Inputs:  - buf: buf that contains data needs to be written
            - nbytes: num of btyes to write
 * Return Value:0 if success -1 if not
 * Function: copy the arguments into the buffer
 */
int32_t system_getargs(uint8_t * buf, int32_t nbytes){
    //check conditions
    if(buf== NULL) return -1;
    if(nbytes < 0) return -1;
    //check if argument is passed or not
    if(!(curr_PCB[curr_rid] ->arg[0])) return -1;
    //copy the arg into the buf
    strncpy((int8_t*)buf,(int8_t*)curr_PCB[curr_rid]->arg,(uint32_t)nbytes);
    return 0;
}

/* system_vidmap
 * Description: set up page for user accessible video memory
 * Inputs:  -screen_start: the address to store the starting address of video memory
 * Return Value:0 if success -1 if not
 * Function: set up pages in page directory table
 */
int32_t system_vidmap(uint8_t ** screen_start){
    if ((uint32_t)screen_start < PROGRAM_EXE_START || (uint32_t)screen_start > PROGRAM_EXE_START + MB_4 - sizeof(uint8_t*)){
        return -1;
    }

    set_video_page(screen_start);
    return 0;
}

/* system_set_handler
 * Description: reserve for signal, return -1 for default
 */
int32_t system_set_handler(int32_t signum, void * handler_address){
    return -1;
}

/* system_sigreturn
 * Description: reserve for signal, return -1 for default
 */
int32_t system_sigreturn(void){
    return -1;
}

/* get_cur_pid
 * Description: get the current free pid for the curr_PCB array
 */
uint8_t get_cur_pid(void) {
    uint8_t i;
    for(i = 0; i < MAX_PROCESS_NUM; i++) {
        if (process_id[i] == 0) {
            process_id[i] = 1;
            return i;
        } 
    }
    return FK_UP;

}

/* delete_pid
 * Description: set the pid to be 0 as not being used 
 */
void delete_pid(uint8_t p_id) {
    if (process_id[p_id] == 1) process_id[p_id] =0;
}



/* set_pit_flag
 * Description: set the pit flag to be 1
 */
void set_pit_flag(){
    pit_flag = 1;
}

