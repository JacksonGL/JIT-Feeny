#include <sys/time.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// comment out the following line
// to run in submitted version, which
// will not collect runtime statistics
#define PRE_SUBMIT

#include "utils.h"
#include "ast.h"
#include "interpreter.h"

int main (int argc, char** argvs) {
  struct timeval start, end;
  // Check number of arguments
  if (argc < 2) {
    printf("Expected at least 1 argument to commandline.\n");
#ifdef PRE_SUBMIT
    print_help();
#endif
    exit(-1);
  } else if (argc == 3 || argc > 4 || (argc == 4 && strcmp(argvs[2], "-s") != 0)) {
    printf("Expected at most 2 arguments to commandline.\n");
#ifdef PRE_SUBMIT
    print_help();
#endif
    exit(-1);
  }

  // Read in AST
  char* filename = argvs[1];
  ScopeStmt* stmt = read_ast(filename);

#ifdef PRE_SUBMIT
  // Read in option
  if (argc == 4) {
    set_collect_stat(1);
    start_time_counter(&start);
  }
#endif

  // Interpret
  // print_scopestmt(stmt);
  interpret(stmt);

#ifdef PRE_SUBMIT
  if (is_collect_stat()) {
    struct timeval time;
    end_time_counter(&start, &end, &time);
    inc_total_time(&time);
    write_stat(argvs[3]);
  }
#endif
}
