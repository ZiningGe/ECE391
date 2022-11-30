#include "terminal.h"
#include "keyboard.h"
#include "systemcall.h"
#include "page.h"



//static char* video_mem = (char *)VIDEO;
// static struct terminal currTerminal[TERMINAL_NUM];

// 0 is the actual video mem, 1,2,3 stands for copy of terminal 0, 1, 2
/*
terminal_init
    Description: Initialize terminal struct variable and keyboard buffer
    Inputs:  NONE
    Outputs: NONE
*/
void terminal_init(){
    currTerminal[curr_tid].currX = 0;
    currTerminal[curr_tid].currY = 0;
}




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
int32_t terminal_open (const uint8_t* filename){
    return 0;
}


/*
terminal_close
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
int32_t terminal_close (int32_t fd){

    return 0;
}

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
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    //terminal_putc('r');
    keyboardBufferStruct temp;
    // input parameter edge cases:
    if(nbytes < 0){ // invalid nbytes
        return -1;
    }else if(nbytes >= MAX_SIZE_KEYBOARD_BUFFER){
        nbytes = MAX_SIZE_KEYBOARD_BUFFER - 1;
    }

    // reset buffer and flag in keyboard buffer
    init_keyboard_buffer();

    //  keep fetching keyboard buffer value
    while(1){
        temp = *(get_keyboard_buffer());
        if(temp.readFinish == 1){
            break;
        }
    }

    memcpy(buf, &(temp.charBuffer), temp.lastIdx);
    //init_keyboard_buffer();
    return temp.lastIdx;
}

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
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    // input parameter edge cases
    int i;
    const char* tempBuf = (const char *)buf;
    /* only write to where is curently active */
    curr_tid = curr_rid;
    if(buf == NULL){
        return -1;
    }
    for(i = 0; i < nbytes; i++){
        if(tempBuf[i] == '\t'){
            terminal_putc(' ');
            terminal_putc(' ');
            terminal_putc(' ');
            terminal_putc(' ');
        }else{
            terminal_putc(tempBuf[i]);
        }
    }
    return i;
}



// ================ HELPER FUNCTIONS ================

/*
terminal_clear
    Description: clear the terminal and reset the cursor
    Inputs: 
        NONE
    Outputs:
        NONE
*/
void terminal_clear(void){
    clear_and_reset();
    // more code to be done
}


/* 
    void terminal_backspace();
    Description: clear video mem, reset cursor
    Inputs: NONE
    Return: NONE
    Effect: -
 */
void terminal_backspace() {
    if(currTerminal[curr_tid].currX == 0) {
        // back to previous line
        if(currTerminal[curr_tid].currY > 0){
            currTerminal[curr_tid].currX = NUM_COLS - 2;
            currTerminal[curr_tid].currY --;
        }
    } else {    
        // simply deduct the position of x
        currTerminal[curr_tid].currX--;
    }    
    // erase from video buffer and update cursor
    *(uint8_t *)(get_current_write_addr() + ((NUM_COLS * currTerminal[curr_tid].currY + currTerminal[curr_tid].currX) << 1)) = '\0';
    *(uint8_t *)(get_current_write_addr() + ((NUM_COLS * currTerminal[curr_tid].currY + currTerminal[curr_tid].currX) << 1) + 1) = ATTRIB;
    cursor_update(currTerminal[curr_tid].currX, currTerminal[curr_tid].currY);    
}


/* 
    void cursor_update();
    Description: update cursor to x, y location
    Inputs: NONE
    Return Value: NONE
    Effect: cursor will be displayed to x,y location in screen
 */
void cursor_update(int32_t x, int32_t y){
    if (curr_kid != curr_tid) return;
    uint8_t pos_h = ((y * NUM_COLS + x) & 0xFF);
    uint8_t pos_l = (((y * NUM_COLS + x) >> 8) & 0xFF);

    outb(CURSOR_REG_H,VGA_HOR_REG_0);
    outb(pos_h, VGA_HOR_REG_1);
    outb(CURSOR_REG_L,VGA_HOR_REG_0);
    outb(pos_l, VGA_HOR_REG_1);
}

/* void clear_and_reset(void);
 * Inputs: NONE
 * Return: NONE
 * Function: Clears video memory */
void clear_and_reset(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(get_current_write_addr() + (i << 1)) = '\0';
        *(uint8_t *)(get_current_write_addr() + (i << 1) + 1) = ATTRIB;
    }
    // reset cursor and terminal x,y
    currTerminal[curr_tid].currX = 0;
    currTerminal[curr_tid].currY = 0;
    cursor_update(0,0);
}

/* void clear_and_reset(void);
 * Inputs: NONE
 * Return: NONE
 * Function: Clears video memory */
void clear_and_reset_cl(void) {
    int32_t i;
    keyboardBufferStruct temp;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(get_current_write_addr() + (i << 1)) = '\0';
        *(uint8_t *)(get_current_write_addr() + (i << 1) + 1) = ATTRIB;
    }
    // reset cursor and terminal x,y
    currTerminal[curr_tid].currX = 0;
    currTerminal[curr_tid].currY = 0;
    cursor_update(0,0);
    int8_t *init_keyword = "391OS> ";
    terminal_printf(init_keyword);
    temp = *(get_keyboard_buffer_kid());
    terminal_printf(temp.charBuffer);
    
}

/* void terminal_scroll_up(void);
 * Inputs: NONE
 * Return: NONE
 * Function: upshift a block of video memory to leave space for last line's printing */
void terminal_scroll_up(void){
    int i;
    
    int firstSectionByte = 2 * (NUM_ROWS-1) * NUM_COLS;
    /*
    move all memory from the second to last row to the beginning of videomem
    since each char is 2 bytes, total number of bytes is 2*(NUM_COLS * (NUM_ROWS - 1))
    */
    memmove(get_current_write_addr(), get_current_write_addr() + 2 * NUM_COLS, firstSectionByte);

    // clear the last line in videomem and reset the cursor to the beginning of it
	for (i = 0; i < NUM_COLS; ++i){
        *(uint8_t *)(get_current_write_addr() + firstSectionByte + 2 * i) = '\0';
        *(uint8_t *)(get_current_write_addr() + firstSectionByte + 2 * i + 1) = ATTRIB;
    }

    // update curr x,y and cursor
    currTerminal[curr_tid].currX = 0;
    currTerminal[curr_tid].currY = NUM_ROWS - 1;
    cursor_update(currTerminal[curr_tid].currX, currTerminal[curr_tid].currY);
}


/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console 
 * */
void terminal_putc(uint8_t c) {
    if(c == '\0') return;
    if(c == '\n' || c == '\r') {
        if(currTerminal[curr_tid].currY == NUM_ROWS - 1){
            terminal_scroll_up();
            return;
        }else{
            currTerminal[curr_tid].currY ++;
            currTerminal[curr_tid].currX = 0;
        }
    } else {
        // first modify video mem with current x, y
        *(uint8_t *)(get_current_write_addr() + ((NUM_COLS * currTerminal[curr_tid].currY + currTerminal[curr_tid].currX) << 1)) = c;
        *(uint8_t *)(get_current_write_addr() + ((NUM_COLS * currTerminal[curr_tid].currY + currTerminal[curr_tid].currX) << 1) + 1) = ATTRIB;
        
        // start looking for next position of printing
        if(currTerminal[curr_tid].currX == NUM_COLS-1 && currTerminal[curr_tid].currY < NUM_ROWS - 1 ){
            // end of the line but not bottom line
            currTerminal[curr_tid].currX = 0;
            currTerminal[curr_tid].currY ++;
        } else if(currTerminal[curr_tid].currX == NUM_COLS-1 && currTerminal[curr_tid].currY == NUM_ROWS - 1 ){
            //  end of the line and is the bottom line
            currTerminal[curr_tid].currX = 0;
            terminal_scroll_up();
        } 
        else {
            // regular shift position to the right one
            currTerminal[curr_tid].currX++;
            currTerminal[curr_tid].currX %= NUM_COLS;
            currTerminal[curr_tid].currY = (currTerminal[curr_tid].currY + (currTerminal[curr_tid].currX / NUM_COLS)) % NUM_ROWS;
        }
    }
    // set the cursor position at the end
    cursor_update(currTerminal[curr_tid].currX, currTerminal[curr_tid].currY);
}


/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t terminal_puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        terminal_putc(s[index]);
        index++;
    }
    return index;
}

/* void terminal_printf(int8_t *format, ...);
 * Inputs: int8_t *format, ...
 * Return Value: int32_t
 *  Function: output any type of val, modified from printf() */
int32_t terminal_printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            terminal_putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((int32_t *)esp), conv_buf, 16);
                                    terminal_puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((int32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    terminal_puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((int32_t *)esp), conv_buf, 10);
                                terminal_puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                terminal_puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            terminal_putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            terminal_puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                terminal_putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}


void set_current_video(uint8_t new_tid){
    curr_tid = new_tid;
}

uint8_t get_current_video(void){
    return curr_tid;
}


void mem_switch(uint8_t prev_vid, uint8_t new_vid){
    copy_video_page(prev_vid+1, 0);
    copy_video_page(0, new_vid+1);
    
}

uint8_t* get_current_write_addr(){
    // 0 hardcode activeID = 0 (terminal 0 is actually working)
    if (curr_writing || curr_kid == curr_rid){ 
        return (uint8_t*)VIDEO_MEM_ADDR;
    }
    else{
        return (uint8_t*)(VIDEO_MEM_ADDR + (1+curr_rid) * fourKB); // 0 stands for same thing
    }
}

