#ifndef _FS_H
#define _FS_H
#include "types.h"
#include "systemcall.h"
#define DIR_ENTRY_SIZE 64
#define FILENAME_SIZE 32
#define DATA_BLOCK_NUM_SIZE 1023
#define DATA_BLOCK_SIZE 4096
#define REGULAR 2


// structure for file system utility and abstraction
typedef struct dir_entry {
    int8_t file_name[FILENAME_SIZE];
    int32_t file_type;
    int32_t inode_num;
    int8_t reserved[24]; 
} dir_entry;

typedef struct boot_block {
    int32_t dir_entry_num;
    int32_t inodes_num;
    int32_t data_blocks_num;
    int8_t reserved[52]; 
    dir_entry dir_entries_63[DIR_ENTRY_SIZE - 1];
} boot_block;

typedef struct inode {
    int32_t length;
    int32_t data_block_num[DATA_BLOCK_NUM_SIZE];
} inode;

typedef struct data_block {
    uint8_t data[DATA_BLOCK_SIZE];
} data_block;






// dir_entry new_entry;
dir_entry new_dir_entry;
// uint32_t cur_offest;
// static boot_block* bootblock_start = NULL;


// function used for file system
int32_t fs_init(uint32_t boot_block_addr);
int32_t fs_open(const int8_t *filename);
int32_t fs_close (int32_t fd);
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes);
int32_t fs_write (int32_t fd, const void* buf, int32_t nbytes);

int32_t dir_open(const int8_t *filename);
int32_t dir_close (int32_t fd);
int32_t dir_read(int32_t fd, void * buf, int32_t nbytes);
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);

// helper functions
int32_t read_dentry_by_name (const int8_t* fname, dir_entry* dentry);
int32_t read_dentry_by_index (uint32_t index, dir_entry* dentry);
int32_t read_data (uint32_t inode_idx, uint32_t offset, uint8_t* buf, uint32_t length);
boot_block* getboot_block();
#endif




