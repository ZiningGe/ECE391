/* REFERENCE: https://wiki.osdev.org/PIC */

/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* void i8259_init()
 * Description: set up the PIC for inturrupt
 * Inputs:   NA   
 * Return Value: NA
 * Function: Initialize the PIC and unmask the irq to slave PIC */
void i8259_init(void) {

    /* Mask all port on both master and slave PIC */
    outb(0xFF, MASTER_DATA);
    outb(0xFF, SLAVE_DATA);

    /* Initialize master PIC */
    outb(ICW1, MASTER_COMMAND);
    outb(ICW2_MASTER, MASTER_DATA);
    outb(ICW3_MASTER, MASTER_DATA);
    outb(ICW4, MASTER_DATA);

     /* Initialize slave PIC */
    outb(ICW1, SLAVE_COMMAND);
    outb(ICW2_SLAVE, SLAVE_DATA);
    outb(ICW3_SLAVE, SLAVE_DATA);
    outb(ICW4, SLAVE_DATA);

    enable_irq(2);

}

/* void enable_irq()
 * Description: enable a specific irq
 * Inputs:   NA   
 * Return Value: NA
 * Function: unmask the irq line and enable the interrupt */
void enable_irq(uint32_t irq_num) {
    uint32_t flags;
    cli_and_save(flags);

    /* irq_num is on master PIC*/
    if (irq_num < MASTER_PIC_NUM){
        master_mask &= ~(1 << irq_num);
        outb(master_mask, MASTER_DATA);
    }
    /* irq_num is on slave PIC*/
    else{
        slave_mask &= ~(1 << (irq_num - MASTER_PIC_NUM));
        outb(slave_mask, SLAVE_DATA);
    }

    restore_flags(flags);
}

/* void disable_irq()
 * Description: disable a specific irq
 * Inputs:   NA   
 * Return Value: NA
 * Function: mask the irq line and disable the interrupt */
void disable_irq(uint32_t irq_num) {
    uint32_t flags;
    cli_and_save(flags);

    /* irq_num is on master PIC*/
    if (irq_num < MASTER_PIC_NUM){
        master_mask |= (1 << irq_num);
        outb(master_mask, MASTER_DATA);
    }
    /* irq_num is on slave PIC*/
    else{
        slave_mask |= (1 << (irq_num - MASTER_PIC_NUM));
        outb(slave_mask, SLAVE_DATA);
    }

    restore_flags(flags);
}

/* void send_eoi()
 * Description: send eoi to the PIC
 * Inputs:   NA   
 * Return Value: NA
 * Function: indicate the end of the service of the interrupt */
void send_eoi(uint32_t irq_num) {
    // send eoi to master PIC
    if (irq_num < MASTER_PIC_NUM){
        outb((irq_num | EOI), MASTER_COMMAND);
    }
    // send eoi to both mater and slave PIC
    else{
        outb(((irq_num - MASTER_PIC_NUM) | EOI), SLAVE_COMMAND);
        outb((2 | EOI), MASTER_COMMAND);
    }
}
