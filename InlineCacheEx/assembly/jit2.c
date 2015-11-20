#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>

int main (int argc, char** argvs) {
	char* code = mmap(0, 2014,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANON, -1, 0);

	code[0] = 0x48;
	code[1] = 0xc7;
	code[2] = 0xc0;
	code[3] = 0x3f;
	code[4] = 0x00;
	code[5] = 0x00;
	code[6] = 0x00;
	code[7] = 0xc3;

	int(*f)() = (void*)code;
	printf("Returned %d\n", f());
	return 0;
}

