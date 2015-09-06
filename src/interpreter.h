#ifndef INTERPRETER_H
#define INTERPRETER_H

// basic object data structures
typedef enum {
	INT_OBJ,
	NULL_OBJ,
	ARRAY_OBJ,
	ENV_OBJ
} ObjType;

typedef struct {
	ObjType type;
} Obj;

typedef struct {
	ObjType type;
	int value;
} IntObj;

typedef struct {
	ObjType type;
} NullObj;

typedef struct {
	ObjType type;
	// all other fields
} EnvObj;

typedef struct {
	ObjType type;
	Obj *elem;
} ArrayElem;

typedef struct {
	ObjType type;
	int length;
	ArrayElem *array_elem_list;
} ArrayObj;


typedef enum {
	VAR_ENTRY,
	CODE_ENTRY
} EntryType;

typedef struct {
	EntryType type;
	Obj* ptr_obj;
	// TODO: add code entry here
} Entry;

// interpreter operations
void interpret (ScopeStmt* stmt);
void interpret_slotstmt (SlotStmt* s);
void interpret_scopestmt (ScopeStmt* s);

// basic object operations
int obj_type (Obj *o);
NullObj* make_null_obj ();

// integer object operations
IntObj* make_int_obj (int value);
IntObj* copy_int_obj (IntObj *intObj);
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
void create_array_elem (Obj* obj, ArrayElem *ptr_elem);
ArrayObj* make_array_obj (IntObj* length, Obj* init);
NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v);
Obj* array_get (ArrayObj* a, IntObj* i);
IntObj* array_length (ArrayObj* a);

// environment object operations
EnvObj* make_evn_obj (Obj* parent);
Entry* get_entry (EnvObj* env, char* name);
void add_entry (EnvObj* env, char* name, Entry* entry);

// expression evaluation operations
Obj* eval_exp (EnvObj* genv, EnvObj* env, Exp* exp);
Obj* eval_printf(Exp* e);
Obj* eval_int_exp(Exp *e);
Obj* eval_null_exp(Exp *e);
Obj* eval_array_exp(Exp* e);
Obj* eval_callslot_exp(EnvObj* genv, EnvObj* env, Exp *e);

// statement evaluation operations
void exec_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);
Obj* eval_stmt (EnvObj* genv, EnvObj* env, ScopeStmt* s);

// util functions
char *copy_string (const char *string);
char *str_replace(char *orig, char *rep, char *with);
char *str_replace_all(char *orig, char *rep, char *with);
char* toString(Obj *obj_ptr);
char* intToString(int i);

#endif