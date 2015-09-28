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

Program* compile (ScopeStmt* stmt);
int compile_exp (Exp* e, Program* program, Vector* body);
int compile_int_exp(IntExp* e, Program* p, Vector* body);
void compile_entry_fun (Program* program, MethodValue* entry_fun);
void compile_slotstmt (SlotStmt* s, Program* program, Vector* body);
void compile_var_stmt (ScopeVar *s, Program* program, Vector* body);
void compile_scopestmt (ScopeStmt* s, Program* program, Vector* body);
// util logics
IntValue* make_int_val (int val);
MethodValue* make_method_value ();
StringValue* make_string_val (char* str);
// register values in constant pool
int register_const_int (Program* p, int val);
int register_const_str (Program* p, char* name);
int register_method(Program* p, MethodValue* mv);
// util functions
char* copy_str (const char *string);

//============================================================
//================= COMPILER FUNCTIONS =======================
//============================================================

int compile_int_exp(IntExp* e, Program* p, Vector* body) {
	return register_const_int(p, e->value);
}

int compile_exp (Exp* e, Program* p, Vector* body) {
	switch (e->tag) {
	case INT_EXP: {
		IntExp* e2 = (IntExp*)e;
		return compile_int_exp(e2, p, body);
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

void compile_slotstmt (SlotStmt* s, Program* program, Vector* body) {
	switch (s->tag) {
	case VAR_STMT: {
		SlotVar* s2 = (SlotVar*)s;
		compile_exp(s2->exp, program, body);
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
		compile_scopestmt(s2->body, program, body);
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
	drop
*/
void compile_var_stmt (ScopeVar *s, Program* p, Vector* body) {
	int exp_val_idx = compile_exp(s->exp, p, body);
	int name_idx = register_const_str(p, s->name);
	/*
	vector_add(body, make_LitIns(exp_val_idx));
	// if global scope, generate set global function
	// TBD
	// if local scope, generate set local function
	// TBD
	vector_add(body, make_DropIns());
	*/
}

void compile_scopestmt (ScopeStmt* s, Program* p, Vector* body) {
	switch (s->tag) {
	case VAR_STMT: {
		ScopeVar* s2 = (ScopeVar*)s;
		compile_var_stmt(s2, p, body);
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
		compile_scopestmt(s2->a, p, body);
		compile_scopestmt(s2->b, p, body);
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
	vector_add(p->values, entry_f);
	int entry_fun_idx = p->values->size - 1;
	p->entry = entry_fun_idx;
}

Program* compile (ScopeStmt* stmt) {
	Program* program = (Program*)malloc(sizeof(Program));
	program->values = make_vector();
	program->slots = make_vector();
	// create entry function
	MethodValue* entry_fun = make_method_value();
	compile_scopestmt(stmt, program, entry_fun->code);
	printf("\n");
	// finally add entry method
	compile_entry_fun(program, entry_fun);
	printf("\n");
	printf("\n");
	print_prog(program);
	printf("\n");
	exit(-1);
	// TODO: return the compiled byte code AST
}

//============================================================
//==================== UTIL LOGICS ===========================
//============================================================

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

int register_const_str(Program* p, char* name) {
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

int register_method(Program* p, MethodValue* mv) {
	vector_add(p->values, mv);
	return p->values->size - 1;
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
