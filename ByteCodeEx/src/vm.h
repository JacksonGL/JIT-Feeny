#ifndef VM_H
#define VM_H
#include "bytecode.h"

void interpret_bc (Program* prog);

#ifdef PRE_SUBMIT
// statistics data structure
typedef struct {
  struct timeval total_time;               // total time in ms
  long total_method_call;        // # of method calls
  long total_int_method_call;    // # of method calls with integer receiver
  long total_array_method_call;  // # of method calls with array receiver
  long total_envobj_method_call; // # of method calls with env obj receiver
  struct timeval total_time_lookup_entry;  // total time in ms spend looking
                                 // up an entry in env obj
} Stat;

// collect statistics operations
void init_stat ();
void print_help ();
int is_collect_stat ();
void write_stat (char* filename);
void set_collect_stat (int flag);
void inc_total_time (const struct timeval *total_time);
void inc_entry_lookup_time (const struct timeval* time);
void start_time_counter (struct timeval *t);
void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* diff);
#endif // PRE_SUBMIT
#endif // VM_H
