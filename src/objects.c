#include "objects.h"
#include <stdlib.h>
#include <string.h>

int obj_type(Obj* o){
	return o->type;
}

NullObj* make_null_obj(){
	static NullObj n= {NULLOBJ};
	return &n;
}

IntObj* make_int_obj(int value){
	static IntObj zero = {INTOBJ, 0}, one = {INTOBJ, 1};
	if(value == 0){ return &zero; }
	if(value == 1){ return &one; }

	IntObj* t = malloc(sizeof(IntObj));
	t->type = INTOBJ;
	t->value = value;
	return t;
}

IntObj* add(IntObj* x, IntObj* y){
	return make_int_obj(x->value + y->value);
}
IntObj* sub(IntObj* x, IntObj* y){
	return make_int_obj(x->value - y->value);
}
IntObj* mul(IntObj* x, IntObj* y){
	return make_int_obj(x->value * y->value);
}
IntObj* div_(IntObj* x, IntObj* y){
	return make_int_obj(x->value / y->value);
}

Obj* eq(IntObj* x, IntObj* y){
	return x->value==y->value ?
		(Obj*)make_int_obj(0): (Obj*)make_null_obj();
}
Obj* lt(IntObj* x, IntObj* y){
	return x->value<y->value ?
		(Obj*)make_int_obj(0): (Obj*)make_null_obj();
}
Obj* le(IntObj* x, IntObj* y){
	return x->value<=y->value ?
		(Obj*)make_int_obj(0): (Obj*)make_null_obj();
}
Obj* gt(IntObj* x, IntObj* y){
	return x->value>y->value ?
		(Obj*)make_int_obj(0): (Obj*)make_null_obj();
}
Obj* ge(IntObj* x, IntObj* y){
	return x->value>=y->value ?
		(Obj*)make_int_obj(0): (Obj*)make_null_obj();
}

ArrayObj* make_array_obj(IntObj* length, Obj* init){
	ArrayObj* t = malloc(sizeof(ArrayObj));
	t->type = ARRAYOBJ;
	t->subtype = obj_type(init);
	t->v = make_vector();
	for(int i = 0; i < length->value; ++i){
		vector_add(t->v, init);
	}
	return t;
}
IntObj* array_length(ArrayObj* a){
	return make_int_obj(a->v->size);
}
NullObj* array_set(ArrayObj* a, IntObj* i, Obj* v){
	vector_set(a->v, i->value, v);
	return make_null_obj();
}

Obj* array_get(ArrayObj* a, IntObj* i){
	return vector_get(a->v, i->value);
}

EnvObj* make_env_obj(Obj* parent){
	EnvObj* t = malloc(sizeof(EnvObj));
	t->type = ENVOBJ;
	t->parent = parent;
	t->names = make_vector();
	t->entries = make_vector();
	return t;
}

void add_entry(EnvObj* env, char* name, Entry* entry){
	vector_add(env->names, name);
	vector_add(env->entries, entry);
}

Entry* get_entry(EnvObj* env, char* name){
	for(int i = 0; i < env->names->size; ++i){
		if(!strcmp(name, vector_get(env->names, i))){
			return vector_get(env->entries, i);
		}
	}
	if(obj_type(env->parent) != NULLOBJ){
		return get_entry((EnvObj*) env, name);
	}
	return NULL;
}

int entry_type(Entry* e){
	return e->tag;
}

VarEntry* make_var_entry(Obj* value){
	VarEntry* t = malloc(sizeof(VarEntry));
	t->tag = VAR_ENTRY;
	t->value = value;
}
Obj* get_value(VarEntry* v){ // for consistency
	return v->value;
}
void set_value(VarEntry* v, Obj* t){ // for consistency
	v->value = t;
}

CodeEntry* make_code_entry(ScopeFn* fn){
	CodeEntry* t = malloc(sizeof(CodeEntry));
	t->tag = CODE_ENTRY;
	t->fn= fn;
	return t;
}
ScopeFn* get_scope_fn(CodeEntry* t){ // for consistency
	return t->fn;
}
