#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include "utils.h"
#include "bytecode.h"

void interpret_bc (Program* p) {
  print_prog(p);
}

int main (int argc, char** argvs) {
  //Check number of arguments
  if(argc != 2){
    printf("Expected 2 arguments to commandline.\n");
    exit(-1);
  }

  //Read in bytecode
  Program* p = load_bytecode(argvs[1]);
  interpret_bc(p);
  //  initvm(link_program(p));
  //  runvm();
  return 0;
}
