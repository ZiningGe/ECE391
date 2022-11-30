#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "lib.h"
#include "types.h"
#include "filesystem.h"
#include "x86_desc.h"
#include "rtc.h"
#include "terminal.h"


#define KERNEL_MAX 0x00800000
#define KERNEL_PROCESS_SIZE 0x0002000
#define EXECUTABLE_ENTRY 24
#define FILENAME_SIZE 32
#define PROGRAM_EXE_START 0x08000000
#define PROGRAM_START 0x08048000
#define MB_4    4194304
#define START_FILE_IDX 2
#define END_FILE_IDX 7
#define MAX_FILE_SIZE 8
#define RTC_NUM 0
#define DIR_NUM 1
#define REG_NUM 2
#define HEADBUF_SIZE 40
#define MAGNUM1 24
#define MAGNUM2 25
#define MAGNUM3 26
#define MAGNUM4 27
#define USER_BOTTOM 0x083FFFFC
#define TERMINAL_NUM 3
#define MAX_PROCESS_NUM 6
#define FK_UP 255



// typedef read_fn_t 
// typedef int32_t *read_fn_t ;
// typedef int32_t (*open_fn)(const int8_t *);
// typedef int32_t (*read_fn)(int);
// typedef int32_t (*write_fn)(int32_t, const void *, int32_t);
// typedef int32_t (*close_fn)(int32_t);
// structure for file system abstraction

// typedef struct fops_jmp_table {
//     open_fn open;
//     read_fn read;
//     write_fn write;
//     close_fn close;
// } fops_jmp_table_t;

typedef struct fops_jmp_table {
    int32_t (*open)(const int8_t *);
    int32_t (*read)(int32_t, void *, int32_t);
    int32_t (*write)(int32_t, const void *, int32_t);
    int32_t (*close)(int32_t);
} fops_jmp_table_t;



typedef struct fd {
    fops_jmp_table_t *file_ops;
    uint32_t inode;
    uint32_t file_position;
    uint32_t flags; // indicate presence in lowest bit   
} fd_t;

typedef struct PCB {
  // pointer to the fd containing 8 files each process
  fd_t fd_arr[8]; 
  struct PCB *father; //pointer to the parent of current PCB
  char arg[33];
  int32_t ebp; 
  int32_t esp;
  int32_t ebp2;
  int32_t esp2; 
  int8_t pid;
  int8_t isActive;
} PCB;


int32_t sys_halt(uint8_t status);
int32_t sys_execute(const uint8_t * command);
int32_t read(int32_t fd, void * buf, int32_t nbytes);
int32_t write(int32_t fd, const void * buf, int32_t nbytes);
int32_t open(const uint8_t * filename);
int32_t close(int32_t fd);
int32_t getargs(uint8_t * buf, int32_t nbytes);
int32_t vidmap(uint8_t ** screen_start);
int32_t set_handler(int32_t signum, void * handler_address);
int32_t sigreturn(void);




int32_t system_halt(uint8_t status);
int32_t system_execute(const uint8_t * command);
int32_t system_read(int32_t fd, void * buf, int32_t nbytes);
int32_t system_write(int32_t fd, const void * buf, int32_t nbytes);
int32_t system_open(const uint8_t * filename);
int32_t system_close(int32_t fd);
int32_t system_getargs(uint8_t * buf, int32_t nbytes);
int32_t system_vidmap(uint8_t ** screen_start);
int32_t system_set_handler(int32_t signum, void * handler_address);
int32_t system_sigreturn(void);
int32_t system_is_executable(char* filename);

// ---- Helper functions ----

int32_t argument_parser(const int8_t* command, uint8_t* arg, int8_t* filename);
void jump_iret(uint32_t entry_point, uint16_t ds);
void jump_to_execute_end(int32_t status, int32_t prev_ebp, int32_t prev_esp);
int32_t is_executable(int8_t* filename);
int32_t fd_init(PCB* pcb);
int32_t fd_close(PCB* pcb);
void flush_TLB();
void set_pit_flag();
// PCB* get_pcb();



PCB* pre_PCB[TERMINAL_NUM];
PCB* curr_PCB[TERMINAL_NUM];
uint32_t curr_rid;


#endif
