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

typedef enum {
	// the root scope is
	// a frame scope
	/*ROOT_SCOPE,*/
	BRANCH_SCOPE,
	FRAME_SCOPE,
	OBJECT_SCOPE
} ScpTag;

typedef struct {
	ScpTag tag;
	Vector* vars;
} _Scope;

typedef struct {
	ScpTag tag;
	Vector* vars;
	_Scope* parent;
} Scope;

typedef struct {
	char* name;	// name of the variable
	int idx;	// its index in the local frame
} VarItem;

Program* compile (ScopeStmt* stmt);
int compile_null_exp (Program* p, Vector* body, Scope* sp);
void compile_entry_fun (Program* p, MethodValue* entry_fun);
int compile_exp (Exp* e, Program* p, Vector* body, Scope* sp);
int compile_if_exp (IfExp* e, Program* p, Vector* body, Scope* sp);
int compile_int_exp (IntExp* e, Program* p, Vector* body, Scope* sp);
int compile_set_exp (SetExp* e, Program* p, Vector* body, Scope* sp);
int compile_ref_exp (Program* p, RefExp* e, Vector* body, Scope* sp);
int compile_slot_exp (SlotExp* e, Program* p, Vector* body, Scope* sp);
int compile_call_exp (CallExp* e, Program* p, Vector* body, Scope* sp);
int compile_slotstmt (SlotStmt* s, Program* p, Vector* body, Scope* sp);
int compile_array_exp (ArrayExp* e, Program* p, Vector* body, Scope* sp);
int compile_while_exp (WhileExp* e, Program* p, Vector* body, Scope* sp);
void compile_var_stmt (ScopeVar *s, Program* p, Vector* body, Scope* sp);
int compile_object_exp (ObjectExp* e, Program* p, Vector* body, Scope* sp);
int compile_printf_exp (PrintfExp* e, Program* p, Vector* body, Scope* sp);
void compile_scopestmt (ScopeStmt* s, Program* p, Vector* body, Scope* sp);
int compile_var_slot_stmt (SlotVar *s, Program* p, Vector* body, Scope* sp);
int compile_set_slot_exp (SetSlotExp* e, Program* p, Vector* body, Scope* sp);
int compile_fn_slot_stmt (SlotMethod* s, Program* p, Vector* body, Scope* sp);
int compile_call_slot_exp (CallSlotExp* e, Program* p, Vector* body, Scope* sp);
// util logics
char* get_end_label();
char* get_test_label();
char* get_loop_label();
char* get_entry_label();
char* get_conseq_label();
int get_next_entry_id ();
Scope* get_root_scope ();
int get_next_entry_id ();
int isGlobalScope (Scope* sp, char* name);
Scope* search_var (Scope* sp, char* name);
char* get_label(int entry_id, char* prefix);
Scope* make_scope (Scope* parent, ScpTag tag);
Scope* get_containing_frame_scope (Scope* sp);
int register_var_in_scope (Scope* sp, char* name);
int get_var_idx_in_frame_scope (Scope* sp, char *name);
void count_nlocals_exp (Exp* e, MethodValue* mv, Vector* vars);
void count_nlocals (ScopeStmt* s, MethodValue* mv, Vector* vars);
// make value methods
Value* make_null_val ();
IntValue* make_int_val (int val);
MethodValue* make_method_value ();
StringValue* make_string_val (char* str);
ClassValue* make_class_val(Vector* slots);
Value* make_slot_val (Program* p, char* name);
// make instruction operations
ByteIns* make_DropIns ();
ByteIns* make_ArrayIns ();
ByteIns* make_ReturnIns ();
ByteIns* make_LitIns (int val_idx);
ByteIns* make_SlotIns(int name_idx);
ByteIns* make_GotoIns (int name_idx);
ByteIns* make_LabelIns (int name_idx);
ByteIns* make_BranchIns (int name_idx);
ByteIns* make_ObjectIns(int class_idx);
ByteIns* make_SetSlotIns(int name_idx);
ByteIns* make_GetLocalIns (int slot_idx);
ByteIns* make_SetLocalIns (int slot_idx);
ByteIns* make_GetGlobalIns (int name_idx);
ByteIns* make_SetGlobalIns (int name_idx);
ByteIns* make_CallIns (int name_idx, int arity);
ByteIns* make_CallSlotIns (int f_name_idx, int arity);
ByteIns* make_PrintfIns (int format_str_idx, int arity);
// register values in constant pool
int register_const_null (Program* p);
int register_const_int (Program* p, int val);
int register_const_str (Program* p, char* name);
int register_const_slot (Program* p, char* name);
int register_const_class(Program* p, ClassValue* cv);
int register_const_method (Program* p, MethodValue* mv);
// util functions
char* copy_str (const char *string);

//============================================================
//================= COMPILER FUNCTIONS =======================
//============================================================

int compile_exp (Exp* e, Program* p, Vector* body, Scope* sp) {
	switch (e->tag) {
	case INT_EXP: {
		IntExp* e2 = (IntExp*)e;
		return compile_int_exp(e2, p, body, sp);
	}
	case NULL_EXP: {
		return compile_null_exp(p, body, sp);
	}
	case PRINTF_EXP: {
		PrintfExp* e2 = (PrintfExp*)e;
		return compile_printf_exp(e2, p, body, sp);
	}
	case ARRAY_EXP: {
		ArrayExp* e2 = (ArrayExp*)e;
		return compile_array_exp(e2, p, body, sp);
	}
	case OBJECT_EXP: {
		ObjectExp* e2 = (ObjectExp*)e;
		return compile_object_exp(e2, p, body, sp);
	}
	case SLOT_EXP: {
		SlotExp* e2 = (SlotExp*)e;
		return compile_slot_exp(e2, p, body, sp);
	}
	case SET_SLOT_EXP: {
		SetSlotExp* e2 = (SetSlotExp*)e;
		return compile_set_slot_exp(e2, p, body, sp);
	}
	case CALL_SLOT_EXP: {
		CallSlotExp* e2 = (CallSlotExp*)e;
		return compile_call_slot_exp(e2, p, body, sp);
	}
	case CALL_EXP: {
		CallExp* e2 = (CallExp*)e;
		return compile_call_exp(e2, p, body, sp);
	}
	case SET_EXP: {
		SetExp* e2 = (SetExp*)e;
		return compile_set_exp(e2, p, body, sp);
	}
	case IF_EXP: {
		IfExp* e2 = (IfExp*)e;
		return compile_if_exp(e2, p, body, sp);
	}
	case WHILE_EXP: {
		WhileExp* e2 = (WhileExp*)e;
		return compile_while_exp(e2, p, body, sp);
	}
	case REF_EXP: {
		RefExp* e2 = (RefExp*)e;
		return compile_ref_exp(p, e2, body, sp);
		break;
	}
	default:
		printf("Unrecognized Expression with tag %d\n", e->tag);
		// TODO uncomment the following statement
		// exit(-1);
	}
	return -1;
}

int compile_slot_exp (SlotExp* e, Program* p, Vector* body, Scope* sp) {
	compile_exp(e->exp, p, body, sp);
	int slot_name_idx = register_const_str(p, e->name);
	vector_add(body, make_SlotIns(slot_name_idx));
	return -1;
}

int compile_set_slot_exp (SetSlotExp* e, Program* p, Vector* body, Scope* sp) {
	compile_exp(e->exp, p, body, sp);
	compile_exp(e->value, p, body, sp);
	int slot_name_idx = register_const_str(p, e->name);
	vector_add(body, make_SetSlotIns(slot_name_idx));
	return -1;
}

int compile_object_exp (ObjectExp* e, Program* p, Vector* body, Scope* sp) {
	// compile for the parent object
	compile_exp(e->parent, p, body, sp);
	Scope* nsp = make_scope(sp, OBJECT_SCOPE);
	Vector* slots = make_vector();
	// compile for each slot
	for (int i = 0; i < e->nslots; i++) {
		int slot_idx = compile_slotstmt(e->slots[i], p, body, nsp);
		vector_add(slots, (void*)slot_idx);
	}
	int class_idx = register_const_class(p, make_class_val(slots));
	vector_add(body, make_ObjectIns(class_idx));
	return -1;
}

int compile_array_exp (ArrayExp* e, Program* p, Vector* body, Scope* sp) {
	compile_exp(e->length, p, body, sp);
	compile_exp(e->init, p, body, sp);
	vector_add(body, make_ArrayIns());
	return -1;
}

int compile_while_exp (WhileExp* e, Program* p, Vector* body, Scope* sp) {
	int test_idx = register_const_str(p, get_test_label());
	int loop_idx = register_const_str(p, get_loop_label());
	// add goto instruction (to the test expression part)
	vector_add(body, make_GotoIns(test_idx));
	// add the label for the body
	vector_add(body, make_LabelIns(loop_idx));
	// compile the body
	compile_scopestmt(e->body, p, body, make_scope(sp, BRANCH_SCOPE));
	// at the end of the while body, insert a drop inst
	vector_add(body, make_DropIns());
	// add label for the test expression part
	vector_add(body, make_LabelIns(test_idx));
	// compile the conditional expression
	compile_exp(e->pred, p, body, sp);
	// add the branch instruction
	vector_add(body, make_BranchIns(loop_idx));
	int null_idx = register_const_null(p);
	// finally add a lit null operation
	vector_add(body, make_LitIns(null_idx));
	return -1;
}

int compile_if_exp (IfExp* e, Program* p, Vector* body, Scope* sp) {
	int conseq_idx = register_const_str(p, get_conseq_label());
	int end_idx = register_const_str(p, get_end_label());
	// compile the conditional expression
	compile_exp(e->pred, p, body, sp);
	// add the branch instruction
	vector_add(body, make_BranchIns(conseq_idx));
	// compile else case
	compile_scopestmt(e->alt, p, body, make_scope(sp, BRANCH_SCOPE));
	// add goto instruction
	vector_add(body, make_GotoIns(end_idx));
	// add the label for then
	vector_add(body, make_LabelIns(conseq_idx));
	// compile then case
	compile_scopestmt(e->conseq, p, body, make_scope(sp, BRANCH_SCOPE));
	// add end label
	vector_add(body, make_LabelIns(end_idx));
	return -1;
}

int compile_printf_exp (PrintfExp* e, Program* p, Vector* body, Scope* sp) {
	for (int i = 0; i < e->nexps; i++) {
		compile_exp(e->exps[i], p, body, sp);
	}
	int format_str_idx = register_const_str(p, e->format);
	vector_add(body, make_PrintfIns(format_str_idx, e->nexps));
	return -1;
}

int compile_set_exp (SetExp* e, Program* p, Vector* body, Scope* sp) {
	int var_idx;
	compile_exp(e->exp, p, body, sp);
	if (isGlobalScope(sp, e->name)) {
		var_idx = register_const_str(p, e->name);
		vector_add(body, make_SetGlobalIns(var_idx));
	} else {
		var_idx = get_var_idx_in_frame_scope(sp, e->name);
		if (var_idx == -1) {
			printf("Error: unknown property: %s.\n", e->name);
			exit(-1);
		}
		vector_add(body, make_SetLocalIns(var_idx));
	}
	return -1;
}

int compile_ref_exp (Program* p, RefExp* e, Vector* body, Scope* sp) {
	int var_idx;
	if (isGlobalScope(sp, e->name)) {
		var_idx = register_const_str(p, e->name);
		vector_add(body, make_GetGlobalIns(var_idx));
	} else {
		var_idx = get_var_idx_in_frame_scope(sp, e->name);
		if (var_idx != -1) {
			vector_add(body, make_GetLocalIns(var_idx));
		} else if (strcmp(e->name, "this") == 0) {
			int this_idx = register_const_str(p, "this");
			vector_add(body, make_GetGlobalIns(this_idx));
		} else {
			printf("Error: unknown reference: %s.\n", e->name);
			exit(-1);
		}
	}
	return var_idx;
}

int compile_null_exp (Program* p, Vector* body, Scope* sp) {
	int val_idx = register_const_null(p);
	vector_add(body, make_LitIns(val_idx));
	return val_idx;
}

int compile_int_exp (IntExp* e, Program* p, Vector* body, Scope* sp) {
	int val_idx = register_const_int(p, e->value);
	vector_add(body, make_LitIns(val_idx));
	return val_idx;
}

int compile_call_slot_exp (CallSlotExp* e, Program* p, Vector* body, Scope* sp) {
	compile_exp(e->exp, p, body, sp);
	// generate get instrctions for loading arguments
	for (int i = 0; i < e->nargs; i++) {
		compile_exp(e->args[i], p, body, sp);
	}
	int f_name_idx = register_const_str(p, e->name);
	vector_add(body, make_CallSlotIns(f_name_idx, e->nargs + 1));
	return -1;
}

int compile_call_exp (CallExp* e, Program* p, Vector* body, Scope* sp) {
	int name_idx = register_const_str(p, e->name);
	// generate get instrctions for loading arguments
	for (int i = 0; i < e->nargs; i++) {
		compile_exp(e->args[i], p, body, sp);
	}
	vector_add(body, make_CallIns(name_idx, e->nargs));
	return -1;
}

int compile_var_slot_stmt (SlotVar *s, Program* p, Vector* body, Scope* sp) {
	compile_exp(s->exp, p, body, (Scope*)sp->parent);
	// register the var name in the current scope
	register_var_in_scope(sp, s->name);
	// if the sope is inside a object creation
	// register a slot and return its index
	int name_idx = register_const_str(p, s->name);
	int slot_idx = register_const_slot(p, s->name);
	return slot_idx;
}

int compile_fn_slot_stmt (SlotMethod* s, Program* p, Vector* body, Scope* sp) {
	MethodValue* method = make_method_value();
	// for slot methods, there is an additional "this" argument
	method->nargs = s->nargs + 1;
	// put this function name into the scope
	// which will be used later for checking
	// global or local scope
	register_var_in_scope(sp, s->name);
	// count_nlocals(s->body, method, make_vector());
	Scope* nsp = make_scope(sp, FRAME_SCOPE);
	// for slot methods, the first argument is "this" argument
	register_var_in_scope(nsp, "this");
	for (int i = 0; i < s->nargs; i++) {
		register_var_in_scope(nsp, s->args[i]);
	}
	compile_scopestmt(s->body, p, method->code, nsp);
	// register the function's name
	method->name = register_const_str(p, s->name);
	// if the last instruction is a drop, remove it
	ByteIns* last_ins = vector_get(method->code,
	                               method->code->size - 1);
	if (last_ins->tag == DROP_OP) {
		vector_pop(method->code);
	}
	// add a return statment
	vector_add(method->code, make_ReturnIns());
	int method_idx = register_const_method(p, method);
	// if this is the global scope, add
	// into the global var list
	if (isGlobalScope(sp, s->name)) {
		vector_add(p->slots, (void*)method_idx);
	}
	// finally count the number of
	// local vars in the frame scope
	method->nlocals = nsp->vars->size - method->nargs;
	return method_idx;
}

int compile_slotstmt (SlotStmt* s, Program* p, Vector* body, Scope* sp) {
	switch (s->tag) {
	case VAR_STMT: {
		SlotVar* s2 = (SlotVar*)s;
		return compile_var_slot_stmt(s2, p, body, sp);
	}
	case FN_STMT: {
		SlotMethod* s2 = (SlotMethod*)s;
		return compile_fn_slot_stmt(s2, p, body, sp);
	}
	default:
		printf("Unrecognized slot statement with tag %d\n", s->tag);
		exit(-1);
	}
	return -1;
}

/*
	lit #0
	set global #1
	(set local #1)
	drop
*/
void compile_var_stmt (ScopeVar *s, Program* p, Vector* body, Scope* sp) {
	int exp_val_idx = compile_exp(s->exp, p, body, sp);
	int local_fram_name_idx = register_var_in_scope(sp, s->name);
	// vector_add(body, make_LitIns(exp_val_idx));
	if (isGlobalScope(sp, s->name)) {
		// if global, then create a var slot and
		// push into constant pool, add the slot
		// idx into the global vars
		int name_idx = register_const_str(p, s->name);
		int slot_idx = register_const_slot(p, s->name);
		vector_add(p->slots, (void*)slot_idx);
		vector_add(body, make_SetGlobalIns(name_idx));
	} else {
		// if the sope is inside a function
		// just a set local instruction
		vector_add(body, make_SetLocalIns(local_fram_name_idx));
	}
	vector_add(body, make_DropIns());
}

void compile_fn_stmt (ScopeFn* s, Program* p, Vector* body, Scope* sp) {
	MethodValue* method = make_method_value();
	method->nargs = s->nargs;
	// put this function name into the scope
	// which will be used later for checking
	// global or local scope
	register_var_in_scope(sp, s->name);
	// count_nlocals(s->body, method, make_vector());
	Scope* nsp = make_scope(sp, FRAME_SCOPE);
	for (int i = 0; i < s->nargs; i++) {
		register_var_in_scope(nsp, s->args[i]);
	}
	compile_scopestmt(s->body, p, method->code, nsp);
	// register the function's name
	method->name = register_const_str(p, s->name);
	// if the last instruction is a drop, remove it
	ByteIns* last_ins = vector_get(method->code,
	                               method->code->size - 1);
	if (last_ins->tag == DROP_OP) {
		vector_pop(method->code);
	}
	// add a return statment
	vector_add(method->code, make_ReturnIns());
	int method_idx = register_const_method(p, method);
	// if this is the global scope, add
	// into the global var list
	if (isGlobalScope(sp, s->name)) {
		vector_add(p->slots, (void*)method_idx);
	}
	// finally count the number of
	// local vars in the frame scope
	method->nlocals = nsp->vars->size - method->nargs;
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
		compile_fn_stmt(s2, p, body, sp);
		break;
	}
	case SEQ_STMT: {
		ScopeSeq* s2 = (ScopeSeq*)s;
		compile_scopestmt(s2->a, p, body, sp);
		if (s2->a->tag == EXP_STMT) {
			vector_add(body, make_DropIns());
		}
		compile_scopestmt(s2->b, p, body, sp);
		break;
	}
	case EXP_STMT: {
		ScopeExp* s2 = (ScopeExp*)s;
		compile_exp(s2->exp, p, body, sp);
		break;
	}
	default:
		printf("Unrecognized scope statement with tag %d\n", s->tag);
		exit(-1);
	}
}

void compile_entry_fun (Program* p, MethodValue* entry_f) {
	// entry function name
	int null_idx = register_const_null(p);
	entry_f->name = register_const_str(p, get_entry_label());
	// add entry function into the constant pool
	int entry_fun_idx = register_const_method(p, entry_f);
	p->entry = entry_fun_idx;
	// finally add load null and return
	vector_add(entry_f->code, make_DropIns());
	vector_add(entry_f->code, make_LitIns(null_idx));
	vector_add(entry_f->code, make_ReturnIns());
}

Program* compile (ScopeStmt* stmt) {
	Program* p = (Program*)malloc(sizeof(Program));
	p->values = make_vector();
	p->slots = make_vector();
	// create entry function
	MethodValue* entry_fun = make_method_value();
	compile_scopestmt(stmt, p, entry_fun->code, get_root_scope());
	// finally add entry method
	compile_entry_fun(p, entry_fun);
	// TODO remove the following two lines of code
	// print_prog(p);
	// exit(-1);
	return p;
}

//============================================================
//==================== UTIL LOGICS ===========================
//============================================================

char* get_test_label() {
	return get_label(get_next_entry_id(), "test");
}

char* get_loop_label() {
	return get_label(get_next_entry_id(), "loop");
}

char* get_conseq_label() {
	return get_label(get_next_entry_id(), "conseq");
}

char* get_end_label() {
	return get_label(get_next_entry_id(), "end");
}

char* get_entry_label() {
	return get_label(get_next_entry_id(), "entry");
}

char* get_label(int entry_id, char* prefix) {
	char* ret = (char*)malloc(12 * sizeof(char));
	sprintf(ret, "%s%d", prefix, entry_id);
	return ret;
}

int get_next_entry_id () {
	static int cur_entry_id = 36;
	return cur_entry_id++;
}

void count_nlocals_exp (Exp* e, MethodValue* method_val, Vector* vars) {
	switch (e->tag) {
	case IF_EXP: {
		IfExp* e2 = (IfExp*)e;
		count_nlocals(e2->conseq, method_val, vars);
		count_nlocals(e2->alt, method_val, vars);
	}
	case WHILE_EXP: {
		WhileExp* e2 = (WhileExp*)e;
		count_nlocals(e2->body, method_val, vars);
	}
	default: return;
	}
}

void count_nlocals (ScopeStmt* s, MethodValue* method_val, Vector* vars) {
	switch (s->tag) {
	case EXP_STMT: {
		ScopeExp* s2 = (ScopeExp*)s;
		count_nlocals_exp(s2->exp, method_val, vars);
		break;
	}
	case FN_STMT: {
		method_val->nlocals++;
		ScopeFn* s2 = (ScopeFn*)s;
		// TODO: does not allow redefinition
		// with the same function name
		vector_add(vars, s2->name);
		break;
	}
	case VAR_STMT: {
		ScopeVar *s2 = (ScopeVar *)s;
		int found = 0;
		// search the var name in the current scope
		for (int i = 0; i < vars->size; i++)
			if (strcmp(vector_get(vars, i), s2->name) == 0)
				found = 1;
		// if the var is not defined
		// register the var name in the current scope
		if (found == 0) {
			method_val->nlocals++;
			vector_add(vars, s2->name);
		}
		break;
	}
	case SEQ_STMT: {
		ScopeSeq* s2 = (ScopeSeq*)s;
		count_nlocals(s2->a, method_val, vars);
		count_nlocals(s2->b, method_val, vars);
		break;
	}
	default:
		printf("Unrecognized scope statement with tag %d\n", s->tag);
		exit(-1);
	}
}

// returns the inner most frame scope
// of scope sp
Scope* get_containing_frame_scope (Scope* sp) {
	Scope* ret = sp;
	while (ret != NULL) {
		if (ret->tag == FRAME_SCOPE ||
		        ret->tag == OBJECT_SCOPE)
			return ret;
		ret = (Scope*)ret->parent;
	}
	return ret;
}

int register_var_in_scope (Scope* sp, char* name) {
	// search the var name in the current scope
	for (int i = 0; i < sp->vars->size; i++) {
		if (strcmp(vector_get(sp->vars, i), name) == 0) {
			printf("Error: redefined variable: %s.\n", name);
		}
	}
	// register it in the inner most surrounding frame scope
	VarItem* item = (VarItem*)malloc(sizeof(VarItem));
	Scope* fsp = get_containing_frame_scope(sp);
	item->idx = fsp->vars->size;
	if (fsp != sp) {
		item->name = copy_str("*_*_*");
	} else {
		item->name = name;
	}
	vector_add(fsp->vars, item);
	// register it in the current scope
	if (fsp != sp) {
		VarItem* local_item = (VarItem*)malloc(sizeof(VarItem));
		local_item->name = name;
		local_item->idx = item->idx;
		vector_add(sp->vars, local_item);
	}
	return item->idx;
}

int get_var_idx_in_frame_scope (Scope* sp, char *name) {
	if (sp == NULL) {
		printf("ERROR: Could not resolve variable %s in environment.\n", name);
		exit(-1);
	}
	// search the var name in the current scope
	for (int i = 0; i < sp->vars->size; i++) {
		VarItem* vi = vector_get(sp->vars, i);
		if (strcmp(vi->name, name) == 0) {
			return vi->idx;
		}
	}
	// will not search further if
	// current scope is a frame scope
	int is_this = !strcmp("this", name);
	if ((!is_this && sp->tag == FRAME_SCOPE) ||
	        (is_this && sp->tag == OBJECT_SCOPE)) {
		printf("ERROR[2]: Could not resolve variable %s in environment.\n", name);
		exit(-1);
	}
	return get_var_idx_in_frame_scope((Scope*)sp->parent, name);
}

Scope* search_var (Scope* sp, char* name) {
	if (sp == NULL) {
		printf("ERROR[3]: Could not resolve variable %s in environment.\n", name);
		exit(-1);
	}
	for (int i = 0; i < sp->vars->size; i++) {
		VarItem* vi = vector_get(sp->vars, i);
		if (strcmp(vi->name, name) == 0) {
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
		root_scope = make_scope(NULL, FRAME_SCOPE);
	}
	return root_scope;
}

Scope* make_scope (Scope* parent, ScpTag tag) {
	Scope* sp = (Scope*)malloc(sizeof(Scope));
	sp->tag = tag;
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

ClassValue* make_class_val(Vector* slots) {
	ClassValue* class_val =
	    (ClassValue*)malloc(sizeof(ClassValue));
	class_val->tag = CLASS_VAL;
	class_val->slots = slots;
	return class_val;
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

int register_const_class(Program* p, ClassValue* cv) {
	vector_add(p->values, cv);
	return p->values->size - 1;
}

//============================================================
//================ BYTECODE FUNCITONS ========================
//============================================================

ByteIns* make_GetGlobalIns (int name_idx) {
	GetGlobalIns* ins = (GetGlobalIns*)malloc(sizeof(GetGlobalIns));
	ins->tag = GET_GLOBAL_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_SetGlobalIns (int name_idx) {
	SetGlobalIns* ins = (SetGlobalIns*)malloc(sizeof(SetGlobalIns));
	ins->tag = SET_GLOBAL_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_GetLocalIns (int slot_idx) {
	GetLocalIns* ins = (GetLocalIns*)malloc(sizeof(GetLocalIns));
	ins->tag = GET_LOCAL_OP;
	ins->idx = slot_idx;
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

ByteIns* make_ArrayIns () {
	ByteIns* ins = (ByteIns*)malloc(sizeof(ByteIns));
	ins->tag = ARRAY_OP;
	return (ByteIns*)ins;
}

ByteIns* make_LitIns (int val_idx) {
	LitIns* ins = (LitIns*)malloc(sizeof(LitIns));
	ins->tag = LIT_OP;
	ins->idx = val_idx;
	return (ByteIns*)ins;
}

ByteIns* make_CallIns (int name_idx, int arity) {
	CallIns* ins = (CallIns*)malloc(sizeof(CallIns));
	ins->tag = CALL_OP;
	ins->name = name_idx;
	ins->arity = arity;
	return (ByteIns*)ins;
}

ByteIns* make_CallSlotIns (int f_name_idx, int arity) {
	CallSlotIns* ins = (CallSlotIns*)malloc(sizeof(CallSlotIns));
	ins->tag = CALL_SLOT_OP;
	ins->name = f_name_idx;
	ins->arity = arity;
	return (ByteIns*)ins;
}

ByteIns* make_PrintfIns (int format_str_idx, int arity) {
	PrintfIns* ins = (PrintfIns*)malloc(sizeof(PrintfIns));
	ins->tag = PRINTF_OP;
	ins->format = format_str_idx;
	ins->arity = arity;
	return (ByteIns*)ins;
}

ByteIns* make_LabelIns (int name_idx) {
	LabelIns* ins = (LabelIns*)malloc(sizeof(LabelIns));
	ins->tag = LABEL_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_BranchIns (int name_idx) {
	BranchIns* ins = (BranchIns*)malloc(sizeof(BranchIns));
	ins->tag = BRANCH_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_GotoIns (int name_idx) {
	GotoIns* ins = (GotoIns*)malloc(sizeof(GotoIns));
	ins->tag = GOTO_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_SlotIns(int name_idx) {
	SlotIns* ins = (SlotIns*)malloc(sizeof(SlotIns));
	ins->tag = SLOT_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_SetSlotIns(int name_idx) {
	SetSlotIns* ins = (SetSlotIns*)malloc(sizeof(SetSlotIns));
	ins->tag = SET_SLOT_OP;
	ins->name = name_idx;
	return (ByteIns*)ins;
}

ByteIns* make_ObjectIns(int class_idx) {
	ObjectIns* ins = (ObjectIns*)malloc(sizeof(ObjectIns));
	ins->tag = OBJECT_OP;
	ins->class = class_idx;
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
