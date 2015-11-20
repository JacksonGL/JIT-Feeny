#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>

extern char return42[];
extern char return42_end[];
extern char inc_counter_ins[];
extern char inc_counter_ins_end[];

long counter  = 0;

int main (int argc, char** argvs) {
	char* code = mmap(0, 2014,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANON, -1, 0);
	/*
	code[0] = 0x48;
	code[1] = 0xc7;
	code[2] = 0xc0;
	code[3] = 0x2a;
	code[4] = 0x00;
	code[5] = 0x00;
	code[6] = 0x00;
	code[7] = 0xc3;
	*/

	memcpy(code, return42, return42_end - return42);

	int(*f)() = (void*)code;
	
	printf("Returned %d\n", f());
	
	memcpy(code, inc_counter_ins, inc_counter_ins_end - inc_counter_ins);
	
	f = (void*)code;
	f();
	printf("counter: %d\n", counter);
	return 0;
}

