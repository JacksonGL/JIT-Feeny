#ifndef OBJECT_H
#define OBJECT_H

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

// array element can be an 
// integer, null or an array
typedef struct {
	ObjTag type;
	Obj *elem;
} ArrayElem;

typedef struct {
	ObjTag type;
	int length;
	int subtype;
	ArrayElem *array_elem_list;
} ArrayObj;

typedef struct {
	ObjTag type;
	Obj* parent;
	Vector* names;
	Vector* entries;
} EnvObj;

typedef enum {VAR_ENTRY, CODE_ENTRY} EntryTag;

typedef struct {
	EntryTag tag;
} Entry;

typedef struct {
	EntryTag tag;
	Obj* value;
} VarEntry;

typedef struct {
	EntryTag tag;
	ScopeFn * fn;
} CodeEntry;

int obj_type(Obj* o);

// integer object operations
NullObj* make_null_obj();
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
ArrayObj* make_array_obj(IntObj* length, Obj* init);
IntObj* array_length(ArrayObj* a);
NullObj* array_set(ArrayObj* a, IntObj* i, Obj* v);
Obj* array_get(ArrayObj* a, IntObj* i);

EnvObj* make_env_obj(Obj* parent);
void add_entry(EnvObj* env, char* name, Entry* entry);
Entry* get_entry(EnvObj* env, char* name);

int entry_type(Entry*);

VarEntry* make_var_entry(Obj* value);
Obj* get_value(VarEntry* v); // for consistency
void set_value(VarEntry* v, Obj*); // for consistency

CodeEntry* make_code_entry(ScopeFn* fn);
ScopeFn* get_scope_fn(CodeEntry* t); // for consistency


// util functions
char *copy_string (const char *string);
char *str_replace(char *orig, char *rep, char *with);
char *str_replace_all(char *orig, char *rep, char *with);
char* toString(Obj *obj_ptr);
char* intToString(int i);

#endif
