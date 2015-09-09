#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"

//----------------------------------------
// implementation of interpreter functions
//----------------------------------------

void interpret (ScopeStmt* s) {
  EnvObj* genv = get_global_env_obj();
  Obj* result = eval_stmt(genv, genv, s);
}

Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s) {
  switch (s->tag) {
  case VAR_STMT: {
    exec_var_stmt(genv, env, s);
    return (Obj*)make_null_obj();
  }
  case FN_STMT: {
    exec_fn_stmt(genv, env, s);
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
    printf("Unrecognized scope statement with tag %d\n", s->tag);
    exit(-1);
  }
}

Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  switch (e->tag) {
  case INT_EXP: {
    return eval_int_exp(genv, env, e);
  }
  case NULL_EXP: {
    return eval_null_exp(genv, env, e);
  }
  case PRINTF_EXP: {
    return eval_printf(genv, env, e);
  }
  case ARRAY_EXP: {
    return eval_array_exp(genv, env, e);
  }
  case OBJECT_EXP: {
    return eval_obj_exp(genv, env, e);
  }
  case SLOT_EXP: {
    return eval_slot_exp(genv, env, e);
  }
  case SET_SLOT_EXP: {
    // TBD
    SetSlotExp* e2 = (SetSlotExp*)e;
    eval_set_slot_exp(genv, env, e2);
    break;
  }
  case CALL_SLOT_EXP: {
    return eval_call_slot_exp(genv, env, e);
  }
  case CALL_EXP: {
    return eval_call_exp(genv, env, (CallExp*)e);
  }
  case SET_EXP: {
    return eval_set_exp(genv, env, e);
  }
  case IF_EXP: {
    return eval_if_exp(genv, env, e);
  }
  case WHILE_EXP: {
    return eval_while_exp(genv, env, e);
  }
  case REF_EXP: {
    return eval_ref_exp(genv, env, e);
  }
  default:
    printf("Unrecognized Expression with tag %d\n", e->tag);
    exit(-1);
  }
  return NULL;
}

Obj* eval_set_slot_exp (EnvObj* genv, EnvObj* env, SetSlotExp* e) {
  Obj* obj_ptr = eval_exp(genv, env, e->exp);
  if (obj_type(obj_ptr) != ENV_OBJ) {
    printf("Cannot get slot %s from non-env object.", e->name);
    exit(-1);
  }
  Entry* entry = get_entry((EnvObj*)obj_ptr, e->name);
  if (entry == NULL || entry_type(entry) != VAR_ENTRY) {
    printf("Var slot %s does not exist in object.", e->name);
    exit(-1);
  }
  Obj* val = eval_exp(genv, env, e->value);
  set_value((VarEntry*)entry, val );
  return val;
}

Obj* eval_slot_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  SlotExp* e2 = (SlotExp*)e;
  Obj* obj_ptr = eval_exp(genv, env, e2->exp);
  if (obj_type(obj_ptr) != ENV_OBJ) {
    printf("Cannot get slot %s from non-env object.", e2->name);
    exit(-1);
  }
  Entry* entry = get_entry((EnvObj*)obj_ptr, e2->name);
  if (entry == NULL || entry_type(entry) != VAR_ENTRY) {
    printf("Var slot %s does not exist in object.", e2->name);
    exit(-1);
  }
  return get_value((VarEntry*)entry);
}

Obj* eval_set_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  SetExp* e2 = (SetExp*)e;
  VarEntry* var_entry = (VarEntry*)get_entry(env, e2->name);
  if (var_entry == NULL) {
    var_entry = (VarEntry*)get_entry(genv, e2->name);
    if (var_entry == NULL) {
      printf("Entry %s is not defined.", e2->name);
      exit(-1);
    }
  }

  Obj* new_val_ptr = eval_exp(genv, env, e2->exp);
  set_value(var_entry, new_val_ptr);
  return new_val_ptr;
}

Obj* eval_while_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  WhileExp* e2 = (WhileExp*)e;
  Obj* cond_ptr = NULL;
  Obj* result = (Obj*)make_null_obj();
  while (1) {
    cond_ptr = eval_exp(genv, env, e2->pred);
    if (cond_ptr == NULL) {
      printf("Invalid conditional value");
      exit(-1);
    }
    if (obj_type(cond_ptr) != NULL_OBJ) {
      // create branch environment/scope
      EnvObj* body_env = make_env_obj((Obj*)env);
      result = eval_stmt(genv, body_env, e2->body);
    } else {
      break;
    }
  }
  return result;
}

Obj* eval_if_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  IfExp* e2 = (IfExp*)e;
  Obj* result = NULL;
  // create branch environment/scope
  EnvObj* branch_env = make_env_obj((Obj*)env);

  Obj* cond_ptr = eval_exp(genv, env, e2->pred);
  if (cond_ptr == NULL) {
    printf("Invalid conditional value");
    exit(-1);
  }
  if (obj_type(cond_ptr) != NULL_OBJ) {
    result = eval_stmt(genv, branch_env, e2->conseq);
  } else {
    result = eval_stmt(genv, branch_env, e2->alt);
  }
  // TODO: free the branch environment object
  return result;
}

Obj* eval_call_exp (EnvObj* genv, EnvObj* env, CallExp*e) {
  Entry* t = get_entry(genv, e->name);
  assert(entry_type(t) == CODE_ENTRY);
  ScopeFn* f = get_scope_fn((CodeEntry*)t);
  // feeny is a statically scoped language
  EnvObj * fnEnv = make_env_obj((Obj*)genv);
  assert(fnEnv != env);
  for (int i = 0; i < e->nargs; ++i) {
    Obj* r = eval_exp(genv, env, e->args[i]);
    add_entry(fnEnv, f->args[i], (Entry*)make_var_entry(r));
    assert(get_entry(fnEnv, f->args[i]) != NULL);
  }
  Obj* result = eval_stmt(genv, fnEnv, f->body);
  //TODO: free fnEnv
  return result;
}

Obj* eval_call_slot_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  Obj* first_arg_obj_ptr = NULL;
  Obj* second_arg_obj_ptr = NULL;

  CallSlotExp* e2 = (CallSlotExp*)e;
  Obj *receiver_ptr = eval_exp(genv, env, e2->exp);
  char *method_name_ptr = e2->name;
  assert(obj_type(receiver_ptr) != NULL_OBJ);

  // TODO: make the error message more informative
  switch (obj_type(receiver_ptr)) {
  // handle built in functions
  case INT_OBJ: {
    Obj* arg;
    if (e2->nargs != 1 ||
        obj_type(arg = eval_exp(genv, env, e2->args[0])) != INT_OBJ) {
      printf("native int function error - %s\n",
             e2->nargs != 1 ? "not enough arguments!" :
             "wrong argument type!"
            );
      exit(-1);
    }
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
      printf("unknown native int function\n");
      exit(-1);
    }
  }
  case ARRAY_OBJ: {
    if (!strcmp(e2->name, "length") && e2->nargs == 0) {
      return (Obj*)array_length((ArrayObj*) receiver_ptr);
    }
    Obj* first_arg;
    if (e2->nargs < 1 || obj_type(first_arg = eval_exp(genv, env, e2->args[0])) != INT_OBJ) {
      printf("native array function error - %s\n",
             e2->nargs < 1 ? "not enough arguments!" :
             "incorrect argument type!");
      exit(-1);
    }
    if (!strcmp(e2->name, "set") && e2->nargs == 2) {
      return (Obj*)array_set((ArrayObj*)receiver_ptr,
                             (IntObj*)first_arg, eval_exp(genv, env, e2->args[1]));
    } else if (!strcmp(e2->name, "get") && e2->nargs == 1) {
      return (Obj*)array_get((ArrayObj*)receiver_ptr, (IntObj*)first_arg);
    } else {
      printf("unknown native array function\n");
      exit(-1);
    }
  }
  case ENV_OBJ: {
    Entry* code = get_entry((EnvObj*)receiver_ptr, e2->name);
    assert(code && entry_type(code) == CODE_ENTRY);
    ScopeFn* f = get_scope_fn((CodeEntry*)code);
    EnvObj * fnEnv = make_env_obj((Obj*)genv);
    assert(fnEnv != env);
    for (int i = 0; i < e2->nargs; ++i) {
      Obj* r = eval_exp(genv, env, e2->args[i]);
      add_entry(fnEnv, f->args[i], (Entry*)make_var_entry(r));
      assert(get_entry(fnEnv, f->args[i]) != NULL);
    }
    add_entry(fnEnv, "this", (Entry*) make_var_entry(receiver_ptr));
    Obj* result = eval_stmt(genv, fnEnv, f->body);
    //TODO: free fnEnv
    return result;
  }
  default:
    // TODO: complain about null obj
    assert(0);
  }
}

Obj* eval_null_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  return (Obj*)make_null_obj();
}

Obj* eval_int_exp (EnvObj* genv, EnvObj* env, Exp *e) {
  IntExp* e2 = (IntExp*)e;
  return (Obj*)make_int_obj(e2->value);
}

Obj* eval_array_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  ArrayExp* e2 = (ArrayExp*)e;
  Obj* array_length_ptr = eval_exp(genv, env, e2->length);
  if (obj_type(array_length_ptr) != INT_OBJ) {
    printf("call array() function: array length is not an integer.\n");
    exit(-1);
  }
  Obj* init_ptr = eval_exp(genv, env, e2->init);
  return (Obj*) make_array_obj((IntObj*)array_length_ptr, init_ptr);
}

Obj* eval_printf (EnvObj* genv, EnvObj* env, Exp* e) {
  char replace_char[] = "~";
  PrintfExp* e2 = (PrintfExp*)e;
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

void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s) {
  printf("Why us be called?\n");
}

void exec_fn_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s) {
  ScopeFn* s2 = (ScopeFn*)s;
  CodeEntry* code_entry = make_code_entry(s2);
  add_entry(env, s2->name, (Entry*)code_entry);
}

void exec_var_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s) {
  ScopeVar* s2 = (ScopeVar*)s;
  Obj* obj_ptr = eval_exp(genv, env, s2->exp);
  VarEntry* var_entry = make_var_entry(obj_ptr);
  add_entry(env, s2->name, (Entry*)var_entry);
}

Obj* eval_obj_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  ObjectExp* e2 = (ObjectExp*)e;
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

Obj* eval_ref_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  RefExp* e2 = (RefExp*)e;
  // try to locate the entry in the local scope
  Entry* entry_ptr = get_entry(env, e2->name);
  if (entry_ptr == NULL) {
    // try to locate the entry in the global scope
    entry_ptr = get_entry(genv, e2->name);
    if (entry_ptr == NULL) {
      printf("undefined reference: %s", e2->name);
      exit(-1);
    }
  }
  if (entry_type(entry_ptr) == VAR_ENTRY) {
    return get_value((VarEntry*)entry_ptr);
  } else {
    printf("cannot refer to code entry");
    exit(-1);
  }
}
