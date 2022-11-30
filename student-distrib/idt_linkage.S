#define ASM 1
#include "x86_desc.h"


#define INTR_LINK(name, func)    \
    .globl name                 ;\
    name:                       ;\
        pushal                  ;\
        pushfl                  ;\
        call func               ;\
        popfl                   ;\
        popal                   ;\
        iret                    ;\

# ASM linkage for IRQ
INTR_LINK(IRQ0, irq0);
INTR_LINK(IRQ1, irq1);
INTR_LINK(IRQ2, irq2);
INTR_LINK(IRQ3, irq3);
INTR_LINK(IRQ4, irq4);
INTR_LINK(IRQ5, irq5);
INTR_LINK(IRQ6, irq6);
INTR_LINK(IRQ7, irq7);
INTR_LINK(IRQ8, irq8);
INTR_LINK(IRQ9, irq9);
INTR_LINK(IRQ10, irq10);
INTR_LINK(IRQ11, irq11);
INTR_LINK(IRQ12, irq12);
INTR_LINK(IRQ13, irq13);
INTR_LINK(IRQ14, irq14);
INTR_LINK(IRQ15, irq15);
# ASM linkage for 0-32 
INTR_LINK(EXCEPTION0, Divide_By_Zero);
INTR_LINK(EXCEPTION1, Debug_Exception);
INTR_LINK(EXCEPTION2, NMI_Ipoint_Exception);
INTR_LINK(EXCEPTION3, Overfnterrupt);
INTR_LINK(EXCEPTION4, Breaklow_Exception);
INTR_LINK(EXCEPTION5, Bound_Range_Exceeded_Exception);
INTR_LINK(EXCEPTION6, Invalid_Opcode_Exception);
INTR_LINK(EXCEPTION7, Device_Not_Available_Exception);
INTR_LINK(EXCEPTION8, Double_Fault_Exception);
INTR_LINK(EXCEPTION9, Coprocessor_Segment_Overrun);
INTR_LINK(EXCEPTION10, Invalid_TSS_Exception);
INTR_LINK(EXCEPTION11, Segment_Not_Present);
INTR_LINK(EXCEPTION12, Stack_Fault_Exception);
INTR_LINK(EXCEPTION13, General_Protection_Exception);
INTR_LINK(EXCEPTION14, Page_Fault_Exception);
INTR_LINK(EXCEPTION16, Floating_Point_Error);
INTR_LINK(EXCEPTION17, Alignment_Check_Exception);
INTR_LINK(EXCEPTION18, Machine_Check_Exception);
INTR_LINK(EXCEPTION19, SIMD_Floating_Point_Exception);


