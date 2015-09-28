#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "ast.h"
#include "compiler.h"
#include "bytecode.h"

//============================================================
//=================== HEADER FUNCTIONS =======================
//============================================================

typedef struct {
	Vector* vars;
} _Scope;

typedef struct {
	Vector* vars;
	_Scope* parent;
} Scope;

Program* compile (ScopeStmt* stmt);
int compile_exp (Exp* e, Program* program, Vector* body, Scope* sp);
int compile_int_exp(IntExp* e, Program* p, Vector* body, Scope* sp);
void compile_entry_fun (Program* program, MethodValue* entry_fun);
void compile_slotstmt (SlotStmt* s, Program* program, Vector* body, Scope* sp);
void compile_var_stmt (ScopeVar *s, Program* program, Vector* body, Scope* sp);
void compile_scopestmt (ScopeStmt* s, Program* program, Vector* body, Scope* sp);
// util logics
Scope* make_scope ();
Value* make_null_val ();
Scope* get_root_scope ();
IntValue* make_int_val (int val);
Value* make_slot_val (Program* p, char* name);
MethodValue* make_method_value ();
StringValue* make_string_val (char* str);
int isGlobalScope (Scope* sp, char* name);
// make instruction operations
ByteIns* make_DropIns ();
ByteIns* make_ReturnIns ();
ByteIns* make_LitIns (int val_idx);
ByteIns* make_SetLocalIns (int slot_idx);
ByteIns* make_SetGlobalIns (int name_idx);
// register values in constant pool
int register_const_null (Program* p);
int register_const_int (Program* p, int val);
int register_const_str (Program* p, char* name);
int register_const_slot (Program* p, char* name);
int register_const_method (Program* p, MethodValue* mv);
// util functions
char* copy_str (const char *string);

//============================================================
//================= COMPILER FUNCTIONS =======================
//============================================================

int compile_int_exp(IntExp* e, Program* p, Vector* body, Scope* sp) {
	return register_const_int(p, e->value);
}

int compile_exp (Exp* e, Program* p, Vector* body, Scope* sp) {
	switch (e->tag) {
	case INT_EXP: {
		IntExp* e2 = (IntExp*)e;
		return compile_int_exp(e2, p, body, sp);
	}
	/*
	case NULL_EXP: {
		printf("null");
		break;
	}
	case PRINTF_EXP: {
		PrintfExp* e2 = (PrintfExp*)e;
		printf("printf(");
		print_string(e2->format);
		for (int i = 0; i < e2->nexps; i++) {
			printf(", ");
			compile_exp(e2->exps[i]);
		}
		printf(")");
		break;
	}
	case ARRAY_EXP: {
		ArrayExp* e2 = (ArrayExp*)e;
		printf("array(");
		compile_exp(e2->length);
		printf(", ");
		compile_exp(e2->init);
		printf(")");
		break;
	}
	case OBJECT_EXP: {
		ObjectExp* e2 = (ObjectExp*)e;
		printf("object : (");
		for (int i = 0; i < e2->nslots; i++) {
			if (i > 0) printf(" ");
			compile_slotstmt(e2->slots[i]);
		}
		printf(")");
		break;
	}
	case SLOT_EXP: {
		SlotExp* e2 = (SlotExp*)e;
		compile_exp(e2->exp);
		printf(".%s", e2->name);
		break;
	}
	case SET_SLOT_EXP: {
		SetSlotExp* e2 = (SetSlotExp*)e;
		compile_exp(e2->exp);
		printf(".%s = ", e2->name);
		compile_exp(e2->value);
		break;
	}
	case CALL_SLOT_EXP: {
		CallSlotExp* e2 = (CallSlotExp*)e;
		compile_exp(e2->exp);
		printf(".%s(", e2->name);
		for (int i = 0; i < e2->nargs; i++) {
			if (i > 0) printf(", ");
			compile_exp(e2->args[i]);
		}
		printf(")");
		break;
	}
	case CALL_EXP: {
		CallExp* e2 = (CallExp*)e;
		printf("%s(", e2->name);
		for (int i = 0; i < e2->nargs; i++) {
			if (i > 0) printf(", ");
			compile_exp(e2->args[i]);
		}
		printf(")");
		break;
	}
	case SET_EXP: {
		SetExp* e2 = (SetExp*)e;
		printf("%s = ", e2->name);
		compile_exp(e2->exp);
		break;
	}
	case IF_EXP: {
		IfExp* e2 = (IfExp*)e;
		printf("if ");
		compile_exp(e2->pred);
		printf(" : (");
		compile_scopestmt(e2->conseq);
		printf(") else : (");
		compile_scopestmt(e2->alt);
		printf(")");
		break;
	}
	case WHILE_EXP: {
		WhileExp* e2 = (WhileExp*)e;
		printf("while ");
		compile_exp(e2->pred);
		printf(" : (");
		compile_scopestmt(e2->body);
		printf(")");
		break;
	}
	case REF_EXP: {
		RefExp* e2 = (RefExp*)e;
		printf("%s", e2->name);
		break;
	}
	*/
	default:
		printf("Unrecognized Expression with tag %d\n", e->tag);
		exit(-1);
	}
	return -1;
}

void compile_slotstmt (SlotStmt* s, Program* program, Vector* body, Scope* sp) {
	switch (s->tag) {
	case VAR_STMT: {
		SlotVar* s2 = (SlotVar*)s;
		compile_exp(s2->exp, program, body, sp);
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
		compile_scopestmt(s2->body, program, body, sp);
		printf(")");
		break;
	}
	default:
		printf("Unrecognized slot statement with tag %d\n", s->tag);
		exit(-1);
	}
}

/*
	lit #0
	set global #1
	(set local #1)
	drop
*/
void compile_var_stmt (ScopeVar *s, Program* p, Vector* body, Scope* sp) {
	int exp_val_idx = compile_exp(s->exp, p, body, sp);
	int name_idx = register_const_str(p, s->name);
	vector_add(sp->vars, s->name);
	int local_fram_name_idx = sp->vars->size - 1;
	vector_add(body, make_LitIns(exp_val_idx));
	if (isGlobalScope(sp, s->name)) {
		// if global, then create a var slot and
		// push into constant pool, add the slot
		// idx into the global vars
		int slot_idx = register_const_slot(p, s->name);
		vector_add(p->slots, (void*)slot_idx);
		vector_add(body, make_SetGlobalIns(name_idx));
	} else {
		// if the sope is inside a object creation
		// create a var slot; push it into constant pool
		vector_add(body, make_SetLocalIns(local_fram_name_idx));
	}
	vector_add(body, make_DropIns());
}

void compile_scopestmt (ScopeStmt* s, Program* p, Vector* body, Scope* sp) {
	switch (s->tag) {
	case VAR_STMT: {
		ScopeVar* s2 = (ScopeVar*)s;
		compile_var_stmt(s2, p, body, sp);
		break;
	}
	case FN_STMT: {
		// search for local variables and
		// add it into the constant pool
		ScopeFn* s2 = (ScopeFn*)s;
		printf("defn %s (", s2->name);
		for (int i = 0; i < s2->nargs; i++) {
			if (i > 0) printf(", ");
			printf("%s", s2->args[i]);
		}
		printf(") : (");
		// compile_scopestmt(s2->body);
		printf(")");
		break;
	}
	case SEQ_STMT: {
		ScopeSeq* s2 = (ScopeSeq*)s;
		compile_scopestmt(s2->a, p, body, sp);
		compile_scopestmt(s2->b, p, body, sp);
		break;
	}
	case EXP_STMT: {
		ScopeExp* s2 = (ScopeExp*)s;
		// compile_exp(s2->exp);
		break;
	}
	default:
		printf("Unrecognized scope statement with tag %d\n", s->tag);
		exit(-1);
	}
}

void compile_entry_fun (Program* p, MethodValue* entry_f) {
	// entry function name
	entry_f->name = register_const_str(p, "entry35");
	// add entry function into the constant pool
	int entry_fun_idx = register_const_method(p, entry_f);
	p->entry = entry_fun_idx;
	int null_idx = register_const_null(p);
	// finally add load null and return
	vector_add(entry_f->code, make_LitIns(null_idx));
	vector_add(entry_f->code, make_ReturnIns());
}

Program* compile (ScopeStmt* stmt) {
	Program* program = (Program*)malloc(sizeof(Program));
	program->values = make_vector();
	program->slots = make_vector();
	// create entry function
	MethodValue* entry_fun = make_method_value();
	compile_scopestmt(stmt, program, entry_fun->code, get_root_scope());
	// finally add entry method
	compile_entry_fun(program, entry_fun);
	print_prog(program);
	printf("\n");
	exit(-1);
	// TODO: return the compiled byte code AST
}

//============================================================
//==================== UTIL LOGICS ===========================
//============================================================

Scope* search_var (Scope* sp, char* name) {
	if (sp == NULL) return sp;
	for (int i = 0; i < sp->vars->size; i++) {
		if (strcmp(vector_get(sp->vars, i), name) == 0) {
			return sp;
		}
	}
	return search_var((Scope*)sp->parent, name);
}

// search a var name in the scope and its parent scope
// return 1 if the nearest var declaration is in
// the global (root) scope; otherwise return 0
int isGlobalScope (Scope* sp, char* name) {
	Scope* res_sp = search_var(sp, name);
	if (res_sp == NULL) {
		printf("Error: isGlobalScope.\n");
		exit(-1);
	}
	if (res_sp == get_root_scope()) {
		return 1;
	} else {
		return 0;
	}
}

Scope* get_root_scope () {
	static Scope* root_scope = NULL;
	if (root_scope == NULL) {
		root_scope = make_scope(NULL);
	}
	return root_scope;
}

Scope* make_scope (Scope* parent) {
	Scope* sp = (Scope*)malloc(sizeof(Scope));
	sp->vars = make_vector();
	sp->parent = (_Scope*)parent;
	return sp;
}

Value* make_null_val () {
	static Value n = {NULL_VAL};
	return &n;
}

IntValue* make_int_val (int val) {
	IntValue* iv = (IntValue*)malloc(sizeof(IntValue));
	iv->tag = INT_VAL;
	iv->value = val;
	return iv;
}

StringValue* make_string_val (char* str) {
	StringValue* sv = (StringValue*)malloc(sizeof(StringValue));
	sv->tag = STRING_VAL;
	sv->value = copy_str(str);
	return sv;
}

Value* make_slot_val (Program* p, char* name) {
	SlotValue* sv = (SlotValue*)malloc(sizeof(SlotValue));
	sv->tag = SLOT_VAL;
	sv->name = register_const_str(p, name);
	return (Value*)sv;
}

MethodValue* make_method_value () {
	MethodValue* method_val =
	    (MethodValue*)malloc(sizeof(MethodValue));
	method_val->tag = METHOD_VAL;
	method_val->name = -1;
	// TODO: set entry function idx
	method_val->nargs = 0;
	method_val->nlocals = 0;
	method_val->code = make_vector();
	return method_val;
}

int register_const_null (Program* p) {
	// search constant pool
	int len = p->values->size;
	for (int i = 0; i < len; i++) {
		Value* val = vector_get(p->values, i);
		if (val->tag == NULL_VAL) {
			return i;
		}
	}
	Value* nv = make_null_val();
	vector_add(p->values, nv);
	return p->values->size - 1;
}

int register_const_int (Program* p, int int_val) {
	// search constant pool
	int len = p->values->size;
	for (int i = 0; i < len; i++) {
		Value* val = vector_get(p->values, i);
		if (val->tag == INT_VAL &&
		        ((IntValue*)val)->value == int_val) {
			return i;
		}
	}
	IntValue* iv = make_int_val(int_val);
	vector_add(p->values, iv);
	return p->values->size - 1;
}

int register_const_str (Program* p, char* name) {
	// search constant pool
	int len = p->values->size;
	for (int i = 0; i < len; i++) {
		Value* val = vector_get(p->values, i);
		if (val->tag == STRING_VAL &&
		        strcmp(((StringValue*)val)->value, name) == 0) {
			return i;
		}
	}
	StringValue* sv = make_string_val(name);
	vector_add(p->values, sv);
	return p->values->size - 1;
}

int register_const_slot (Program* p, char* name) {
	Value* slot = make_slot_val(p, name);
	vector_add(p->values, slot);
	return p->values->size - 1;
}

int register_const_method(Program* p, MethodValue* mv) {
	vector_add(p->values, mv);
	return p->values->size - 1;
}

//============================================================
//================ BYTECODE FUNCITONS ========================
//============================================================

ByteIns* make_SetGlobalIns (int name_idx) {
	SetGlobalIns* ins = (SetGlobalIns*)malloc(sizeof(SetGlobalIns));
	ins->tag = SET_GLOBAL_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_SetLocalIns (int slot_idx) {
	SetLocalIns* ins = (SetLocalIns*)malloc(sizeof(SetLocalIns));
	ins->tag = SET_LOCAL_OP;
	ins->idx = slot_idx;
	return (ByteIns*)ins;
}

ByteIns* make_ReturnIns () {
	ByteIns* ins = (ByteIns*)malloc(sizeof(ByteIns));
	ins->tag = RETURN_OP;
	return (ByteIns*)ins;
}

ByteIns* make_DropIns () {
	ByteIns* ins = (ByteIns*)malloc(sizeof(ByteIns));
	ins->tag = DROP_OP;
	return (ByteIns*)ins;
}

ByteIns* make_LitIns (int val_idx) {
	LitIns* ins = (LitIns*)malloc(sizeof(LitIns));
	ins->tag = LIT_OP;
	ins->idx = val_idx;
	return (ByteIns*)ins;
}

//============================================================
//==================== UTIL FUNCITONS ========================
//============================================================

char* copy_str (const char *string) {
	char *stringcopy = malloc (1 + strlen (string));
	if (stringcopy) {
		strcpy (stringcopy, string);
	} else {
		fprintf (stderr, "malloc failure!");
	}
	return stringcopy;
}
