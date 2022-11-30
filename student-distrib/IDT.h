#ifndef _IDT
#define _IDT

#include "x86_desc.h"
#include "i8259.h"
#include "keyboard.h"
#include "lib.h"
#include "idt_linkage.h"
#include "rtc.h"
#include "PIT.h"

#define SYSTEMCALL 0x80
// function list
extern void populate_IDT();
extern uint32_t systemcall_handler(void);
// void Divide_By_Zero();
// void Debug_Exception();
// void NMI_Ipoint_Exception();
// void Overfnterrupt();
// void Breaklow_Exception();
// void Bound_Range_Exceeded_Exception();
// void Invalid_Opcode_Exception();
// void Device_Not_Available_Exception();
// void Double_Fault_Exception();
// void Coprocessor_Segment_Overrun();
// void Invalid_TSS_Exception();
// void Segment_Not_Present();
// void Stack_Fault_Exception();
// void General_Protection_Exception();
// void Page_Fault_Exception();
// void Floating_Point_Error();
// void Alignment_Check_Exception();
// void Machine_Check_Exception();
// void SIMD_Floating_Point_Exception();


// void irq0(void);
                                          

#endif




