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

	if(argc == 3){
		start_timer("interpret_time");
	}

  //Interpret bytecode
  interpret_bc(program);
	if(argc == 3){
		FILE* stat = fopen(argvs[2], "w");
		end_timer("interpret_time");
		fprintf(stat, "interpret: %f\n",
				get_double("interpret_time"));

		fprintf(stat, "gabage_collector time: %f\n",
				get_double("garbage_collector_time"));
		fprintf(stat, "halloc bytes: %zu\n",
				get_int("halloc_bytes"));
		fclose(stat);
	}
  return 0;
}
