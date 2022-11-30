#include "keyboard.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "systemcall.h"
#include "page.h"
#include "terminal.h"

// The array used to extract ascii when there's no function key flag raising
const char scancode_to_ascii_regular[69] = {
    '\0', // first scan code is blank 
    '\0', // no response to ESC
    '1', '2' ,'3', '4', '5', '6', '7', '8', '9', '0',           // scan code [0x02 - 0x0B]: number from 1 - 0
    '-', '=', '\0', '\0',                                      // scan code [0x0D - 0x0F]: -, =, BACKSPACE, TAB
    'q', 'w' ,'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',           // scan code [0x10 - 0x19]: qwertyuiop
    '[', ']', '\n', '\0',                                       // scan code [0x1A - 0x1D]: [, ], rightenter, left ctrl
    'a', 's' ,'d', 'f', 'g', 'h', 'j', 'k', 'l',                // scan code [0x1E - 0x26]: asdfghjkl
    ';', '\'', '`', '\0', '\\',                                 // scan code [0x27 - 0x2B]: ;, ', `, leftshift, '\'
    'z', 'x' ,'c', 'v', 'b', 'n', 'm',                          // scan code [0x2C - 0x32]: 
    ',', '.' ,'/', '\0', '*', '\0',  ' ', '\0',                 // scan code [0x33 - 0x3A]: , . / rightshift * leftAlt, space CapsLock
    '\0', '\0' ,'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' // scan code [0x3B - 0x44]: number from F1-F10
};

// The array used to extract ascii when CapsLock is setted
const char scancode_to_ascii_cap[69] = {
    '\0', // first scan code is blank 
    '\0', // no response to ESC
    '1', '2' ,'3', '4', '5', '6', '7', '8', '9', '0',           // scan code [0x02 - 0x0B]: number from 1 - 0
    '-', '=', '\0', '\0',                                      // scan code [0x0D - 0x0F]: -, =, BACKSPACE, TAB
    'Q', 'W' ,'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',           // scan code [0x10 - 0x19]: qwertyuiop
    '[', ']', '\n', '\0',                                       // scan code [0x1A - 0x1D]: [, ], rightenter, left ctrl
    'A', 'S' ,'D', 'F', 'G', 'H', 'J', 'K', 'L',                // scan code [0x1E - 0x26]: asdfghjkl
    ';', '\'', '`', '\0', '\\',                                 // scan code [0x27 - 0x2B]: ;, ', `, leftshift, '\'
    'Z', 'X' ,'C', 'V', 'B', 'N', 'M',                          // scan code [0x2C - 0x32]: 
    ',', '.' ,'/', '\0', '*', '\0',  ' ', '\0',                 // scan code [0x33 - 0x3A]: , . / rightshift * leftAlt, space CapsLock
    '\0', '\0' ,'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' // scan code [0x3B - 0x44]: number from F1-F10
};

// The array used to extract ascii when shift is setted
const char scancode_to_ascii_shift[69] = {
    '\0', // first scan code is blank 
    '\0', // no response to ESC
    '!', '@' ,'#', '$', '%', '^', '&', '*', '(', ')',           // scan code [0x02 - 0x0B]: number from 1 - 0
    '_', '+', '\0', '\0',                                      // scan code [0x0D - 0x0F]: -, =, BACKSPACE, TAB
    'Q', 'W' ,'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',           // scan code [0x10 - 0x19]: qwertyuiop
    '{', '}', '\n', '\0',                                       // scan code [0x1A - 0x1D]: [, ], rightenter, left ctrl
    'A', 'S' ,'D', 'F', 'G', 'H', 'J', 'K', 'L',              // scan code [0x1E - 0x26]: asdfghjkl
    ':', '"', '~', '\0', '|',                                 // scan code [0x27 - 0x2B]: ;, ', `, leftshift, '\'
    'Z', 'X' ,'C', 'V', 'B', 'N', 'M',                         // scan code [0x2C - 0x32]: 
    '<', '>' ,'?', '\0', '*', '\0',  ' ', '\0',                 // scan code [0x33 - 0x3A]: , . / rightshift * leftAlt, space CapsLock
    '\0', '\0' ,'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0' // scan code [0x3B - 0x44]: number from F1-F10
};


// instantiate flag and buffer struct
static struct keyboardFlagsStruct keyboardFlags[TERMINAL_NUM];
static struct keyboardBufferStruct keyboardBuffer[TERMINAL_NUM];



/*
function: init_keyboard:
    DESCRIPTION:
        - call enable_irq() to keyboard port, enable interrupt of keyboard
          and initialize flags
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - Initialize keyboard interrupt and flags of function keys
*/
void init_keyboard(){
    int i;
    curr_tid = 0;
    //enable keyboard interrupt
    enable_irq(KEYBOARD_IRQ);

    //initialize keyboard flags;
    keyboardFlags[curr_rid].alt = 0;
    keyboardFlags[curr_rid].shift = 0;
    keyboardFlags[curr_rid].ctrl = 0;
    keyboardFlags[curr_rid].caps = 0;

    // initialize keyboard buffer
    keyboardBuffer[curr_rid].lastIdx = 0;
    for(i = 0; i < MAX_SIZE_KEYBOARD_BUFFER; i++){
        keyboardBuffer[curr_rid].charBuffer[i] = '\0';
    }
}


/*
function: init_keyboard_buffer:
    DESCRIPTION:
        - initialize the keyboard buffer, called when terminal_read is executing
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - Initialize all variables in keyboard buffer struct
*/
extern void init_keyboard_buffer(){
    int i;
    keyboardBuffer[curr_rid].lastIdx = 0;
    for(i = 0; i < MAX_SIZE_KEYBOARD_BUFFER; i++){
        keyboardBuffer[curr_rid].charBuffer[i] = '\0';
    }
    keyboardBuffer[curr_rid].readFinish = 0;
    init_keyboard();
}


/*
function: get_keyboard_buffer:
    DESCRIPTION:
        - get the pointer of keyboard buffer, used for getting buffer values and readFinish flag
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - Help terminal_read to read correct keyboard value
*/
struct keyboardBufferStruct* get_keyboard_buffer(){
    return &keyboardBuffer[curr_rid];
}

struct keyboardBufferStruct* get_keyboard_buffer_kid(){
    return &keyboardBuffer[curr_kid];
}


/*
function: scancode_to_ascii:
    DESCRIPTION:
        - Outputs an 2 bytes ASCII from input scancode value and global flags 
    INPUT: 
        - uint8_t scancode: the received 2 byte code from keyboard port 0x60
    RETURN:
        - the corresponding ascii code to the scancode and current flags
    EFFECTS:
        - None
*/
char scancode_to_ascii(uint8_t scancode){
    
    // simplify extra flag logic for checkpoint 1
    char ascii;

    // situation of different flags raising
    ascii = scancode_to_ascii_regular[scancode];

    if(keyboardFlags[curr_kid].caps == 1 && keyboardFlags[curr_kid].shift == 0){    //  only cap is on
        ascii = scancode_to_ascii_cap[scancode];
    }else if(keyboardFlags[curr_kid].caps == 1 && keyboardFlags[curr_kid].shift == 1){  // both on, prefer shift
        ascii =  scancode_to_ascii_shift[scancode];
    }else if(keyboardFlags[curr_kid].caps == 0 && keyboardFlags[curr_kid].shift == 1){  // only shift on
        ascii =  scancode_to_ascii_shift[scancode];
    }
    
    return ascii;

}


/*
function: flags_handler:
    DESCRIPTION:
        - A check process within keyboard handler, set up corresponding function key's flag
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - change the ascii that print to terminal
*/
extern uint8_t flags_handler(uint8_t scancode){
    //  setup template to deal with flags
    int isChanged = 0;
    switch (scancode)
    {
    case 0x00 /* null */:
        /* code */
       isChanged = 1;
       break;

    case 0x01 /* ESC */:
        /* code */
       isChanged = 1;
       break;
    
    // left ctrl press and release
    case 0x1D /* left ctrl */:
        /* code */
        keyboardFlags[curr_kid].ctrl = 1;
        isChanged = 1;
        break;
    case 0x9D /* left ctrl */:
        /* code */
        keyboardFlags[curr_kid].ctrl = 0;
        isChanged = 1;
        break;
    
    
    // left shift press and release
    case 0x2A	 /* left shift */:
        /* code */
        keyboardFlags[curr_kid].shift = 1;
        isChanged = 1;   
        break;
    case 0xAA	 /* left shift */:
        /* code */
        keyboardFlags[curr_kid].shift = 0;
        isChanged = 1;   
        break;


    // right shift press and release
    case 0x36	 /* right shift */:
        /* code */
        keyboardFlags[curr_kid].shift = 1;
        isChanged = 1;   
        break;
    case 0xB6	 /* right shift */:
        /* code */
        keyboardFlags[curr_kid].shift = 0;
        isChanged = 1;   
        break;

    // right shift press and release
    case 0x38 /* left alt */:
        /* code */
        keyboardFlags[curr_kid].alt = 1;
        isChanged = 1;   
        break;
    case 0xB8 /* left alt */:
        keyboardFlags[curr_kid].alt = 0;
        isChanged = 1;   
        break;
    

    case 0x3A /* CapsLock  */:
        /* code */
        if(keyboardFlags[curr_kid].caps == 0){
            keyboardFlags[curr_kid].caps = 1;
        }else{
            keyboardFlags[curr_kid].caps = 0;
        }
        isChanged = 1;   
        break;

       
    default:
        isChanged = 0;
        break;
    }

    return isChanged;

}


/*
function: shortcut_handler:
    DESCRIPTION:
        - A handler to do certain functions when short cut is received
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - various short-cut are going to 
*/
extern uint8_t shortcut_handler(uint8_t scancode){
    //  TODO: ATL+F1/2/3
    //terminal_printf("sc handler");
    if(keyboardFlags[curr_kid].ctrl == 1){
        switch (scancode)
        {
        case 0x26:  // ctrl + l: clear screen
            // terminal_printf("ctrl L");
            clear_and_reset_cl();   
            keyboardFlags[curr_kid].ctrl = 0;    
            return 1;     
            break;


        default:
            break;
        }
    }
    
    if(keyboardFlags[curr_kid].alt == 1){
        // terminal_printf("alt flag");
        switch (scancode)
        {
        case 0x02:  // F1: Switch to T1
            switch_handler(0);
            //terminal_printf("received switch to 0");
            break;
        case 0x03:  // F1: Switch to T1
            switch_handler(1);
            //terminal_printf("received switch to 1");
            break;
        case 0x04:  // F1: Switch to T1
            switch_handler(2);
            //terminal_printf("received switch to 2");
            break;


        default:
            break;
        }
    }
    return 0;
}


/*
function: switch_handler:
    DESCRIPTION:
        - A handler when terminal switch called by keyboard
    INPUT: 
        - target_kid:
            the target terminal we want to see
    RETURN:
        - N/A
    EFFECTS:
        - do memcopy for memory switch, update video page for user video mem, change current kid and tid
*/
extern void switch_handler(uint8_t target_kid){

    //  curr terminal id switch
    mem_switch(get_current_video(), target_kid);
    //  switch current display terminal
    update_video_page_switch(target_kid, get_current_video());
    set_current_video(target_kid);
    curr_kid = target_kid;
}


/*
function: functional_key_handler:
    DESCRIPTION:
        - A handler to do certain functions when single functional key (tab, enter) is received
    INPUT: 
        - scancode:
            to decide which function going to be runned accordingly
    RETURN:
        - 1 for any flag changes, handler quit
        - 0 for nothing happened, keep on checking
    EFFECTS:
        - various performance which differs from regular key will be executed 
*/
uint8_t functional_key_handler(uint8_t scancode){
    int isChanged = 0;
    int i;
    switch (scancode)
    { 
    case 0x0F:  //  TAB
        if(keyboardBuffer[curr_kid].lastIdx < MAX_SIZE_KEYBOARD_BUFFER - 1){
            for(i = 0; i < 4; i++){ // print 4 spaces on screen
                terminal_putc(' ');
            }
            // fill in the buffer by a single '\t'
            keyboardBuffer[curr_kid].charBuffer[keyboardBuffer[curr_kid].lastIdx] = '\t';
            keyboardBuffer[curr_kid].lastIdx ++;
            isChanged = 1;
            
        }  
        break;
    
    case 0x1C:  // ENTER
        if(keyboardBuffer[curr_kid].lastIdx < MAX_SIZE_KEYBOARD_BUFFER){
            terminal_putc('\n');  
            keyboardBuffer[curr_kid].charBuffer[keyboardBuffer[curr_kid].lastIdx] = '\n';
            keyboardBuffer[curr_kid].lastIdx ++;
            keyboardBuffer[curr_kid].readFinish = 1; // update readFinish flag, allow next step for terminal_read()
            isChanged = 1;
        }
        break;

    case  0x0E: // BACKSPACE
        if(keyboardBuffer[curr_kid].lastIdx > 0){
            keyboardBuffer[curr_kid].lastIdx --;
            if(keyboardBuffer[curr_kid].charBuffer[keyboardBuffer[curr_kid].lastIdx] == '\t'){  // delete 4 char when TAB's '\t' is met
                for(i = 0; i < 4; i++){
                    terminal_backspace();
                }
            }else{
                terminal_backspace();
            }
            keyboardBuffer[curr_kid].charBuffer[keyboardBuffer[curr_kid].lastIdx] = '\0'; // erase the char in buffer
            isChanged = 1;   
        }else{  //  cannot use backspace when current line is empty
            isChanged = 1;
        }  
    
    default:
        break;
    }

    return isChanged;
}


/*
function: keyboard_handler:
    DESCRIPTION:
        - Handle the keyboard interrupt generated by pressing a key. Call helper functions
          to generate a ascii from keyboard input and print to terminal
    INPUT: 
        - NONE
    RETURN:
        - NONE
    EFFECTS:
        - print corresponding character to terminal
*/
extern void keyboard_handler(){
    uint8_t ascii;
    uint8_t flagUpdate = 0;
    uint8_t shortCutUpdate = 0;
    uint8_t functionalKeyUpdate = 0;
    
    cli();

    // ready for any keyboard writing to memory

    /* ensure that what we write is on current displaying frame */
    curr_tid = curr_kid;
    curr_writing = 1;

    // read scancode and get input character
    uint8_t scancode = inb(KEYBOARD_PORT_ADDR);
    // receive flag change, simply update the flag, then print out current buffer content
    // check any flag update
    // terminal_printf("%d", scancode);
    // terminal_printf("current alt flag is: %d \n", keyboardFlags[curr_kid].alt);
    flagUpdate = flags_handler(scancode);
    if(flagUpdate){
        curr_writing = 0;        
        send_eoi(KEYBOARD_IRQ);
        
        sti();
        return;
    }

    // check any short cut activated
    shortCutUpdate = shortcut_handler(scancode);
    if(shortCutUpdate){
        curr_writing = 0;    
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    }

    // check any funcctional key pressed (e.g. TAB, ENTER...)
    functionalKeyUpdate = functional_key_handler(scancode);
    if(functionalKeyUpdate){
        curr_writing = 0;  
        send_eoi(KEYBOARD_IRQ);
        sti();
        return;
    } 

    // obtain ascii based on current flag
    if(scancode <= 0x44){
        ascii = scancode_to_ascii(scancode);    
        if(keyboardBuffer[curr_kid].lastIdx < MAX_SIZE_KEYBOARD_BUFFER - 1){ // only less than 127 can be written in ascii
            // print out, update keyboard buffer
            terminal_putc(ascii);  
            keyboardBuffer[curr_kid].charBuffer[keyboardBuffer[curr_kid].lastIdx] = ascii;
            keyboardBuffer[curr_kid].lastIdx ++;
        }  
    }

    // send eoi
    curr_writing = 0;  
    send_eoi(KEYBOARD_IRQ);
    sti();

}




