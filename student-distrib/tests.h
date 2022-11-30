#ifndef TESTS_H
#define TESTS_H

// test launcher 
//3.1
void launch_tests();
int idt_test();
int div_zero_test();
int check_present_bit();
int dereference_fault_test_1();
int dereference_fault_test_2();
int dereference_fault_test_3();
int dereference_valid_test();  

//3.2
int terminal_test();
int read_frame0();
int read_frame1();
int read_pingpong();
int read_largefile();
int read_fail();
int rtc_change_frequency();
int ls_test();
#endif /* TESTS_H */







