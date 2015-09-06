#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "ast.h"
#include "interpreter.h"

//----------------------------------------
// implementation of interpreter functions
//----------------------------------------

void interpret (ScopeStmt* s) {
  switch (s->tag) {
  case VAR_STMT: {
    // TBD
    ScopeVar* s2 = (ScopeVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
    // TBD
    ScopeFn* s2 = (ScopeFn*)s;
    printf("defn %s (", s2->name);
    for (int i = 0; i < s2->nargs; i++) {
      if (i > 0) printf(", ");
      printf("%s", s2->args[i]);
    }
    printf(") : (");
    interpret_scopestmt(s2->body);
    printf(")");
    break;
  }
  case SEQ_STMT: {
    // TBD
    ScopeSeq* s2 = (ScopeSeq*)s;
    interpret_scopestmt(s2->a);
    //printf(" ");
    interpret_scopestmt(s2->b);
    break;
  }
  case EXP_STMT: {
    ScopeExp* s2 = (ScopeExp*)s;
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  default:
    printf("Unrecognized scope statement with tag %d\n", s->tag);
    exit(-1);
  }
}

void interpret_slotstmt (SlotStmt* s) {
  switch (s->tag) {
  case VAR_STMT: {
    // TBD
    SlotVar* s2 = (SlotVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
    // TBD
    SlotMethod* s2 = (SlotMethod*)s;
    printf("method %s (", s2->name);
    for (int i = 0; i < s2->nargs; i++) {
      if (i > 0) printf(", ");
      printf("%s", s2->args[i]);
    }
    printf(") : (");
    interpret_scopestmt(s2->body);
    printf(")");
    break;
  }
  default:
    printf("Unrecognized slot statement with tag %d\n", s->tag);
    exit(-1);
  }
}

void interpret_scopestmt (ScopeStmt* s) {
  switch (s->tag) {
  case VAR_STMT: {
    // TBD
    ScopeVar* s2 = (ScopeVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
    // TBD
    ScopeFn* s2 = (ScopeFn*)s;
    printf("defn %s (", s2->name);
    for (int i = 0; i < s2->nargs; i++) {
      if (i > 0) printf(", ");
      printf("%s", s2->args[i]);
    }
    printf(") : (");
    interpret_scopestmt(s2->body);
    printf(")");
    break;
  }
  case SEQ_STMT: {
    // TBD
    ScopeSeq* s2 = (ScopeSeq*)s;
    interpret_scopestmt(s2->a);
    // printf(" ");
    interpret_scopestmt(s2->b);
    break;
  }
  case EXP_STMT: {
    ScopeExp* s2 = (ScopeExp*)s;
    eval_exp(NULL, NULL, s2->exp);
    break;
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
    ObjectExp* e2 = (ObjectExp*)e;
    EnvObj* nenv = make_env_obj(eval_exp(genv, env, e2->parent));
    for (int i = 0; i < e2->nslots; i++) {
      exec_stmt(genv, nenv, e2->slots[i]);
    }
    return (Obj*)env;
  }
  case SLOT_EXP: {
    // TBD
    SlotExp* e2 = (SlotExp*)e;
    eval_exp(genv, env, e2->exp);
    printf(".%s", e2->name);
    break;
  }
  case SET_SLOT_EXP: {
    // TBD
    SetSlotExp* e2 = (SetSlotExp*)e;
    eval_exp(genv, env, e2->exp);
    printf(".%s = ", e2->name);
    eval_exp(genv, env, e2->value);
    break;
  }
  case CALL_SLOT_EXP: {
    return eval_callslot_exp(genv, env, e);
    break;
  }
  case CALL_EXP: {
    // TBD
    CallExp* e2 = (CallExp*)e;
    printf("%s(", e2->name);
    for (int i = 0; i < e2->nargs; i++) {
      if (i > 0) printf(", ");
      print_exp(e2->args[i]);
    }
    printf(")");
    break;
  }
  case SET_EXP: {
    // TBD
    SetExp* e2 = (SetExp*)e;
    printf("%s = ", e2->name);
    eval_exp(genv, env, e2->exp);
    break;
  }
  case IF_EXP: {
    // TBD
    IfExp* e2 = (IfExp*)e;
    printf("if ");
    eval_exp(genv, env, e2->pred);
    printf(" : (");
    interpret_scopestmt(e2->conseq);
    printf(") else : (");
    interpret_scopestmt(e2->alt);
    printf(")");
    break;
  }
  case WHILE_EXP: {
    // TBD
    WhileExp* e2 = (WhileExp*)e;
    printf("while ");
    eval_exp(genv, env, e2->pred);
    printf(" : (");
    interpret_scopestmt(e2->body);
    printf(")");
    break;
  }
  case REF_EXP: {
    // TBD
    RefExp* e2 = (RefExp*)e;
    printf("%s", e2->name);
    break;
  }
  default:
    printf("Unrecognized Expression with tag %d\n", e->tag);
    exit(-1);
  }
}

Obj* eval_callslot_exp(EnvObj* genv, EnvObj* env, Exp *e) {
  Obj* first_arg_obj_ptr = NULL;
  Obj* second_arg_obj_ptr = NULL;

  CallSlotExp* e2 = (CallSlotExp*)e;
  Obj *receiver_ptr = eval_exp(genv, env, e2->exp);
  char *method_name_ptr = e2->name;

  // TODO: make the error message more informative
  // handle built in functions
  if (strcmp(method_name_ptr, "add") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)int_obj_add((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "sub") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)int_obj_sub((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "mul") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)int_obj_mul((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "div") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)int_obj_div((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "mod") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)int_obj_mod((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "lt") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)lt((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "gt") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)gt((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "le") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)le((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "ge") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)ge((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "eq") == 0) {
    if ((receiver_ptr->type == INT_OBJ) && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return (Obj*)eq((IntObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native function error\n");
      exit(-1);
    }
  }

  if (strcmp(method_name_ptr, "set") == 0) {
    if ((receiver_ptr->type == ARRAY_OBJ) && (e2->nargs == 2)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ
        && ((second_arg_obj_ptr = eval_exp(genv, env, e2->args[1])) != NULL)) {

      return (Obj*)array_set((ArrayObj*)receiver_ptr,
                             (IntObj*)first_arg_obj_ptr, second_arg_obj_ptr);
    } else {
      printf("native get function error\n");
      exit(-1);
    }
  } else if (strcmp(method_name_ptr, "get") == 0) {
    if (receiver_ptr->type == ARRAY_OBJ && (e2->nargs == 1)
        && ((first_arg_obj_ptr = eval_exp(genv, env, e2->args[0])) != NULL)
        && first_arg_obj_ptr->type == INT_OBJ) {
      return array_get((ArrayObj*)receiver_ptr, (IntObj*)first_arg_obj_ptr);
    } else {
      printf("native set function error\n");
      exit(-1);
    }
  }

  // TODO: handle method call
  /*
  for(int i=0; i<e2->nargs; i++) {}
  */
  return NULL;
}

Obj* eval_null_exp(EnvObj* genv, EnvObj* env, Exp *e) {
  return (Obj*)make_null_obj();
}

Obj* eval_int_exp(EnvObj* genv, EnvObj* env, Exp *e) {
  IntExp* e2 = (IntExp*)e;
  return (Obj*)make_int_obj(e2->value);
}

Obj* eval_array_exp(EnvObj* genv, EnvObj* env, Exp* e) {
  ArrayExp* e2 = (ArrayExp*)e;
  Obj* array_length_ptr = eval_exp(NULL, NULL, e2->length);
  if (obj_type(array_length_ptr) != INT_OBJ) {
    printf("call array() function: array length is not an integer.\n");
    exit(-1);
  }
  Obj* init_ptr = eval_exp(NULL, NULL, e2->init);
  return (Obj*) make_array_obj((IntObj*)array_length_ptr, init_ptr);
}

Obj* eval_printf(EnvObj* genv, EnvObj* env, Exp* e) {
  char replace_char[] = "~";
  PrintfExp* e2 = (PrintfExp*)e;
  // replace all ~ in formats into the corresponding arguments
  char *new_str = copy_string(e2->format);
  for (int i = 0; i < e2->nexps; i++) {
    char *old_str = new_str;
    Obj *argObj = eval_exp(NULL, NULL, e2->exps[i]);
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

void exec_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s){

}
Obj* eval_stmt(EnvObj* genv, EnvObj* env, ScopeStmt* s){

}

