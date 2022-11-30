#include "IDT.h"

char str0[] ={"Divide_by_zero"};
char str1[] ={"Debug_Exception"};
char str2[] ={"NMI_Ipoint_Exception"};
char str3[] ={"Overfnterrupt"};
char str4[] ={"Breaklow_Exception"};
char str5[] ={"Bound_Range_Exceeded_Exception"};
char str6[] ={"Invalid_Opcode_Exception"};
char str7[] ={"Device_Not_Available_Exception"};
char str8[] ={"Double_Fault_Exception"};
char str9[] ={"Coprocessor_Segment_Overrun"};
char str10[] ={"Invalid_TSS_Exception"};
char str11[] ={"Segment_Not_Present"};
char str12[] ={"Stack_Fault_Exception"};
char str13[] ={"General_Protection_Exception"};
char str14[] ={"Page_Fault_Exception"};
char str16[] ={"Floating_Point_Error"};
char str17[] ={"Alignment_Check_Exception"};
char str18[] ={"Machine_Check_Exception"};
char str19[] ={"SIMD_Floating_Point_Exception"};



/* void irq0()
 * Description: interrupt handler for irq0
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq0(void){
    // printf("interrupt: irq0\n ");
    // while(1);
    PIT_interrupt_handler();
}


/* void irq1()
 * Description: interrupt handler for irq1
 * Inputs:   NA   
 * Return Value: NA
 * Function: call the keyboard handler */
void irq1(void){
    keyboard_handler(); 
}

/* void irq2()
 * Description: interrupt handler for irq2
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq2(void){
    printf("interrupt: irq2\n ");
    while(1);
}

/* void irq3()
 * Description: interrupt handler for irq3
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq3(void){
    printf("interrupt: irq3\n ");
    while(1);
}

/* void irq4()
 * Description: interrupt handler for irq4
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq4(void){
    printf("interrupt: irq4\n ");
    while(1);
}

/* void irq5()
 * Description: interrupt handler for irq5
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq5(void){
    printf("interrupt: irq5\n ");
    while(1);
}

/* void irq6()
 * Description: interrupt handler for irq6
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq6(void){
    printf("interrupt: irq6\n ");
    while(1);
}

/* void irq7()
 * Description: interrupt handler for irq7
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq7(void){
    printf("interrupt: irq7\n ");
    while(1);
}

/* void irq8()
 * Description: interrupt handler for irq8
 * Inputs:   NA   
 * Return Value: NA
 * Function: call the rtc handler */
void irq8(void){
    rtc_handler();
}

/* void irq9()
 * Description: interrupt handler for irq9
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq9(void){
    printf("interrupt: irq9\n ");
    while(1);
}

/* void irq10()
 * Description: interrupt handler for irq10
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq10(void){
    printf("interrupt: irq10\n ");
    while(1);
}

/* void irq11()
 * Description: interrupt handler for irq11
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq11(void){
    printf("interrupt: irq11\n ");
    while(1);
}

/* void irq12()
 * Description: interrupt handler for irq12
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq12(void){
    printf("interrupt: irq12\n ");
    while(1);
}

/* void irq13()
 * Description: interrupt handler for irq13
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq13(void){
    printf("interrupt: irq13\n ");
    while(1);
}

/* void irq14()
 * Description: interrupt handler for irq14
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq14(void){
    printf("interrupt: irq14\n ");
    while(1);
}

/* void irq15()
 * Description: interrupt handler for irq15
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void irq15(void){
    printf("interrupt: irq15\n ");
    while(1);
}

/* void Divide_By_Zero()
 * Description: Exception handler for Divide_By_Zero
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Divide_By_Zero(void){
    printf("%s", "exception: Divide_by_zero");
    while(1);
    }

/* void Debug_Exception()
 * Description: Exception handler for Debug_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Debug_Exception(void){
    printf("%s", "exception: Debug_Exception");
    while(1);
    }

/* void NMI_Ipoint_Exception()
 * Description: Exception handler for NMI_Ipoint_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void NMI_Ipoint_Exception(void){
    printf("%s", "exception: NMI_Ipoint_Exception");
    while(1);
    }

/* void Overfnterrupt()
 * Description: Exception handler for Overfnterrupt
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Overfnterrupt(void){
    printf("%s", "exception: Overfnterrupt");
    while(1);
    }

/* void Breaklow_Exception()
 * Description: Exception handler for Breaklow_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Breaklow_Exception(void){
    printf("%s", "exception: Breaklow_Exception");
    while(1);
    }

/* void Bound_Range_Exceeded_Exception()
 * Description: Exception handler for Bound_Range_Exceeded_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Bound_Range_Exceeded_Exception(void){
    printf("%s", "exception: Breaklow_Exception");
    while(1);
    }

/* void Invalid_Opcode_Exception()
 * Description: Exception handler for Invalid_Opcode_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Invalid_Opcode_Exception(void){
    printf("%s", "exception: Invalid_Opcode_Exception");
    while(1);
    }

/* void Device_Not_Available_Exception()
 * Description: Exception handler for Device_Not_Available_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Device_Not_Available_Exception(void){
    printf("%s", "exception: Device_Not_Available_Exception");
    while(1);
    }

/* void Double_Fault_Exception()
 * Description: Exception handler for Double_Fault_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Double_Fault_Exception(void){
    printf("%s", "exception: Double_Fault_Exception");
    while(1);
    }

/* void Coprocessor_Segment_Overrun()
 * Description: Exception handler for Coprocessor_Segment_Overrun
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Coprocessor_Segment_Overrun(void){
    printf("%s", "exception: Coprocessor_Segment_Overrun");
    while(1);
    }

/* void Invalid_TSS_Exception()
 * Description: Exception handler for Invalid_TSS_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Invalid_TSS_Exception(void){
    printf("%s", "exception: Invalid_TSS_Exception");
    while(1);
    }

/* void Segment_Not_Present()
 * Description: Exception handler for Segment_Not_Present
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Segment_Not_Present(void){
    printf("%s", "exception: Segment_Not_Present");
    while(1);
    }

/* void Stack_Fault_Exception()
 * Description: Exception handler for Stack_Fault_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Stack_Fault_Exception(void){
    printf("%s", "exception: Stack_Fault_Exception");
    while(1);
    }

/* void General_Protection_Exception()
 * Description: Exception handler for General_Protection_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void General_Protection_Exception(void){
    printf("%s", "exception: General_Protection_Exception");
    while(1);
    }

/* void Page_Fault_Exception()
 * Description: Exception handler for Page_Fault_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Page_Fault_Exception(void){
    printf("%s", "exception: Page_Fault_Exception");
    while(1);
    }

/* void Floating_Point_Error()
 * Description: Exception handler for Floating_Point_Error
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Floating_Point_Error(void){
    printf("%s", "exception: Floating_Point_Error");
    while(1);
    }

/* void Alignment_Check_Exception()
 * Description: Exception handler for Alignment_Check_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Alignment_Check_Exception(void){
    printf("%s", "exception: Alignment_Check_Exception");
    while(1);
    }

/* void Machine_Check_Exception()
 * Description: Exception handler for Machine_Check_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void Machine_Check_Exception(void){
    printf("%s", "exception: Machine_Check_Exception");
    while(1);
    }

/* void SIMD_Floating_Point_Exception()
 * Description: Exception handler for SIMD_Floating_Point_Exception
 * Inputs:   NA   
 * Return Value: NA
 * Function: print the handler being executed */
void SIMD_Floating_Point_Exception(void){
    printf("%s", "exception: SIMD_Floating_Point_Exception");
    while(1);
    }

// void Gen_Handler() {
//     printf("%s", "Divide By Zero");
//     while(1);
// }



/* void populate_IDT()
 * Description: initialize IDT for exceptions and interrupts
 * Inputs:   NA   
 * Return Value: NA
 * Function: set idt entries for 19 exceptions and 16 interrupts */
void populate_IDT() {

    int i;
    // set for trap gate 
    for (i =0; i < 256; i++) {
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0; // ????? Maybe?
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        idt[i].dpl = 0;
        // idt[i].present = 1;
        if (i == SYSTEMCALL) {
            idt[i].dpl = 3;
            idt[i].reserved3 = 1; 
        }

    }
    //set up for interrupt gate 
    // for (i = 32; i <256; i++) {
    //     idt[i].seg_selector = KERNEL_CS;
    //     idt[i].reserved4 = 0;
    //     idt[i].reserved3 = 0;
    //     idt[i].reserved2 = 1;
    //     idt[i].reserved1 = 1;
    //     idt[i].size = 1;
    //     idt[i].reserved0 = 0;
    //     idt[i].dpl = 0;

    // }

    SET_IDT_ENTRY(idt[0],  Divide_By_Zero);
    SET_IDT_ENTRY(idt[1], Debug_Exception);
    SET_IDT_ENTRY(idt[2], NMI_Ipoint_Exception);
    SET_IDT_ENTRY(idt[4], Overfnterrupt);
    SET_IDT_ENTRY(idt[3], Breaklow_Exception);
    SET_IDT_ENTRY(idt[5], Bound_Range_Exceeded_Exception);
    SET_IDT_ENTRY(idt[6], Invalid_Opcode_Exception);
    SET_IDT_ENTRY(idt[7], Device_Not_Available_Exception);
    SET_IDT_ENTRY(idt[8], Double_Fault_Exception);
    SET_IDT_ENTRY(idt[9], Coprocessor_Segment_Overrun);
    SET_IDT_ENTRY(idt[10], Invalid_TSS_Exception);
    SET_IDT_ENTRY(idt[11], Segment_Not_Present);
    SET_IDT_ENTRY(idt[12], Stack_Fault_Exception);
    SET_IDT_ENTRY(idt[13], General_Protection_Exception);
    SET_IDT_ENTRY(idt[14], Page_Fault_Exception);
    SET_IDT_ENTRY(idt[16], Floating_Point_Error);
    SET_IDT_ENTRY(idt[17], Alignment_Check_Exception);
    SET_IDT_ENTRY(idt[18], Machine_Check_Exception);
    SET_IDT_ENTRY(idt[19], SIMD_Floating_Point_Exception);
// dealing with IRQ
    SET_IDT_ENTRY(idt[32], IRQ0);
    SET_IDT_ENTRY(idt[33], IRQ1);
    SET_IDT_ENTRY(idt[34], IRQ2);
    SET_IDT_ENTRY(idt[35], IRQ3);
    SET_IDT_ENTRY(idt[36], IRQ4);
    SET_IDT_ENTRY(idt[37], IRQ5);
    SET_IDT_ENTRY(idt[38], IRQ6);
    SET_IDT_ENTRY(idt[39], IRQ7);
    SET_IDT_ENTRY(idt[40], IRQ8);
    SET_IDT_ENTRY(idt[41], IRQ9);
    SET_IDT_ENTRY(idt[42], IRQ10);
    SET_IDT_ENTRY(idt[43], IRQ11);
    SET_IDT_ENTRY(idt[44], IRQ12);
    SET_IDT_ENTRY(idt[45], IRQ13);
    SET_IDT_ENTRY(idt[46], IRQ14);
    SET_IDT_ENTRY(idt[47], IRQ15);

    SET_IDT_ENTRY(idt[0x80], systemcall_handler);

    // lidt(ldt_desc_ptr);
}



