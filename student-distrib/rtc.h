#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define REG_A 0x8A
#define REG_B 0x8B
#define REG_C 0x0C
#define RTC_PORT 0x70
#define CMOS_PORT 0x71 

#define ORed_OR 0x40
#define RTC_IRQ 8

#define MAX_FRE 1024
#define MIN_FRE 2
#define MAX_RATE 15

#define RATE_MASK 0x0F 

// function used for itc
void rtc_initial();
void rtc_handler();
void set_up_frequency(int32_t frequency);
int32_t rtc_open(const int8_t *filename);
int32_t rtc_close (int32_t fd);
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes);

#endif


