#ifndef _PIT_H
#define _PIT_H
#include "types.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"
#include "lib.h"
#include "page.h"
#include "systemcall.h"
#include "types.h"
#include "x86_desc.h"
#include "terminal.h"

#define PIT_COMMAND 0x43
#define PIT_DATA 0x40
#define MODE3 0x36
#define FREQUENCY 5120
#define PIT_IRQ_NUM 0
#define TERMINAL_SIZE 3

void PIT_init();
void scheduler();
void PIT_interrupt_handler();

#endif 
