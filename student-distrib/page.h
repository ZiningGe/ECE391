#ifndef PAGE_H
#define PAGE_H 

#define PAGE_BOUNDARY 4096
#define PAGE_SIZE 1024
#define KERNEL_ADDRESS 0x00400000
#define KERNEL_BUTTOM 0x00800000
#define VIDEO_MEM_ADDR 0xB8000 // define 
#define VIDEO_MEM_IDX  ((0x003FF000 & VIDEO_MEM_ADDR) >>12)
#define eightMB 0x00800000
#define fourMB 0x400000
#define fourKB 0x1000


#define USER_PROGRAM_START 0x08000000

typedef struct page_dir_entry {
    union {
        uint32_t val;
        //4mb paging
        struct{
            uint32_t p_d                : 1;
            uint32_t rd_wt_d            : 1;
            uint32_t user_sup_d         : 1;
            uint32_t pwt_d              : 1;
            uint32_t pcd_d              : 1;
            uint32_t accessed_d         : 1;
            uint32_t dirty_d            : 1;
            uint32_t page_size_d        : 1;      
            uint32_t g_d                : 1;
            uint32_t avl_d              : 3;
            uint32_t pat_d              : 1;
            uint32_t addr_20_13_d       : 8;
            uint32_t reserved_d         : 1;
            uint32_t addr_31_22_d       : 10;
        }__attribute__((packed)); 
        //4 kb paging
        struct{
            uint32_t p_d                : 1;
            uint32_t rd_wt_d            : 1;
            uint32_t user_sup_d         : 1;
            uint32_t pwt_d              : 1;
            uint32_t pcd_d              : 1;
            uint32_t accessed_d         : 1;
            uint32_t avl_d              : 1;
            uint32_t page_size_d        : 1;      
            uint32_t avl_11_8_d         : 4;
            uint32_t addr_31_12_d       : 20;
        }__attribute__((packed));     
    };
} page_dir_entry;



typedef struct page_table_entry {
    union {
        uint32_t val;
        struct{
            uint32_t p_t                : 1;
            uint32_t rd_wt_t            : 1;
            uint32_t user_sup_t         : 1;
            uint32_t pwt_t              : 1;
            uint32_t pcd_t              : 1;
            uint32_t accessed_t         : 1;
            uint32_t dirty_t            : 1;
            uint32_t pat_t              : 1;
            uint32_t g_t                : 1;
            uint8_t  avl_t              : 3;
            uint32_t addr_31_12_t       : 20;
        }__attribute__((packed));     
    };
} page_table_entry;

// build the array for both two tables
page_dir_entry pde[PAGE_SIZE] __attribute__((aligned (PAGE_BOUNDARY)));
page_table_entry pte[PAGE_SIZE] __attribute__((aligned (PAGE_BOUNDARY)));

page_table_entry video_pte[PAGE_SIZE] __attribute__((aligned (PAGE_BOUNDARY)));


//funtions needed
void init_page();
void enble_page();
void set_process_page(uint32_t pid);
void set_video_page(uint8_t ** screen_start);
void clear_video_page();
void update_video_page_switch(uint32_t dst, uint32_t src);
void copy_video_page(uint32_t dst, uint32_t src);
#endif


