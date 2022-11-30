#include "rtc.h"
#include "lib.h"
#include "i8259.h"

static volatile uint32_t rtc_int_counter;
/* void rtc_initial()
 * Description: set up the rtc for inturrupt
 * Inputs:   NA   
 * Return Value: NA
 * Function: Initialize the rtc */
void rtc_initial() {
// write B first
    //disable interrupts first
    rtc_int_counter = 0;
    cli();
    // select reg b and disable NMI
    outb(REG_B,RTC_PORT);
    //read the cur value of reg b
    unsigned char prev = inb(CMOS_PORT);
    //set the index again 
    outb(REG_B,RTC_PORT);
    // write the previous value ORed with 0x40
    outb(prev | ORed_OR, CMOS_PORT);

    sti();
//Turning on IRQ8
    enable_irq(RTC_IRQ);
//set up the default frequency
    set_up_frequency(MAX_FRE);
    return;
}

/* void rtc_handler()
 * Description: set up the rtc handler for inturrupt
 * Inputs:   NA   
 * Return Value: NA
 * Function: process the rtc handler and send the EOI to PIC*/
void rtc_handler() {
    // send interrupt ack to PIC
    rtc_int_counter++;
    outb(REG_C,RTC_PORT);
    inb(CMOS_PORT);
    // test_interrupts();
    // printf("1");
    send_eoi(RTC_IRQ);
}

/* set_up_frequency
 * Description: set up the rtc frequency for inturrupt
 * Inputs:   NA   
 * Return Value: NA
 * Function: process the rtc handler and send the EOI to PIC*/
void set_up_frequency(int32_t frequency) {
    //check if freq is in the range
    if (frequency < MIN_FRE || frequency > MAX_FRE) return;
    // Check if frequency is a power of 2
    if (frequency & (frequency - 1)) return;

    unsigned char rate = MAX_RATE + 1 - log2(frequency) ;
    // set rate not obave 15 
    rate &= RATE_MASK;
    // determine whether it is faster than 3
    if (rate < 3)  return;

    // start critical section
    cli();
    // select reg a and disable NMI
    outb(REG_A, RTC_PORT);
    //read the cur value of reg a
    unsigned char prev = inb(CMOS_PORT);
    //set the index again 
    outb(REG_A,RTC_PORT);
    // write only our rate to A
    outb((prev & 0xF0) | rate, CMOS_PORT);
    // end the critical point
    sti();
}
/* rtc_open
 * Description:  provides access to the file system rtc through system call 
 * Inputs:   filename   
 * Return Value:0 if success
 * Function: process the rtc handler and send the EOI to PIC*/
int32_t rtc_open(const int8_t *filename){
    // Question 2 or 1024 here ???
    set_up_frequency(MIN_FRE);
    return 0;
}

/* rtc_clsoe
 * Description:  closes the specified file descriptor and makes it available for return from later calls to open.
 * Inputs:   fd  
 * Return Value: 0 if success
 * Function: process the rtc handler and send the EOI to PIC*/
int32_t rtc_close (int32_t fd){
    return 0;
}

/* rtc_read
 * Description:  provides access to the file system rtc through system call 
 * Inputs:   fd, buf, nbytes   
 * Return Value:0 if success
 * Function: process the rtc handler and send the EOI to PIC*/
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    rtc_int_counter = 0;
    // wait for the interrupt to come
    while (!rtc_int_counter) {}
    rtc_int_counter = 0;
    return 0;
}

/* rtc_write
 * Description:  provides access to the file system rtc through system call 
 * Inputs:   fd, buf, nbytes   
 * Return Value:0 if success
 * Function: process the rtc handler and send the EOI to PIC*/
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes){
    // check failing conditions
    if (nbytes != sizeof(uint32_t) || buf == NULL) return -1;
    int freq = *((int *) buf);
    set_up_frequency(freq);
    return 0;
}

