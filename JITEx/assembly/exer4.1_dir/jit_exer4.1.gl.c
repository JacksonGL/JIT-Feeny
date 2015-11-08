#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>

int main (int argc, char** argvs) {
	char* code = mmap(0, 2014,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_PRIVATE | MAP_ANON, -1, 0);
	
	printf("Returned %d\n", f(12));
	return 0;
}

