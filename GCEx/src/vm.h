#ifndef VM_H
#define VM_H
#include "bytecode.h"

//is this necessary?
#define PRE_SUBMIT

#ifdef PRE_SUBMIT
extern double gc_time;
extern size_t halloc_bytes;
extern size_t halloc_int_bytes;
#endif

void interpret_bc (Program* prog);

#endif
