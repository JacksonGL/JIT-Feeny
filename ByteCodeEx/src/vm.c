#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include "utils.h"
#include "bytecode.h"
#include "vm.h"
#include "uthash.h"

//============================================================
//=============== BASIC DATA STRUCTURE =======================
//============================================================

typedef enum {
	NULL_OBJ,
	INT_OBJ,
	STR_OBJ,
	SLOT_OBJ,
	METHOD_OBJ,
	CLASS_OBJ,
	OBJ_OBJ
} ObjTag;

typedef int ValIndex;

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
	char* value;
} StrObj;

typedef struct {
	ObjTag type;
	ValIndex name_str_index;
} SlotObj;

typedef struct {
	ObjTag type;
	ValIndex name_str_index;
	int num_args;
	int num_local_vars;
	Vector* ins_vec_ptr; // all instructions
} MethodObj;

typedef struct {
	ObjTag type;
	Vector* slot_vec_ptr;
} ClassObj;

typedef struct {
	ObjTag type;
	Vector* slot_vec_ptr;
	Obj* parent_obj_ptr;
	// ClassObj* class_ptr;
} ObjObj;

typedef struct {
	Vector* slot_vec_ptr;
	// address of the call byte
	// code instruction, this is
	// also the return address
	ByteIns* call_ins_ptr;
} _Frame;

typedef struct {
	Vector* slot_vec_ptr;
	// address of the call byte
	// code instruction, this is
	// also the return address
	ByteIns* call_ins_ptr;
	// caller's frame
	_Frame* call_frame_ptr;
} Frame;


typedef struct {
	const char *key;	// hashtable item key
	ValIndex value;		// hashtable item value
	UT_hash_handle hh;	// used for internal book keeping, ignore it
} Hashable;

ValIndex find_item(Hashable* table, char* key);
void add_item(Hashable* table, char* key, ValIndex value);

void exec_value (Value* v);
void exec_prog (Program* p);
void exec_ins (ByteIns* ins);

Frame* create_frame (Vector* slot_vec_ptr, ByteIns* call_ins_ptr, Frame* call_frame_ptr);

//============================================================
// hashtable implementations

void add_item(Hashable* table, char* key, ValIndex value) {
	Hashable* item_ptr = (Hashable*)malloc(sizeof(Hashable));
	item_ptr->key = key;
	item_ptr->value = value;
	HASH_ADD_KEYPTR(hh, table, item_ptr->key, strlen(item_ptr->key), item_ptr);
}

ValIndex find_item(Hashable* table, char* key) {
	Hashable* item;
	HASH_FIND_STR(table, key, item);
	if (item == NULL) {
		return -1;
	}
	return item->value;
}

//============================================================

Vector* get_global_slots () {
	static Vector* global_slots = NULL;
	if (global_slots == NULL)
		global_slots = make_vector();
	return global_slots;
}

Hashable* init_hashtable () {
	Hashable* table = NULL;
	Hashable* item_ptr = (Hashable*)malloc(sizeof(Hashable));
	item_ptr->key = "*&(&^&*";
	item_ptr->value = -1;
	HASH_ADD_KEYPTR(hh, table, item_ptr->key, 
		strlen(item_ptr->key), item_ptr);
	return table;
}

Hashable* get_global_var_map () {
	static Hashable* global_var_map = NULL;
	if (global_var_map == NULL) {
		global_var_map = init_hashtable();
	}
	return global_var_map;
}

// get instruction label table
Hashable* get_ins_label_table () {
	static Hashable* ins_label_table = NULL;
	if (ins_label_table == NULL) {
		ins_label_table = init_hashtable();
	}
	return ins_label_table;
}

Vector* get_operand_stack () {
	static Vector * operand_stack = NULL;
	if (operand_stack == NULL)
		operand_stack = make_vector();
	return operand_stack;
}

Frame* get_cur_frame () {
	static Frame* current_frame_ptr = NULL;
	if (current_frame_ptr == NULL)
		current_frame_ptr = create_frame(NULL, NULL, NULL);
	return current_frame_ptr;
}

//============================================================

Frame* create_frame (Vector* slot_vec_ptr, ByteIns* call_ins_ptr, Frame* call_frame_ptr) {
	Frame* ret = malloc(sizeof(Frame));
	ret->slot_vec_ptr = slot_vec_ptr;
	ret->call_ins_ptr = call_ins_ptr;
	ret->call_frame_ptr = (_Frame*)call_frame_ptr;
	return ret;
}

//============================================================
//=============== BYTE CODE INTERPRETER ======================
//============================================================

void exec_value (Value* v) {
	switch (v->tag) {
	case INT_VAL: {
		IntValue* v2 = (IntValue*)v;
		printf("Int(%d)", v2->value);
		break;
	}
	case NULL_VAL: {
		printf("Null");
		break;
	}
	case STRING_VAL: {
		StringValue* v2 = (StringValue*)v;
		printf("String(");
		print_string(v2->value);
		printf(")");
		break;
	}
	case METHOD_VAL: {
		MethodValue* v2 = (MethodValue*)v;
		printf("Method(#%d, nargs:%d, nlocals:%d) :", v2->name, v2->nargs, v2->nlocals);
		for (int i = 0; i < v2->code->size; i++) {
			printf("\n      ");
			print_ins(vector_get(v2->code, i));
		}
		break;
	}
	case SLOT_VAL: {
		SlotValue* v2 = (SlotValue*)v;
		printf("Slot(#%d)", v2->name);
		break;
	}
	case CLASS_VAL: {
		ClassValue* v2 = (ClassValue*)v;
		printf("Class(");
		for (int i = 0; i < v2->slots->size; i++) {
			if (i > 0) printf(", ");
			printf("#%d", (int)vector_get(v2->slots, i));
		}
		printf(")");
		break;
	}
	default:
		printf("Value with unknown tag: %d\n", v->tag);
		exit(-1);
	}
}

void exec_ins (ByteIns* ins) {
	switch (ins->tag) {
	case LABEL_OP: {
		LabelIns* i = (LabelIns*)ins;
		printf("label #%d", i->name);
		break;
	}
	case LIT_OP: {
		LitIns* i = (LitIns*)ins;
		printf("   lit #%d", i->idx);
		break;
	}
	case PRINTF_OP: {
		PrintfIns* i = (PrintfIns*)ins;
		printf("   printf #%d %d", i->format, i->arity);
		break;
	}
	case ARRAY_OP: {
		printf("   array");
		break;
	}
	case OBJECT_OP: {
		ObjectIns* i = (ObjectIns*)ins;
		printf("   object #%d", i->class);
		break;
	}
	case SLOT_OP: {
		SlotIns* i = (SlotIns*)ins;
		printf("   slot #%d", i->name);
		break;
	}
	case SET_SLOT_OP: {
		SetSlotIns* i = (SetSlotIns*)ins;
		printf("   set-slot #%d", i->name);
		break;
	}
	case CALL_SLOT_OP: {
		CallSlotIns* i = (CallSlotIns*)ins;
		printf("   call-slot #%d %d", i->name, i->arity);
		break;
	}
	case CALL_OP: {
		CallIns* i = (CallIns*)ins;
		printf("   call #%d %d", i->name, i->arity);
		break;
	}
	case SET_LOCAL_OP: {
		SetLocalIns* i = (SetLocalIns*)ins;
		printf("   set local %d", i->idx);
		break;
	}
	case GET_LOCAL_OP: {
		GetLocalIns* i = (GetLocalIns*)ins;
		printf("   get local %d", i->idx);
		break;
	}
	case SET_GLOBAL_OP: {
		SetGlobalIns* i = (SetGlobalIns*)ins;
		printf("   set global #%d", i->name);
		break;
	}
	case GET_GLOBAL_OP: {
		GetGlobalIns* i = (GetGlobalIns*)ins;
		printf("   get global #%d", i->name);
		break;
	}
	case BRANCH_OP: {
		BranchIns* i = (BranchIns*)ins;
		printf("   branch #%d", i->name);
		break;
	}
	case GOTO_OP: {
		GotoIns* i = (GotoIns*)ins;
		printf("   goto #%d", i->name);
		break;
	}
	case RETURN_OP: {
		printf("   return");
		break;
	}
	case DROP_OP: {
		printf("   drop");
		break;
	}
	default: {
		printf("Unknown instruction with tag: %u\n", ins->tag);
		exit(-1);
	}
	}
}

void exec_prog (Program* p) {
	printf("Constants :");
	Vector* vs = p->values;
	for (int i = 0; i < vs->size; i++) {
		printf("\n   #%d: ", i);
		exec_value(vector_get(vs, i));
	}
	printf("\nGlobals :");
	for (int i = 0; i < p->slots->size; i++)
		printf("\n   #%d", (int)vector_get(p->slots, i));
	printf("\nEntry : #%d", p->entry);
}


void interpret_bc (Program* p) {
	// printf("Interpreting Bytecode Program:\n");
	// print_prog(p);
	// printf("\n");
}

