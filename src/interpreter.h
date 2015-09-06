#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "objects.h"

void interpret (ScopeStmt* stmt);

void exec_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s);
Obj* eval_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s);

Obj* eval_exp(EnvObj* genv, EnvObj* env, Exp* s);

#endif
