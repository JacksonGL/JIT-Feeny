#ifndef OBJECT_H
#define OBJECT_H

#include <sys/timeb.h>
#include <stdarg.h>
#include <stdio.h>
#include "ast.h"
#include "utils.h"

typedef enum { NULL_OBJ, INT_OBJ, ARRAY_OBJ, ENV_OBJ} ObjTag;

typedef struct {
  ObjTag type;
} Obj;

typedef struct {
  ObjTag type;
} NullObj;

typedef struct {
  ObjTag type;
  int value;
} IntObj;

typedef struct {
  ObjTag type;
  Vector* v;
} ArrayObj;

typedef struct {
  ObjTag type;
  Obj* parent;
  Vector* names;
  Vector* entries;
} EnvObj;

typedef enum {VAR_ENTRY, CODE_ENTRY} EntryTag;

typedef struct {
  EntryTag type;
} Entry;

typedef struct {
  EntryTag type;
  Obj* value;
} VarEntry;

typedef struct {
  EntryTag type;
  ScopeFn * fn;
} CodeEntry;

int obj_type(Obj* o);

// integer object operations
NullObj* make_null_obj();
IntObj* make_int_obj (int value);
IntObj* int_obj_add (IntObj* x, IntObj* y);
IntObj* int_obj_sub (IntObj* x, IntObj* y);
IntObj* int_obj_mul (IntObj* x, IntObj* y);
IntObj* int_obj_div (IntObj* x, IntObj* y);
IntObj* int_obj_mod (IntObj* x, IntObj* y);

// comparison operations for integer object
Obj* lt (IntObj* x, IntObj* y);
Obj* gt (IntObj* x, IntObj* y);
Obj* le (IntObj* x, IntObj* y);
Obj* ge (IntObj* x, IntObj* y);
Obj* eq (IntObj* x, IntObj* y);

// array object operations
IntObj* array_length(ArrayObj* a);
Obj* array_get(ArrayObj* a, IntObj* i);
NullObj* array_set(ArrayObj* a, IntObj* i, Obj* v);
ArrayObj* make_array_obj(IntObj* length, Obj* init);

// environment object operations
EnvObj* get_global_env_obj ();
EnvObj* make_env_obj(Obj* parent);
void free_env_obj(EnvObj* e); // Does not free parents

// entry operations
int entry_type(Entry*);
Entry* get_entry(EnvObj* env, const char* name);
void add_entry(EnvObj* env, char* name, Entry* entry);

// variable entry operations
Obj* get_value(VarEntry* v); // for consistency
void set_value(VarEntry* v, Obj*); // for consistency
VarEntry* make_var_entry(Obj* value);

// code entry operations
ScopeFn* get_scope_fn(CodeEntry* t); // for consistency
CodeEntry* make_code_entry(ScopeFn* fn);

// util functions
char* intToString(int i);
void print_env(EnvObj* e);
char* toString(Obj *obj_ptr);
char *copy_string (const char *string);
char *str_replace(char *orig, char *rep, char *with);
char *str_replace_all(char *orig, char *rep, char *with);

void debugf(const char *fmt, ...);

// statistics data structure
typedef struct {
  long total_time;               // total time in ms
  long total_method_call;        // # of method calls
  long total_int_method_call;    // # of method calls with integer receiver
  long total_array_method_call;  // # of method calls with array receiver
  long total_envobj_method_call; // # of method calls with env obj receiver
  long total_time_lookup_entry;  // total time in ms spend looking
                                 // up an entry in env obj
} Stat;

// collect statistics operations
void init_stat ();
int is_collect_stat ();
void write_stat (char* filename);
void set_collect_stat (int flag);
void inc_total_time (long total_time);
void inc_entry_lookup_time (long time);
void inc_method_call (Obj* receiver_ptr);
void start_time_counter (struct timeb *t);
long end_time_counter (struct timeb *start, struct timeb *end);

#endif
