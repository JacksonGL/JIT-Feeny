#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"

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
