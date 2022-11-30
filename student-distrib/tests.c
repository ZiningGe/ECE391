#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "page.h"
#include "filesystem.h"
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	terminal_printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	terminal_printf("\n [TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();

			result = FAIL;
		}

	}
	if ((idt[40].offset_15_00 == NULL) && (idt[40].offset_31_16 == NULL)) {
		terminal_printf( " rtc failure \n");
		assertion_failure();
		result = FAIL;
	}

	return result;
}


/* divide_zero_Test
 * 
 * Raise divide by zero exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT entries
 * Files: IDT.c/h
 */
int div_zero_test() {
  TEST_HEADER;
  int b __attribute__((unused)) = 0;
  b = 1 / b;
  return FAIL;
}

/* PDT Test
 * 
 * Asserts that first 2 IDT entries are present and sizes are 4kb and 4mb
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Page directory table, page table
 * Files: page.h/c
 */
int check_present_bit() {
	int i;
	for (i = 0; i < PAGE_SIZE; i++){
		if (i == 0){
			if (pde[i].p_d != 1){
				terminal_printf("First entry in Page directory table is not present");
				return FAIL;
			}
			if (pde[i].page_size_d != 0){
				terminal_printf("First entry in Page directory table is not 4kb");
				return FAIL;
			}
		}
		else if (i == 1){
			if (pde[i].p_d != 1){
				terminal_printf("Second entry in Page directory table is not present");
				return FAIL;
			}
			if (pde[i].page_size_d != 1){
				terminal_printf("Second entry in Page directory table is not 4mb");
				return FAIL;
			}
		}
		else{
			if (pde[i].p_d != 0){
				terminal_printf("Other entries in Page directory table is present");
				return FAIL;
			}
		}
	}
	return PASS;
}


/* page fault Test1
 * 
 * Raise a page fault when dereference address above video mem
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: PDT, PT, IDT
 * Files: page.c/h, IDT.c/h
 */
int dereference_fault_test_1() {
	TEST_HEADER;
	int j;
	terminal_printf("Dereference address above video mem");
	int* i = (int *)0x00;
	j = *i;
	return FAIL;
}


/* page fault Test2
 * 
 * Raise a page fault when dereference address in between video mem and kernel
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: PDT, PT, IDT
 * Files: page.c/h, IDT.c/h
 */
int dereference_fault_test_2() {
	TEST_HEADER;
	int j;
	terminal_printf("Dereference address in between video mem and kernel");
	int* i = (int *)0xC0000;
	j = *i;
	return FAIL;
}

/* page fault Test3
 * 
 * Raise a page fault when dereference address below kernel
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: PDT, PT, IDT
 * Files: page.c/h, IDT.c/h
 */
int dereference_fault_test_3() {
	TEST_HEADER;
	int j;
	terminal_printf("Dereference address below kernel");
	int* i = (int *)0x900000;
	j = *i;
	return FAIL;
}


/* dereference valid Test
 * 
 * Dereference address in the video mem and kernel
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: PDT, PT
 * Files: page.c/h
 */
int dereference_valid_test(){
	TEST_HEADER;
	int a, b, c, d;
	int * valid_address_1 = (int *)0xb8000;
	int * valid_address_2 = (int *)0xb9000 - 1;
	int * valid_address_3 = (int *)0x400000;
	int * valid_address_4 = (int *)0x800000 - 1;

	terminal_printf("Dereference video mem top\t");
	a = *valid_address_1;
	terminal_printf("Dereference video mem buttom\n");
	b = *valid_address_2;
	terminal_printf("Dereference kernel top\t");
	c = *valid_address_3;
	terminal_printf("Dereference kernel bottom\n");
	d = *valid_address_4;

	return PASS;
}

/* Checkpoint 2 tests */

//rtc test

/* rtc_change_frequency
 * 
 * test different rate of printing using rtc operations 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc
 * Files: rtc.c/h
 */
int rtc_change_frequency() {
	TEST_HEADER;
	int rtc_count = 0;
	int freq;
	rtc_open(NULL);
	terminal_printf("Open rtc at frequency 2Hz as default\n");
	for (rtc_count = 0; rtc_count < 5; rtc_count ++){
		rtc_read(NULL, NULL, NULL);
		terminal_printf("1");
	}

	terminal_printf("\n Write rtc at frequenct 8Hz\n");
	freq = 8;
	rtc_write(NULL, &freq, 4);
	for (rtc_count = 0; rtc_count < 20; rtc_count ++){
		rtc_read(NULL, NULL, NULL);
		terminal_printf("1");
	}
	
	terminal_printf("\n Write rtc at frequenct 16Hz\n");
	freq = 16;
	rtc_write(NULL, &freq, 4);
	for (rtc_count = 0; rtc_count < 20; rtc_count ++){
		rtc_read(NULL, NULL, NULL);
		terminal_printf("1");
	}

	terminal_printf("\n Write rtc at frequenct 64Hz\n");
	freq = 64;
	rtc_write(NULL, &freq, 4);
	for (rtc_count = 0; rtc_count < 100; rtc_count ++){
		rtc_read(NULL, NULL, NULL);
		terminal_printf("1");
	}

	terminal_printf("\n Write rtc at frequenct 128Hz\n");
	freq = 128;
	rtc_write(NULL, &freq, 4);
	for (rtc_count = 0; rtc_count < 100; rtc_count ++){
		rtc_read(NULL, NULL, NULL);
		terminal_printf("1");
	}

	terminal_printf("\n");
	rtc_close(NULL);

	return PASS;
}

// keyboard test/terminal test

/* terminal_test
 * 
 * test different rate of printing using rtc operations 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc
 * Files: rtc.c/h
 */
int terminal_test(){
	TEST_HEADER;
	// printf("start terminal test\n");
	int readByte = 8;
	int writeByte = 3;
	int a;
	int b;
	int i;
	int32_t fd;
	char *buf[128];
	// clean buffer for next test
	for(i = 0; i < 128; i++){
	buf[i] = '\0';
	}

	// test 1: w < r	
    terminal_printf("== start terminal r/w test 1: byte limit (r=8, w=3) ==\n");
    terminal_printf("== start terminal_read() ==\n");
	a = terminal_read(fd, buf, readByte);
    terminal_printf("== start terminal_write() ==\n");
	b = terminal_write(fd, buf, writeByte);
    terminal_printf("\n== finish terminal r/w test 1: byte limit ==\n");


	// test 2: overflow:
	readByte = MAX_SIZE_KEYBOARD_BUFFER;
	writeByte = MAX_SIZE_KEYBOARD_BUFFER;	
	// clean buffer for next test
	for(i = 0; i < 128; i++){
	buf[i] = '\0';
	}

	terminal_printf("== start terminal r/w test 2: overflow ==\n");
	terminal_printf("== start terminal_read() == \n");
	a = terminal_read(fd, buf, readByte);
	terminal_printf("the nbytes read when overflow is: %d \n", a);
	terminal_printf("== start terminal_write() == \n");
	b = terminal_write(fd, buf, writeByte);
	terminal_printf("\n== finish terminal r/w test 2: overflow == \n");


	readByte = MAX_SIZE_KEYBOARD_BUFFER;
	writeByte = MAX_SIZE_KEYBOARD_BUFFER;
	
	while(1){
		// clean buffer for next test
		for(i = 0; i < 128; i++){
		buf[i] = '\0';
		}
		terminal_printf("== start terminal r/w test 3: infinite war ==\n");
		terminal_printf("== start terminal_read() == \n");
		a = terminal_read(fd, buf, readByte);
		terminal_printf("== start terminal_write() == \n");
		b = terminal_write(fd, buf, writeByte);
		terminal_printf("== finish terminal r/w test 3: infinite war == \n");
	}	
	
	return PASS;
}

// filesystem file test

/* read_frame0
 * 
 * test read operation thorugh opening a file
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int read_frame0(){
	TEST_HEADER;
	// Print the ls file to the screen
	int32_t i;
	int32_t count;
	int8_t buf[187];
	fs_open((int8_t *)"frame0.txt");
	count = fs_read(0, buf, 187);
	for (i = 0; i < count; i++) {
		terminal_printf("%c", buf[i]);
	}
	fs_close(0);
	terminal_printf("\n");
	
	return PASS;
}

/* read_frame1
 * 
 * test read operation thorugh opening a file
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int read_frame1(){
	TEST_HEADER;
	// Print the ls file to the screen
	int32_t i;
	int32_t count;
	int8_t buf[174];
	fs_open((int8_t *)"frame1.txt");
	count = fs_read(0, buf, 174);
	for (i = 0; i < count; i++) {
		terminal_printf("%c", buf[i]);
	}
	fs_close(0);
	terminal_printf("\n");
	
	return PASS;
}

/* read_pingpong
 * 
 * test read operation thorugh opening a file
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int read_pingpong(){
	TEST_HEADER;
	// Print the ls file to the screen
	int32_t i;
	int32_t count;
	int8_t buf[5500];
	fs_open((int8_t *)"pingpong");
	count = fs_read(0, buf, 5500);
	for (i = 0; i < count; i++) {
		terminal_printf("%c", buf[i]);
	}
	fs_close(0);
	terminal_printf("\n");
	
	return PASS;
}

/* read_largefile
 * 
 * test edge case on whether open a file whose file name is larger than 32 Bs
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int read_largefile(){
	TEST_HEADER;
	// Print the ls file to the screen
	int32_t i,val;
	int32_t count;
	int8_t buf[5500];
	val = fs_open((int8_t *)"verylargetextwithverylongname.txt");
	if(val == -1) {
		terminal_printf("file name larger than 32, cannot print \n");
		return PASS;
	}
	count = fs_read(0, buf, 5500);
	for (i = 0; i < count; i++) {
		terminal_printf("%c", buf[i]);
	}
	fs_close(0);
	terminal_printf("\n");
	
	return FAIL;
}

/* read_fail
 * 
 * test edge cases on file system operations
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int read_fail(){
	if (fs_open((int8_t *)".") != -1){
		return FAIL;
	}

	if (fs_open((int8_t *)"rtc") != -1){
		return FAIL;
	}

	if (fs_open((int8_t *)"wyl_sb.txt") != -1){
		return FAIL;
	}

	return PASS;
}

//filesystem directory test 
//filesystem directory test 
/* ls_test
 * 
 * test printing of directory entry
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: filesystem
 * Files: filesystem.c/h
 */
int ls_test() {
	TEST_HEADER;
	char buf[FILENAME_SIZE]; //create a buffer to store name
	printf("\nstarting ls test\n"); 
	dir_open((int8_t *)"fsdir"); //open the directory
	while (dir_read(1, buf, FILENAME_SIZE) != -1) {} //while not equal to -1 (Failure), loop
	return PASS; 
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("page_value_test", check_present_bit());
	// TEST_OUTPUT("page_fault_test_1", dereference_fault_test_1());
	// TEST_OUTPUT("page_fault_test_2", dereference_fault_test_2());
	// TEST_OUTPUT("page_fault_test_3", dereference_fault_test_3());
	// TEST_OUTPUT("page_valid_test", dereference_valid_test());
	// TEST_OUTPUT("Divide By Zero", div_zero_test());
	// TEST_OUTPUT("filesystem_test:",read_frame0());
	// TEST_OUTPUT("filesystem_test:",read_pingpong());
	// TEST_OUTPUT("filesystem_ls_test:", ls_test());
	// TEST_OUTPUT("filesystem_test:",read_largefile());
	// TEST_OUTPUT("filesystem_test:",read_frame1());
	// TEST_OUTPUT("filesystem_test:",read_fail());
	// TEST_OUTPUT("termnal_test:",terminal_test());
	TEST_OUTPUT("rtc_test:", rtc_change_frequency());
	
}






