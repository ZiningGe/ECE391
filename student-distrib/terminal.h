#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"
#include "keyboard.h"
#include "page.h"
#include "lib.h"
#include "systemcall.h"

#define TERMINAL_MAX_COL    80
#define TERMINAL_MAX_ROW    25


uint8_t curr_tid;

typedef struct terminal
{
    /* data */
    int32_t currX;
    int32_t currY;
}terminal;

struct terminal currTerminal[3];

/*
terminal_init
    Description: Initialize terminal struct variable and keyboard buffer
    Inputs:  NONE
    Outputs: NONE
*/
void terminal_init ();


/*
terminal_open
    Description: open the file through fd
    Inputs: 
        - fd: 
            file decriptor
        - buf: 
            pointer to a buffer
        - nbytes:
            number of bytes going to be transferred to buf
    Outputs:
        - 0 for success -1 for error:

*/
int32_t terminal_open (const uint8_t* filename);


/*
terminal_close
    Description: close the current opened file
    Inputs: 
        - fd: 
            file decriptor
        - buf: 
            pointer to a buffer
        - nbytes:
            number of bytes going to be transferred to buf
    Outputs:
        - 0 for success -1 for error:
*/
int32_t terminal_close (int32_t fd);

/*
terminal_read
    Description: read from the keyboard buffer into buf, return number of bytes read
    Inputs: 
        - fd: 
            file decriptor
        - buf: 
            pointer to a buffer
        - nbytes:
            number of bytes going to be transferred to buf
    Outputs:
        - 0:

*/
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes);


/*
terminal_write
    Description: read from the keyboard buffer into buf, return number of bytes read
    Inputs: 
        - fd: 
            file decriptor
        - buf: 
            pointer to a buffer which contains data provided from 
            terminal_read, ready to be displayed
        - nbytes:
            number of bytes going to be displayed
    Outputs:
        - 0:

*/
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes);




// ================ HELPER FUNCTIONS ================

/*
terminal_clear
    Description: clear the terminal and reset the cursor
    Inputs: 
        NONE
    Outputs:
        NONE
*/
void terminal_clear();

/* void terminal_putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void terminal_putc(uint8_t c);


/* 
    void terminal_backspace();
    Description: clear video mem, reset cursor
    Inputs: NONE
    Return: NONE
    Effect: -
 */
void terminal_backspace();

/* 
    void cursor_update();
    Description: update cursor to x, y location
    Inputs: NONE
    Return Value: NONE
    Effect: cursor will be displayed to x,y location in screen
 */
void cursor_update(int32_t x, int32_t y);


/* void clear_and_reset(void);
 * Inputs: NONE
 * Return: NONE
 * Function: Clears video memory */
void clear_and_reset(void);


/* void terminal_scroll_up(void);
 * Inputs: NONE
 * Return: NONE
 * Function: upshift a block of video memory to leave space for last line's printing */
void terminal_scroll_up(void);


/* void terminal_puts(char* s);
 * Inputs: string to be printed
 * Return Value: void
 *  Function: Output string */
int32_t terminal_puts(int8_t *s);

/* void terminal_printf(int8_t *format, ...);
 * Inputs: int8_t *format, ...
 * Return Value: int32_t
 *  Function: output any type of val, modified from printf() */
int32_t terminal_printf(int8_t *format, ...);

/* void clear_and_reset_cl(void);
 * Inputs: NONE
 * Return: NONE
 * Function: Clears video memory */
void clear_and_reset_cl(void);



uint8_t get_current_video(void);

void mem_switch(uint8_t prev_vid, uint8_t new_vid);


uint8_t* get_current_write_addr();

void set_current_video(uint8_t new_tid);

#endif
