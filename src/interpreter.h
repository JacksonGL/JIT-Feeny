#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "objects.h"

// interpreter operations
void interpret (ScopeStmt* stmt);
void interpret_slotstmt (SlotStmt* s);
void interpret_scopestmt (ScopeStmt* s);

// expression evaluation operations
Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* exp);
Obj* eval_printf (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_int_exp (EnvObj* genv, EnvObj* env, Exp *e);
Obj* eval_null_exp (EnvObj* genv, EnvObj* env, Exp *e);
Obj* eval_array_exp (EnvObj* genv, EnvObj* env, Exp* e);
Obj* eval_callslot_exp (EnvObj* genv, EnvObj* env, Exp *e);
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);

// statement evaluation operations
void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);

#endif