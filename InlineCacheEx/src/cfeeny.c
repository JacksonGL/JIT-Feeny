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

		fprintf(stat, "JIT/Quicken Gen time: %f\n",
				get_double("quicken_time"));
		fprintf(stat, "Percent time in Quicken: %f\n",
				get_double("quicken_time")/get_double("interpret_time")*100);
		fclose(stat);
	}
  return 0;
}