#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "objects.h"

// interpreter operations
void interpret (ScopeStmt* stmt);
Obj* interpret_slotstmt (EnvObj* genv, EnvObj* env, SlotStmt* s);

// expression evaluation operations
Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* exp);
Obj* eval_printf (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_if_exp (EnvObj* genv, EnvObj* env, Exp *e);
Obj* eval_obj_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_ref_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_int_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_set_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_slot_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_null_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_while_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_array_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_call_exp (EnvObj* genv, EnvObj* env, CallExp* e);
Obj* eval_call_slot_exp (EnvObj* genv, EnvObj* env, Exp *e);
Obj* eval_set_slot_exp(EnvObj* genv, EnvObj* env, SetSlotExp* e);

// statement evaluation operations
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
void exec_fn_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s);
void exec_var_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);

void print_help ();

#endif
