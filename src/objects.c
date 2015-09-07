#include "objects.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int obj_type(Obj* o) {
	return o->type;
}

NullObj* make_null_obj() {
	static NullObj n = {NULL_OBJ};
	return &n;
}

IntObj* make_int_obj(int value) {
	//cache for memory usage - could make the cache larger
	static IntObj zero = {INT_OBJ, 0}, one = {INT_OBJ, 1};
	if (value == 0) { return &zero; }
	if (value == 1) { return &one; }

	IntObj* t = malloc(sizeof(IntObj));
	t->type = INT_OBJ;
	t->value = value;
	return t;
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

Obj* eq(IntObj* x, IntObj* y) {
	return x->value == y->value ?
	       (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* lt(IntObj* x, IntObj* y) {
	return x->value < y->value ?
	       (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* le(IntObj* x, IntObj* y) {
	return x->value <= y->value ?
	       (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* gt(IntObj* x, IntObj* y) {
	return x->value > y->value ?
	       (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}
Obj* ge(IntObj* x, IntObj* y) {
	return x->value >= y->value ?
	       (Obj*)make_int_obj(0) : (Obj*)make_null_obj();
}

ArrayObj* make_array_obj(IntObj* length, Obj* init) {
	ArrayObj* t = malloc(sizeof(ArrayObj));
	t->type = ARRAY_OBJ;
	t->v = make_vector();
	for (int i = 0; i < length->value; ++i) {
		vector_add(t->v, init);
	}
	return t;
}

IntObj* array_length(ArrayObj* a) {
	return make_int_obj(a->v->size);
}

NullObj* array_set (ArrayObj* a, IntObj* i, Obj* v) {
	if (i->value >= a->v->size || i->value < 0) {
		printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
		exit(-1);
	}

	vector_set(a->v, i->value, v);
	return make_null_obj();
}

Obj* array_get (ArrayObj* a, IntObj* i) {
	if (i->value >= a->v->size || i->value < 0) {
		printf("array index out of bound. array length: %d. index: %d", a->v->size, i->value);
		exit(-1);
	}
	return vector_get(a->v, i->value);
}

EnvObj* make_env_obj(Obj* parent) {
	EnvObj* t = malloc(sizeof(EnvObj));
	t->type = ENV_OBJ;
	t->parent = parent;
	t->names = make_vector();
	t->entries = make_vector();
	return t;
}

void add_entry(EnvObj* env, char* name, Entry* entry) {
	vector_add(env->names, name);
	vector_add(env->entries, entry);
}

Entry* get_entry(EnvObj* env, char* name) {
	for (int i = 0; i < env->names->size; ++i) {
		if (!strcmp(name, vector_get(env->names, i))) {
			return vector_get(env->entries, i);
		}
	}
	if (obj_type(env->parent) != NULL_OBJ) {
		return get_entry((EnvObj*) env->parent, name);
	}
	return NULL;
}

int entry_type(Entry* e) {
	return e->type;
}

VarEntry* make_var_entry(Obj* value) {
	VarEntry* t = malloc(sizeof(VarEntry));
	t->type = VAR_ENTRY;
	t->value = value;
	return t;
}

Obj* get_value(VarEntry* v) { // for consistency
	return v->value;
}
void set_value(VarEntry* v, Obj* t) { // for consistency
	v->value = t;
}

CodeEntry* make_code_entry(ScopeFn* fn) {
	CodeEntry* t = malloc(sizeof(CodeEntry));
	t->type = CODE_ENTRY;
	t->fn = fn;
	return t;
}

ScopeFn* get_scope_fn(CodeEntry* t) { // for consistency
	return t->fn;
}

EnvObj* get_global_env_obj () {
	static EnvObj* global_env = NULL;
	if (global_env == NULL) {
		global_env = make_env_obj((Obj*)make_null_obj());
	}
	return global_env;
}

void print_tabs(int t){
	for(int i = 0; i < t; ++i){
		debugf("\t");
	}
}

void _print_env(EnvObj* e, int tabs){
	print_tabs(tabs);
	if( e != get_global_env_obj()){
		debugf("Env %p\n", e);
	} else {
		debugf("GlobalEnv %p\n", e);
	}
	for(int i = 0; i < e->names->size; ++i){
		print_tabs(tabs);
		debugf("%s : %s\n",
				vector_get(e->names, i),
				entry_type((Entry*)vector_get(e->entries, i)) == VAR_ENTRY ? "var" : "code");
	}

	if(obj_type(e->parent) == NULL_OBJ){
		print_tabs(tabs+1);
		debugf("Null parent\n");
	} else {
		_print_env((EnvObj*)e->parent, tabs+1);
	}
}

void print_env(EnvObj* e){
	_print_env(e, 0);
}

void debugf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
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

char* arrayToString(ArrayObj *obj_ptr) {
	char** strs = malloc(sizeof(char*) * obj_ptr->v->size);
	int size_of_str = 1; //opening brace
	for(int i = 0; i< obj_ptr->v->size; ++i){
		strs[i] = toString(vector_get(obj_ptr->v, i));
		size_of_str += strlen(strs[i]);
		if(i){
			size_of_str += 1; //space
		}
	}
	size_of_str += 1; // closing brace
	size_of_str += 1; // null at end of string

	char *result = malloc(size_of_str * sizeof(char));

	char* t = result;
	t = strcat(t, "[");
	for(int i = 0; i < obj_ptr->v->size; ++i){
		t = strcat(t, strs[i]);
	}
	t = strcat(t, "]");
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
		strcpy(result, "Null");
		return result;
	}
	case ARRAY_OBJ: {
		return arrayToString((ArrayObj*)obj_ptr);
	}
	}
	return NULL;
}
