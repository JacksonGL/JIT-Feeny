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
    ScopeVar* s2 = (ScopeVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
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
    SlotVar* s2 = (SlotVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
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
    ScopeVar* s2 = (ScopeVar*)s;
    printf("var %s = ", s2->name);
    eval_exp(NULL, NULL, s2->exp);
    break;
  }
  case FN_STMT: {
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

//-----------------------------------------
// implementation of interpreter operations
//-----------------------------------------

IntObj* make_int_obj (int value) {
  IntObj* ptr_obj = malloc(sizeof(IntObj));
  ptr_obj->type = INT_OBJ;
  ptr_obj->value = value;
  return ptr_obj;
}

IntObj* copy_int_obj (IntObj *intObj) {
  IntObj* ptr_obj = malloc(sizeof(IntObj));
  ptr_obj->type = INT_OBJ;
  ptr_obj->value = intObj->value;
  return ptr_obj;
}

NullObj* make_null_obj () {
  NullObj* ptr_obj = malloc(sizeof(NullObj));
  ptr_obj->type = NULL_OBJ;
  return ptr_obj;
}

void create_array_elem (Obj* obj, ArrayElem *ptr_elem) {
  ptr_elem->type = obj->type;
  switch (obj->type) {
  case INT_OBJ: {
    ptr_elem->elem = (Obj*)copy_int_obj((IntObj*)obj);
    break;
  }
  case ENV_OBJ:
  case NULL_OBJ:
  case ARRAY_OBJ: {
    ptr_elem->elem = obj;
    break;
  }
  }
}

// all elements of an array in Feeny are integers
ArrayObj* make_array_obj (IntObj* length, Obj* init) {
  ArrayObj* ptr_array_obj = malloc(sizeof(ArrayObj));
  ptr_array_obj->type = ARRAY_OBJ;
  int len = ptr_array_obj->length = length->value;
  ptr_array_obj->array_elem_list = malloc(len * sizeof(ArrayElem));
  for (int i = 0; i < len; i++) {
    ArrayElem *elem_ptr = (ptr_array_obj->array_elem_list) + i;
    create_array_elem(init, elem_ptr);
  }
  return ptr_array_obj;
}

EnvObj* make_evn_obj (Obj* parent) {
  return NULL;
}

Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* e) {
  switch (e->tag) {
  case INT_EXP: {
    return eval_int_exp(e);
  }
  case NULL_EXP: {
    return eval_null_exp(e);
  }
  case PRINTF_EXP: {
    return eval_printf(e);
  }
  case ARRAY_EXP: {
    return eval_array_exp(e);
  }
  case OBJECT_EXP: {
    ObjectExp* e2 = (ObjectExp*)e;
    printf("object : (");
    for (int i = 0; i < e2->nslots; i++) {
      if (i > 0) printf(" ");
      interpret_slotstmt(e2->slots[i]);
    }
    printf(")");
    break;
  }
  case SLOT_EXP: {
    SlotExp* e2 = (SlotExp*)e;
    eval_exp(genv, env, e2->exp);
    printf(".%s", e2->name);
    break;
  }
  case SET_SLOT_EXP: {
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
    SetExp* e2 = (SetExp*)e;
    printf("%s = ", e2->name);
    eval_exp(genv, env, e2->exp);
    break;
  }
  case IF_EXP: {
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
    WhileExp* e2 = (WhileExp*)e;
    printf("while ");
    eval_exp(genv, env, e2->pred);
    printf(" : (");
    interpret_scopestmt(e2->body);
    printf(")");
    break;
  }
  case REF_EXP: {
    RefExp* e2 = (RefExp*)e;
    printf("%s", e2->name);
    break;
  }
  default:
    printf("Unrecognized Expression with tag %d\n", e->tag);
    exit(-1);
  }
}

int obj_type (Obj *o) {
  return o->type;
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
  for(int i=0; i<e2->nargs; i++){
    if(i > 0) printf(", ");

  }
  */
  return NULL;
}

Obj* eval_null_exp(Exp *e) {
  return (Obj*)make_null_obj();
}

Obj* eval_int_exp(Exp *e) {
  IntExp* e2 = (IntExp*)e;
  return (Obj*)make_int_obj(e2->value);
}

Obj* eval_array_exp(Exp* e) {
  ArrayExp* e2 = (ArrayExp*)e;
  Obj* array_length_ptr = eval_exp(NULL, NULL, e2->length);
  if (obj_type(array_length_ptr) != INT_OBJ) {
    printf("call array() function: array length is not an integer.\n");
    exit(-1);
  }
  Obj* init_ptr = eval_exp(NULL, NULL, e2->init);
  return (Obj*) make_array_obj((IntObj*)array_length_ptr, init_ptr);
}

Obj* eval_printf(Exp* e) {
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

// comparison operations for integer object
Obj* lt (IntObj* x, IntObj* y) {
  if (x->value < y->value) {
    return (Obj*)make_int_obj(0);
  } else {
    return (Obj*)make_null_obj();
  }
}

Obj* gt (IntObj* x, IntObj* y) {
  if (x->value > y->value) {
    return (Obj*)make_int_obj(0);
  } else {
    return (Obj*)make_null_obj();
  }
}

Obj* le (IntObj* x, IntObj* y) {
  if (x->value <= y->value) {
    return (Obj*)make_int_obj(0);
  } else {
    return (Obj*)make_null_obj();
  }
}

Obj* ge (IntObj* x, IntObj* y) {
  if (x->value >= y->value) {
    return (Obj*)make_int_obj(0);
  } else {
    return (Obj*)make_null_obj();
  }
}

Obj* eq (IntObj* x, IntObj* y) {
  if (x->value == y->value) {
    return (Obj*)make_int_obj(0);
  } else {
    return (Obj*)make_null_obj();
  }
}

NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v) {
  if (i->value >= a->length || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, i->value);
    exit(-1);
  }
  if (v->type != NULL_OBJ && v->type != (a->array_elem_list->type)) {
    printf("array set error: element type does not equal to array type");
    exit(-1);
  }

  if (v->type == INT_OBJ) {
    a->array_elem_list[i->value].elem = (Obj*)make_int_obj(((IntObj*)v)->value);
  } else {
    a->array_elem_list[i->value].elem = v;
  }
  // TODO: complete the array set function
  return make_null_obj();
}

Obj* array_get (ArrayObj* a, IntObj* i) {
  if (i->value >= a->length || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, i->value);
    exit(-1);
  }
  return (Obj*)a->array_elem_list[i->value].elem;
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

char* arrayToString(Obj *obj_ptr) {
  // TODO: convert [0, 1, 2] into [0 1 2]
  char *result = malloc(8 * sizeof(char));
  strcpy(result, "[ARRAY]");
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
    strcpy(result, "NULL");
    return result;
  }
  case ARRAY_OBJ: {
    return arrayToString(obj_ptr);
  }
  }
  return NULL;
}