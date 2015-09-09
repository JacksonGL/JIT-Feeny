#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"

int main (int argc, char** argvs) {
  struct timeb start, end;
  // Check number of arguments
  if (argc < 2) {
    printf("Expected at least 1 argument to commandline.\n");
    print_help();
    exit(-1);
  } else if (argc == 3 || argc > 4 || (argc == 4 && strcmp(argvs[2], "-s") != 0)) {
    printf("Expected at most 2 arguments to commandline.\n");
    print_help();
    exit(-1);
  }

  // Read in AST
  char* filename = argvs[1];
  ScopeStmt* stmt = read_ast(filename);

  // Read in option
  if (argc == 4) {
    set_collect_stat(1);
    start_time_counter(&start);
  }

  // Interpret
  // print_scopestmt(stmt);
  interpret(stmt);

  if (is_collect_stat()) {
    long time = end_time_counter(&start, &end);
    inc_total_time(time);
    write_stat(argvs[3]);
  }
}
