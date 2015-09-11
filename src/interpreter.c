#include <sys/timeb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "ast.h"
#include "utils.h"
#include "interpreter.h"

// Copied from sys/time.h
// to not pollute our #define space
// WAS NOT WRITTEN BY LG OR BM - rights belong to original author
#define timersub(a, b, result)                  \
  do {                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;            \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;            \
    if ((result)->tv_usec < 0) {                \
      --(result)->tv_sec;                  \
      (result)->tv_usec += 1000000;                \
    }                        \
  } while (0)


#ifndef PRE_SUBMIT
// the following content only appear in submitted version

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



// expression evaluation operations
Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* exp);
Obj* eval_printf (EnvObj* genv, EnvObj* env, PrintfExp* e);
Obj* eval_if_exp (EnvObj* genv, EnvObj* env, IfExp *e);
Obj* eval_obj_exp (EnvObj* genv, EnvObj* env, ObjectExp* e);
Obj* eval_ref_exp (EnvObj* genv, EnvObj* env, RefExp* e);
Obj* eval_int_exp (EnvObj* genv, EnvObj* env, IntExp* e);
Obj* eval_set_exp (EnvObj* genv, EnvObj* env, SetExp* e);
Obj* eval_slot_exp (EnvObj* genv, EnvObj* env, SlotExp* e);
Obj* eval_while_exp (EnvObj* genv, EnvObj* env, WhileExp* e);
Obj* eval_array_exp (EnvObj* genv, EnvObj* env, ArrayExp* e);
Obj* eval_set_slot_exp(EnvObj* genv, EnvObj* env, SetSlotExp* e);
Obj* eval_call_exp (EnvObj* genv, EnvObj* env, CallExp* e);
Obj* eval_call_slot_exp (EnvObj* genv, EnvObj* env, CallSlotExp *e);

// statement evaluation operations
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
void exec_fn_stmt(EnvObj* genv, EnvObj* env, ScopeFn* s);
void exec_var_stmt (EnvObj* genv, EnvObj* env, ScopeVar* s);

void print_help ();


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
int is_collect_stat ();
void write_stat (char* filename);
void set_collect_stat (int flag);
void inc_total_time (const struct timeval *total_time);
void inc_entry_lookup_time (const struct timeval* time);
void inc_method_call (Obj* receiver_ptr);
void start_time_counter (struct timeval *t);
void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* diff);
#endif


//----------------------------------------
// implementation of interpreter functions
//----------------------------------------

void slot_assert(int i, SlotStmt* s, const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  if(!i){
    printf("Encountered error: ");
    vprintf(fmt, args);
    printf(" in slot statement ");
    print_slotstmt(s);
    printf("\n");
    exit(-1);
  }
  va_end(args);
}

void scope_assert(int i, ScopeStmt* s, const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  if(!i){
    printf("Encountered error: ");
    vprintf(fmt, args);
    printf(" in scope statement ");
    print_scopestmt(s);
    printf("\n");
    exit(-1);
  }
  va_end(args);
}

void exp_assert(int i, Exp* s, const char * fmt, ...){
  va_list args;
  va_start(args, fmt);
  if(!i){
    printf("Encountered error: ");
    vprintf(fmt, args);
    printf(" in expression ");
    print_exp(s);
    printf("\n");
    exit(-1);
  }
  va_end(args);
}

void interpret (ScopeStmt* s) {
  EnvObj* genv = get_global_env_obj();
  eval_stmt(genv, genv, s);
}

Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s) {
  switch (s->tag) {
  case VAR_STMT: {
    exec_var_stmt(genv, env, (ScopeVar*)s);
    return (Obj*)make_null_obj();
  }
  case FN_STMT: {
    exec_fn_stmt(genv, env, (ScopeFn*)s);
    return (Obj*)make_null_obj();
  }
  case SEQ_STMT: {
    ScopeSeq* s2 = (ScopeSeq*)s;
    eval_stmt(genv, env, s2->a);
    return eval_stmt(genv, env, s2->b);
  }
  case EXP_STMT: {
    ScopeExp* s2 = (ScopeExp*)s;
    return eval_exp(genv, env, s2->exp);
  }
  default:
    scope_assert(0, s, "Unrecognized scope statement with tag %d", s->tag);
    exit(-1);
  }
}

Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  switch (e->tag) {
  case INT_EXP: {
    return eval_int_exp(genv, env, (IntExp*)e);
  }
  case NULL_EXP: {
    return (Obj*) make_null_obj();
  }
  case PRINTF_EXP: {
    return eval_printf(genv, env, (PrintfExp*)e);
  }
  case ARRAY_EXP: {
    return eval_array_exp(genv, env, (ArrayExp*)e);
  }
  case OBJECT_EXP: {
    return eval_obj_exp(genv, env, (ObjectExp*)e);
  }
  case SLOT_EXP: {
    return eval_slot_exp(genv, env, (SlotExp*)e);
  }
  case SET_SLOT_EXP: {
    return eval_set_slot_exp(genv, env, (SetSlotExp*)e);
  }
  case CALL_SLOT_EXP: {
    return eval_call_slot_exp(genv, env, (CallSlotExp*)e);
  }
  case CALL_EXP: {
    return eval_call_exp(genv, env, (CallExp*)e);
  }
  case SET_EXP: {
    return eval_set_exp(genv, env, (SetExp*)e);
  }
  case IF_EXP: {
    return eval_if_exp(genv, env, (IfExp*)e);
  }
  case WHILE_EXP: {
    return eval_while_exp(genv, env, (WhileExp*)e);
  }
  case REF_EXP: {
    return eval_ref_exp(genv, env, (RefExp*)e);
  }
  default:
    exp_assert(0, e, "Unrecognized Expression with tag %d", e->tag);
  }
  return NULL;
}

Obj* eval_set_slot_exp (EnvObj* genv, EnvObj* env, SetSlotExp* e2) {
  Obj* obj_ptr = eval_exp(genv, env, e2->exp);
  exp_assert(obj_type(obj_ptr)==ENV_OBJ, (Exp*)e2,
    "Cannot get slot %s from non-env object", e2->name);

  Entry* entry = get_entry((EnvObj*)obj_ptr, e2->name);

  exp_assert(entry!=NULL && entry_type(entry) == VAR_ENTRY, (Exp*) e2,
    "Var slot %s does not exist in object", e2->name);

  Obj* val = eval_exp(genv, env, e2->value);
  set_value((VarEntry*)entry, val);
  return val;
}

Obj* eval_slot_exp (EnvObj* genv, EnvObj* env, SlotExp* e2) {
  Obj* obj_ptr = eval_exp(genv, env, e2->exp);

  exp_assert(obj_type(obj_ptr) == ENV_OBJ, (Exp*) e2, "Cannot get slot %s from non-env object", e2->name);

  Entry* entry = get_entry((EnvObj*)obj_ptr, e2->name);
  exp_assert(entry!= NULL && entry_type(entry)==VAR_ENTRY,
    (Exp*) e2, "Var slot %s does not exist in object", e2->name);
  return get_value((VarEntry*)entry);
}

Obj* eval_set_exp (EnvObj* genv, EnvObj* env, SetExp *e2) {
  VarEntry* var_entry = (VarEntry*)get_entry(env, e2->name);
  if (var_entry == NULL) {
    var_entry = (VarEntry*)get_entry(genv, e2->name);
    exp_assert(var_entry!= NULL, (Exp*) e2, "Entry %s is not defined", e2->name);
  }

  Obj* new_val_ptr = eval_exp(genv, env, e2->exp);
  set_value(var_entry, new_val_ptr);
  return new_val_ptr;
}

Obj* eval_while_exp (EnvObj* genv, EnvObj* env, WhileExp *e2) {
  Obj* cond_ptr = NULL;
  Obj* result = (Obj*)make_null_obj();
  while (1) {
    cond_ptr = eval_exp(genv, env, e2->pred);

    exp_assert(cond_ptr != NULL, (Exp*) e2, "Invalid conditional value");

    if (obj_type(cond_ptr) != NULL_OBJ) {
      // create branch environment/scope
      EnvObj* body_env = make_env_obj((Obj*)env);
      result = eval_stmt(genv, body_env, e2->body);
      free_env_obj(body_env);
    } else {
      break;
    }
  }
  return result;
}

Obj* eval_if_exp (EnvObj* genv, EnvObj* env, IfExp *e2) {
  Obj* result = NULL;
  // create branch environment/scope
  EnvObj* branch_env = make_env_obj((Obj*)env);

  Obj* cond_ptr = eval_exp(genv, env, e2->pred);
  exp_assert(cond_ptr != NULL, (Exp*) e2, "Invalid conditional value");

  if (obj_type(cond_ptr) != NULL_OBJ) {
    result = eval_stmt(genv, branch_env, e2->conseq);
  } else {
    result = eval_stmt(genv, branch_env, e2->alt);
  }
  // TODO: free the branch environment object
  free_env_obj(branch_env);
  return result;
}

Obj* eval_call_exp (EnvObj* genv, EnvObj* env, CallExp*e) {
  Entry* t = get_entry(genv, e->name);
  exp_assert(entry_type(t) == CODE_ENTRY, (Exp*) e, "Can only call methods/functions!");
  ScopeFn* f = get_scope_fn((CodeEntry*)t);
  // feeny is a statically scoped language
  EnvObj * fnEnv = make_env_obj((Obj*)genv);
  exp_assert(e->nargs == f->nargs, (Exp*) e,
    "Wrong number of arguments!");
  for (int i = 0; i < e->nargs; ++i) {
    Obj* r = eval_exp(genv, env, e->args[i]);
    add_entry(fnEnv, f->args[i], (Entry*)make_var_entry(r));
    exp_assert(get_entry(fnEnv, f->args[i]) != NULL, (Exp*) e,
      "Failed to evaluated %ith argument!", i);
  }
  Obj* result = eval_stmt(genv, fnEnv, f->body);
  //TODO: free fnEnv
  free_env_obj(fnEnv);
  return result;
}

Obj* eval_call_slot_exp (EnvObj* genv, EnvObj* env, CallSlotExp *e2) {
  Obj *receiver_ptr = eval_exp(genv, env, e2->exp);
  exp_assert(obj_type(receiver_ptr) != NULL_OBJ, (Exp*) e2,
    "Null object does not have methods!");
  inc_method_call(receiver_ptr);

  // TODO: make the error message more informative
  switch (obj_type(receiver_ptr)) {
  // handle built in functions
  case INT_OBJ: {
    Obj* arg;
    exp_assert(e2->nargs == 1 && obj_type(arg = eval_exp(genv, env, e2->args[0])) == INT_OBJ,
      (Exp*) e2, "native int function error - %s",
      e2->nargs != 1 ? "not enough arguments!" :
        "wrong argument type!");
    if (!strcmp(e2->name, "add")) {
      return (Obj*) int_obj_add((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "sub")) {
      return (Obj*) int_obj_sub((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "mul")) {
      return (Obj*) int_obj_mul((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "div")) {
      return (Obj*) int_obj_div((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "mod")) {
      return (Obj*) int_obj_mod((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "gt")) {
      return (Obj*) gt((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "ge")) {
      return (Obj*) ge((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "lt")) {
      return (Obj*) lt((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "le")) {
      return (Obj*) le((IntObj*) receiver_ptr, (IntObj*)arg);
    } else if (!strcmp(e2->name, "eq")) {
      return (Obj*) eq((IntObj*) receiver_ptr, (IntObj*)arg);
    } else {
      exp_assert(0, (Exp*) e2, "unknown native int function");
    }
  }
  case ARRAY_OBJ: {
    if (!strcmp(e2->name, "length") && e2->nargs == 0) {
      return (Obj*)array_length((ArrayObj*) receiver_ptr);
    }
    Obj* first_arg;
    exp_assert(e2->nargs > 0 && obj_type(first_arg = eval_exp(genv, env, e2->args[0])) == INT_OBJ,
      (Exp*) e2, "native array function error - %s",
      e2->nargs < 1 ? "not enough arguments!" :
        "incorrect argument type!");
    if (!strcmp(e2->name, "set") && e2->nargs == 2) {
      return (Obj*)array_set((ArrayObj*)receiver_ptr,
                             (IntObj*)first_arg, eval_exp(genv, env, e2->args[1]));
    } else if (!strcmp(e2->name, "get") && e2->nargs == 1) {
      return (Obj*)array_get((ArrayObj*)receiver_ptr, (IntObj*)first_arg);
    } else {
      exp_assert(0, (Exp*) e2, "unknown native array function");
    }
  }
  case ENV_OBJ: {
    Entry* code = get_entry((EnvObj*)receiver_ptr, e2->name);
    exp_assert(code && entry_type(code) == CODE_ENTRY, (Exp*)e2,
      "Failed to get code for method!");
    ScopeFn* f = get_scope_fn((CodeEntry*)code);
    EnvObj * fnEnv = make_env_obj((Obj*)genv);
    for (int i = 0; i < e2->nargs; ++i) {
      Obj* r = eval_exp(genv, env, e2->args[i]);
      add_entry(fnEnv, f->args[i], (Entry*)make_var_entry(r));
      exp_assert(get_entry(fnEnv, f->args[i]) != NULL, (Exp*) e2,
        "Failed to evaluated %ith argument!", i);
    }
    add_entry(fnEnv, "this", (Entry*) make_var_entry(receiver_ptr));
    Obj* result = eval_stmt(genv, fnEnv, f->body);
    //TODO: free fnEnv
    free_env_obj(fnEnv);
    return result;
  }
  default:
    // TODO: complain about null obj
    exp_assert(0, (Exp*) e2, "Cannot call method on null object!");
    exit(-1);
  }
}

Obj* eval_int_exp (EnvObj* genv, EnvObj* env, IntExp *e2) {
  return (Obj*)make_int_obj(e2->value);
}

Obj* eval_array_exp (EnvObj* genv, EnvObj* env, ArrayExp* e2) {
  Obj* array_length_ptr = eval_exp(genv, env, e2->length);
  exp_assert(obj_type(array_length_ptr) == INT_OBJ, (Exp*) e2,
    "call array() function: array length is not an integer");
  Obj* init_ptr = eval_exp(genv, env, e2->init);
  return (Obj*) make_array_obj((IntObj*)array_length_ptr, init_ptr);
}

Obj* eval_printf (EnvObj* genv, EnvObj* env, PrintfExp* e2) {
  char replace_char[] = "~";
  // replace all ~ in format string into the corresponding arguments
  char *new_str = copy_string(e2->format);
  for (int i = 0; i < e2->nexps; i++) {
    char *old_str = new_str;
    Obj *argObj = eval_exp(genv, env, e2->exps[i]);
    char *arg_str = toString(argObj);
    new_str = str_replace(old_str, replace_char, arg_str);
    if (arg_str != NULL) free(arg_str);
    if (old_str != NULL) free(old_str);
  }
  if (new_str != NULL) {
    printf("%s", new_str);
    free(new_str);
  }
  // TODO: what should be the return value of printf?
  return (Obj*)make_null_obj();
}

void exec_fn_stmt (EnvObj* genv, EnvObj* env, ScopeFn* s2) {
  CodeEntry* code_entry = make_code_entry(s2);
  add_entry(env, s2->name, (Entry*)code_entry);
}

void exec_var_stmt (EnvObj* genv, EnvObj* env, ScopeVar* s2) {
  Obj* obj_ptr = eval_exp(genv, env, s2->exp);
  VarEntry* var_entry = make_var_entry(obj_ptr);
  add_entry(env, s2->name, (Entry*)var_entry);
}

Obj* eval_obj_exp (EnvObj* genv, EnvObj* env, ObjectExp* e2) {
  EnvObj* nenv = make_env_obj(eval_exp(genv, env, e2->parent));
  for (int i = 0; i < e2->nslots; i++) {
    switch (e2->slots[i]->tag) {
    case VAR_STMT:
      eval_stmt(env, nenv, (ScopeStmt*)e2->slots[i]);
      break;
    default:
      eval_stmt(genv, nenv, (ScopeStmt*) e2->slots[i]);
    }
  }
  return (Obj*)nenv;
}

Obj* eval_ref_exp (EnvObj* genv, EnvObj* env, RefExp* e2) {
  // try to locate the entry in the local scope
  Entry* entry_ptr = get_entry(env, e2->name);
  if (entry_ptr == NULL) {
    // try to locate the entry in the global scope
    entry_ptr = get_entry(genv, e2->name);
    exp_assert(entry_ptr != NULL, (Exp*) e2,
      "undefined reference: %s", e2->name);
  }
  if (entry_type(entry_ptr) == VAR_ENTRY) {
    return get_value((VarEntry*)entry_ptr);
  } else {
    exp_assert(0, (Exp*) e2, "cannot refer to code entry");
    exit(-1);
  }
}

void print_help () {
  printf("Command line:\n\tcfeeny filename [-s path]\n\n\t-s print statistics to the specified file\n");
}

//----------------------------------------
// implementation of object functions
//----------------------------------------

int obj_type(Obj* o) {
  return o->type;
}

NullObj* make_null_obj() {
  static NullObj n = {NULL_OBJ};
  return &n;
}

IntObj* make_int_obj(int value) {
  //cache for memory usage - could make the cache larger
  static int cache_initted = 0;
  static IntObj cached[101];
  if(! cache_initted){
    for(int i = 0; i < sizeof(cached)/sizeof(IntObj); ++i){
      cached[i].type = INT_OBJ;
      cached[i].value = i;
    }
    cache_initted = 1;
  }
  if(value >= 0 && value < sizeof(cached)/sizeof(IntObj)){
    return &cached[value];
  }

  IntObj* t = malloc(sizeof(IntObj));
  t->type = INT_OBJ;
  t->value = value;
  return t;
}

IntObj* int_obj_add (IntObj* x, IntObj* y) {
  return make_int_obj(x->value + y->value);
}
IntObj* int_obj_sub (IntObj* x, IntObj* y) {
  return make_int_obj(x->value - y->value);
}
IntObj* int_obj_mul (IntObj* x, IntObj* y) {
  return make_int_obj(x->value * y->value);
}
IntObj* int_obj_div (IntObj* x, IntObj* y) {
  return make_int_obj(x->value / y->value);
}
IntObj* int_obj_mod (IntObj* x, IntObj* y) {
  return make_int_obj(x->value % y->value);
}

Obj* eq(IntObj* x, IntObj* y) {
  return x->value == y->value ?
         (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* lt(IntObj* x, IntObj* y) {
  return x->value < y->value ?
         (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* le(IntObj* x, IntObj* y) {
  return x->value <= y->value ?
         (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* gt(IntObj* x, IntObj* y) {
  return x->value > y->value ?
         (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* ge(IntObj* x, IntObj* y) {
  return x->value >= y->value ?
         (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}

ArrayObj* make_array_obj(IntObj* length, Obj* init) {
  ArrayObj* t = malloc(sizeof(ArrayObj));
  t->type = ARRAY_OBJ;
  t->v = make_vector();
  for (int i = 0; i < length->value; ++i) {
    vector_add(t->v, init);
  }
  return t;
}

IntObj* array_length(ArrayObj* a) {
  return make_int_obj(a->v->size);
}

NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v) {
  if (i->value >= a->v->size || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
    exit(-1);
  }

  vector_set(a->v, i->value, v);
  return make_null_obj();
}

Obj* array_get (ArrayObj* a, IntObj* i) {
  if (i->value >= a->v->size || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
    exit(-1);
  }
  return vector_get(a->v, i->value);
}

EnvObj* make_env_obj(Obj* parent) {
  EnvObj* t = malloc(sizeof(EnvObj));
  t->type = ENV_OBJ;
  t->parent = parent;
  t->names = make_vector();
  t->entries = make_vector();
  return t;
}

void free_env_obj(EnvObj* e){
  for(int i = 0; i < e->entries->size; ++i){
    Entry* ent = (Entry*) vector_get(e->entries, i);
    if(entry_type(ent) == VAR_ENTRY){
      free((VarEntry*) ent);
    } else {
      free((CodeEntry*) ent);
    }
  }
  vector_free(e->entries);
  vector_free(e->names);
  free(e);
}

void add_entry(EnvObj* env, char* name, Entry* entry) {
  vector_add(env->names, name);
  vector_add(env->entries, entry);
}

Entry* get_entry(EnvObj* env, const char* name) {
  struct timeval start, end;
  Entry* result = NULL;
  // start collecting time
  if (is_collect_stat())
    start_time_counter(&start);

  for (int i = 0; i < env->names->size; ++i) {
    if (!strcmp(name, vector_get(env->names, i))) {
      result = vector_get(env->entries, i);
      break;
    }
  }
  if (result == NULL && obj_type(env->parent) != NULL_OBJ) {
    result = get_entry((EnvObj*) env->parent, name);
  }

  // end collecting time
  if (is_collect_stat()) {
    struct timeval time;
    end_time_counter(&start, &end, &time);
    inc_entry_lookup_time(&time);
  }
  return result;
}

int entry_type(Entry* e) {
  return e->type;
}

VarEntry* make_var_entry(Obj* value) {
  VarEntry* t = malloc(sizeof(VarEntry));
  t->type = VAR_ENTRY;
  t->value = value;
  return t;
}

Obj* get_value(VarEntry* v) { // for consistency
  return v->value;
}
void set_value(VarEntry* v, Obj* t) { // for consistency
  v->value = t;
}

CodeEntry* make_code_entry(ScopeFn* fn) {
  CodeEntry* t = malloc(sizeof(CodeEntry));
  t->type = CODE_ENTRY;
  t->fn = fn;
  return t;
}

ScopeFn* get_scope_fn(CodeEntry* t) { // for consistency
  return t->fn;
}

EnvObj* get_global_env_obj () {
  static EnvObj* global_env = NULL;
  if (global_env == NULL) {
    global_env = make_env_obj((Obj*)make_null_obj());
  }
  return global_env;
}

void print_tabs(int t) {
  for (int i = 0; i < t; ++i) {
    debugf("\t");
  }
}

void _print_env(EnvObj* e, int tabs) {
  print_tabs(tabs);
  if ( e != get_global_env_obj()) {
    debugf("Env %p\n", e);
  } else {
    debugf("GlobalEnv %p\n", e);
  }
  for (int i = 0; i < e->names->size; ++i) {
    print_tabs(tabs);
    debugf("%s : %s\n",
           vector_get(e->names, i),
           entry_type((Entry*)vector_get(e->entries, i)) == VAR_ENTRY ? "var" : "code");
  }

  if (obj_type(e->parent) == NULL_OBJ) {
    print_tabs(tabs + 1);
    debugf("Null parent\n");
  } else {
    _print_env((EnvObj*)e->parent, tabs + 1);
  }
}

void print_env(EnvObj* e) {
  _print_env(e, 0);
}

void debugf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
}

//---------------------------------------
// functions that collect statistics
//---------------------------------------

static int collectStat = 0;
static Stat* stat = NULL;

void set_collect_stat (int flag) {
  collectStat = flag;
  if (flag) {
    init_stat();
  }
}

int is_collect_stat () {
  return collectStat != 0;
}

void init_stat () {
  stat = malloc(sizeof(Stat));
  stat->total_time.tv_usec = 0;
  stat->total_time.tv_sec = 0;
  stat->total_method_call = 0;
  stat->total_int_method_call = 0;
  stat->total_array_method_call = 0;
  stat->total_envobj_method_call = 0;
  stat->total_time_lookup_entry.tv_sec = 0;
  stat->total_time_lookup_entry.tv_usec = 0;
}

void start_time_counter (struct timeval *t) {
  gettimeofday(t, NULL);
}

void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* result) {
  gettimeofday(end, NULL);
  timersub(end, start, result);
}

void inc_total_time (const struct timeval *total_time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time.tv_sec += total_time->tv_sec;
  stat->total_time.tv_usec += total_time->tv_usec;
}

void inc_entry_lookup_time (const struct timeval* time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time_lookup_entry.tv_sec += time->tv_sec;
  stat->total_time_lookup_entry.tv_usec += time->tv_usec;
}

void inc_method_call (Obj* receiver_ptr) {
  if (receiver_ptr == NULL
          || collectStat == 0
          || stat == NULL)
    return;

  stat->total_method_call++;
  switch (receiver_ptr->type) {
  case INT_OBJ:
    stat->total_int_method_call++;
    break;
  case ARRAY_OBJ:
    stat->total_array_method_call++;
    break;
  case ENV_OBJ:
    stat->total_envobj_method_call++;
    break;
  default:
    break;
  }
}

void write_stat (char* filename) {
  if (!is_collect_stat() || stat == NULL) return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fprintf(f, "Total Time: %f ms\n", 0.0+stat->total_time.tv_sec*1000.0+stat->total_time.tv_usec/1000.0);
  fprintf(f, "Total Time Lookup Entry: %f ms\n", 0.0+stat->total_time_lookup_entry.tv_sec*1000.0+stat->total_time_lookup_entry.tv_usec/1000.0);
  fprintf(f, "Total Method Call: %ld\n", stat->total_method_call);
  fprintf(f, "Total Integer Method Call: %ld\n", stat->total_int_method_call);
  fprintf(f, "Total Array Method Call: %ld\n", stat->total_array_method_call);
  fprintf(f, "Total EnvObj Method Call: %ld\n", stat->total_envobj_method_call);

  fclose(f);
}

//---------------------------------------
// implementation of util functions
//---------------------------------------

// You must free the result if result is non-NULL.
// only replace the first occurrance of *rep
char *str_replace (char *orig, char *rep, char *with) {
  char *result; // the return string
  char *ins;    // the next insert point
  char *tmp;    // varies
  int len_rep;  // length of rep
  int len_with; // length of with
  int len_front; // distance between rep and end of last rep
  int count;    // number of replacements

  if (!orig)
    return NULL;
  if (!rep)
    rep = "";
  len_rep = strlen(rep);
  if (!with)
    with = "";
  len_with = strlen(with);

  ins = orig;
  for (count = 0; (tmp = strstr(ins, rep)); ++count) {
    ins = tmp + len_rep;
  }
  if (count > 0) {
    count = 1;
  }
  // first time through the loop, all the variable are set correctly
  // from here on,
  //    tmp points to the end of the result string
  //    ins points to the next occurrence of rep in orig
  //    orig points to the remainder of orig after "end of rep"
  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;

  while (count--) {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}

// You must free the result if result is non-NULL.
char* str_replace_all (char *orig, char *rep, char *with) {
  char *result; // the return string
  char *ins;    // the next insert point
  char *tmp;    // varies
  int len_rep;  // length of rep
  int len_with; // length of with
  int len_front; // distance between rep and end of last rep
  int count;    // number of replacements

  if (!orig)
    return NULL;
  if (!rep)
    rep = "";
  len_rep = strlen(rep);
  if (!with)
    with = "";
  len_with = strlen(with);

  ins = orig;
  for (count = 0; (tmp = strstr(ins, rep)); ++count) {
    ins = tmp + len_rep;
  }

  // first time through the loop, all the variable are set correctly
  // from here on,
  //    tmp points to the end of the result string
  //    ins points to the next occurrence of rep in orig
  //    orig points to the remainder of orig after "end of rep"
  tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

  if (!result)
    return NULL;

  while (count--) {
    ins = strstr(orig, rep);
    len_front = ins - orig;
    tmp = strncpy(tmp, orig, len_front) + len_front;
    tmp = strcpy(tmp, with) + len_with;
    orig += len_front + len_rep; // move to next "end of rep"
  }
  strcpy(tmp, orig);
  return result;
}

char* copy_string (const char *string) {
  char *stringcopy = malloc (1 + strlen (string));
  if (stringcopy) {
    strcpy (stringcopy, string);
  } else {
    fprintf (stderr, "malloc failure!");
  }
  return stringcopy;
}

char* intToString(int i) {
  char* b = malloc(20 * sizeof(char));
  char const digit[] = "0123456789";
  char* p = b;
  if (i < 0) {
    *p++ = '-';
    i *= -1;
  }
  int shifter = i;
  do { //Move to where representation ends
    ++p;
    shifter = shifter / 10;
  } while (shifter);
  *p = '\0';
  do { //Move back, inserting digits as u go
    *--p = digit[i % 10];
    i = i / 10;
  } while (i);
  return b;
}

char* arrayToString(ArrayObj *obj_ptr) {
  char** strs = malloc(sizeof(char*) * obj_ptr->v->size);
  int size_of_str = 1; //opening brace
  for (int i = 0; i < obj_ptr->v->size; ++i) {
    strs[i] = toString(vector_get(obj_ptr->v, i));
    size_of_str += strlen(strs[i]);
    if (i) {
      size_of_str += 1; //space
    }
  }
  size_of_str += 1; // closing brace
  size_of_str += 1; // null at end of string

  char *result = malloc(size_of_str * sizeof(char));
  *result = '\0';

  char* t = result;
  t = strcat(t, "[");
  for (int i = 0; i < obj_ptr->v->size; ++i) {
    t = strcat(t, strs[i]);
  }
  t = strcat(t, "]");
  return result;
}

// convert an interpreter object into a string
char* toString(Obj *obj_ptr) {
  switch (obj_ptr->type) {
    case INT_OBJ: {
      return intToString(((IntObj*)obj_ptr)->value);
    }
    case ENV_OBJ: {
      char *result = malloc(9 * sizeof(char));
      strcpy(result, "[OBJECT]");
      return result;
    }
    case NULL_OBJ: {
      char *result = malloc(5 * sizeof(char));
      strcpy(result, "Null");
      return result;
    }
    case ARRAY_OBJ: {
      return arrayToString((ArrayObj*)obj_ptr);
    }
  }
  return NULL;
}
