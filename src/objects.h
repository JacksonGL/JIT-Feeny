#ifndef OBJECT_H
#define OBJECT_H

#include "ast.h"
#include "utils.h"
typedef enum { OBJ, NULLOBJ, INTOBJ, ARRAYOBJ, ENVOBJ} ObjTag;

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
	int subtype;
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

NullObj* make_null_obj();

IntObj* make_int_obj(int value);
IntObj* add(IntObj* x, IntObj* y);
IntObj* sub(IntObj* x, IntObj* y);
IntObj* mul(IntObj* x, IntObj* y);
IntObj* div_(IntObj* x, IntObj* y);

Obj* eq(IntObj* x, IntObj* y);
Obj* lt(IntObj* x, IntObj* y);
Obj* le(IntObj* x, IntObj* y);
Obj* gt(IntObj* x, IntObj* y);
Obj* ge(IntObj* x, IntObj* y);

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

#endif
