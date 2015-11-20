#ifndef VM_H
#define VM_H
#include <stdint.h>
#include "bytecode.h"

#define PRE_SUBMIT

//profiling functions
#define PROFILE
#ifdef PROFILE
#define start_timer(a) _start_timer(a)
#define end_timer(a) _end_timer(a)

#define add_double(a, b) _add_double(a,b)
#define get_double(a) _get_double(a)

#define add_int(a, b) _add_int(a,b)
#define get_int(a) _get_int(a)

#else

#define start_timer(a) ((void)0)
#define end_timer(a) ((void)0)

#define add_double(a, b) ((void)0)
#define get_double(a) ((void)0)

#define add_int(a, b) ((void)0)
#define get_int(a) ((void)0)

#endif

void _start_timer(const char * name);
void _end_timer(const char * name);

void _add_double(const char * name, double val);
double _get_double(const char * name);

void _add_int(const char * name, intptr_t val);
intptr_t _get_int(const char * name);

void interpret_bc (Program* prog);

#endif
