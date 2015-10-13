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
  if(argc == 3){
		gettimeofday(&total_start, NULL);
	}
  ScopeStmt* stmt = read_ast(filename);

  FILE* stat = NULL;
	if(argc == 3){
		stat = fopen(argvs[2], "w");
		gettimeofday(&start, NULL);
	}

  //Compile to bytecode
  Program* program = compile(stmt);
  double compile_time=0;
	if(argc == 3){
		gettimeofday(&end, NULL);
		compile_time=(end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0);
		fprintf(stat, "compile: %f\n", compile_time);
		gettimeofday(&start, NULL);
	}

  //Interpret bytecode
  interpret_bc(program);
	if(argc == 3){
		gettimeofday(&end, NULL);
		fprintf(stat, "interpret: %f\n", (end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0));
		double total_time = (end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (total_start.tv_sec * 1000.0 + total_start.tv_usec/1000.0);
		fprintf(stat, "percent compile time: %f%%\n", compile_time/total_time*100.0);
	}
	fclose(stat);

  return 0;
}
