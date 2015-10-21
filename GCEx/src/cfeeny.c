#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/time.h>
#include "utils.h"
#include "ast.h"
#include "compiler.h"
#include "vm.h"
#define PRE_SUBMIT

int main (int argc, char** argvs) {
  struct timeval total_start,start, end;
  // Check number of arguments
  if (argc != 2 && argc != 3) {
    printf("Expected at least 1 argument to commandline.\n");
    exit(-1);
  }


  //Read in AST
  char* filename = argvs[1];
  ScopeStmt* stmt = read_ast(filename);


  //Compile to bytecode
  Program* program = compile(stmt);

  FILE* stat = NULL;
	if(argc == 3){
		stat = fopen(argvs[2], "w");
		gettimeofday(&start, NULL);
	}

  //Interpret bytecode
  interpret_bc(program);
	if(argc == 3){
		gettimeofday(&end, NULL);
		fprintf(stat, "interpret: %f\n", (end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0));

		fprintf(stat, "gabage_collector time: %f\n", gc_time);
		//fprintf(stat, "halloc bytes: %zu\n", halloc_bytes);
		//fprintf(stat, "halloc integer object bytes: %zu\n", halloc_int_bytes);
	}
	fclose(stat);

  return 0;
}
