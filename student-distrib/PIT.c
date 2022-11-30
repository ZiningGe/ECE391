#include "PIT.h"
#include "systemcall.h"
#include "x86_desc.h"
static int8_t  scheduler_index = -1;
 
/* PIT_init
 * Description: initialize PIT interrupt
 * Inputs:  None
 * Return Value:  None
 * Function: enable PIT and set frequency
 */
void PIT_init() {

    cli();
    // enable PIT
    outb(MODE3, PIT_COMMAND);
    // set PIT frequency
    outb(FREQUENCY & 0xFF, PIT_DATA);
    outb((FREQUENCY >> 8) & 0xFF, PIT_DATA);

    //enbale PIT on PIC interrupt line 0
    enable_irq(PIT_IRQ_NUM);

    sti();

    return;
}


/* PIT_interrupt_handler
 * Description: schedule to the next job
 * Inputs:  None
 * Return Value:  None
 * Function: send EOI to PIC and call the scheduler
 */
void PIT_interrupt_handler() {
    // send EOI
    send_eoi(PIT_IRQ_NUM);
    
    // call scheduler
    scheduler();
    

}


/* scheduler
 * Description: schedule to next job
 * Inputs:  None
 * Return Value:  None
 * Function: update PCB, esp, ebp and paging
 */
void scheduler() {
    uint32_t flags;
    uint32_t next_esp, next_ebp;
    cli_and_save(flags);
    //update scheduler index
    scheduler_index = (scheduler_index + 1) % TERMINAL_SIZE;
    // obtain 2 pcb
    PCB* current_PCB = curr_PCB[curr_rid];
    // if (current_PCB == NULL) return -1;
    PCB* next_PCB = curr_PCB[(uint8_t)scheduler_index];

    if (current_PCB == NULL || curr_rid == scheduler_index){
        restore_flags(flags);
        sti();
        return;
    }


    // save current esp/ebp to current_PCB
    asm volatile("movl %%esp, %0" : "=g"(current_PCB->esp2));
    asm volatile("movl %%ebp, %0" : "=g"(current_PCB->ebp2));
    //update terminal id 
    curr_rid = scheduler_index;
    set_current_video(scheduler_index);

    // NULL check
    if(next_PCB == NULL){
        set_pit_flag();
        restore_flags(flags);
        sti();
        system_execute((uint8_t*)"shell");

        return;
    }
    
    // update TSS
    tss.esp0 = KERNEL_MAX - (KERNEL_PROCESS_SIZE * next_PCB->pid) - 4;
    tss.ss0 = KERNEL_DS;
    // update process page
    set_process_page(next_PCB->pid);
    //update ESP and EBP for current new switched process
    next_esp = next_PCB->esp2;
    next_ebp = next_PCB->ebp2;
    asm volatile("movl %0, %%esp" : "=g"(next_esp));
    asm volatile("movl %0, %%ebp" : "=g"(next_ebp));
    restore_flags(flags);
    sti();
    return;
}

