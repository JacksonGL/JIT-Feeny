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


typedef int ValIndex;

typedef struct {
	const char *key;	// hashtable item key
	int value;		    // hashtable item value
	UT_hash_handle hh;	// used for internal book keeping, ignore it
} Hashable;

typedef enum {
	INT_OBJ,
	NULL_OBJ,
	STR_OBJ,
	METHOD_OBJ,
	SLOT_OBJ,
	CLASS_OBJ,
	OBJ_OBJ
} ObjTag;

/* reuse the value structure from bytecode.h

typedef struct {
  ValTag tag;
} Value;

typedef struct {
  ValTag tag;
  int value;
} IntValue;

typedef struct {
  ValTag tag;
  char* value;
} StringValue;

typedef struct {
  ValTag tag;
  int name;
  int nargs;
  int nlocals;
  Vector* code;
} MethodValue;

typedef struct {
  ValTag tag;
  int name;
} SlotValue;

typedef struct {
  ValTag tag;
  Vector* slots;
} ClassValue;
*/

typedef struct {
	ValTag tag;
	Vector* slots;
	// a hash table that maps a name to index;
	Hashable* name_to_slot_table;
} IndexedClassValue;

typedef struct {
	ObjTag tag;
	Vector* slot_vec_ptr;
	Value* parent_obj_ptr;
	ClassValue* class_ptr;
} ObjectValue;

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

// hash table operations
ValIndex find_item (Hashable* table, char* key);
void add_item (Hashable* table, char* key, ValIndex value);

// global data structure operations
int get_entry_function ();
Vector* get_global_slots ();
Vector* get_constant_pool ();
void set_global_slots (Vector* slots);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector* values, Value* v);
IndexedClassValue* create_class_value(Vector* values, ClassValue* v2);

// byte code interpreter operations
void exec_prog (Program* p);
void exec_ins (ByteIns* ins);

Frame* create_frame (Vector* slot_vec_ptr, ByteIns* call_ins_ptr, Frame* call_frame_ptr);


//============================================================
//============== HASHTABLE ENCAPSULATION =====================
//============================================================

Hashable* init_hashtable () {
	Hashable* table = NULL;
	Hashable* item_ptr = (Hashable*)malloc(sizeof(Hashable));
	item_ptr->key = "*&(&^&*";
	item_ptr->value = -1;
	HASH_ADD_KEYPTR(hh, table, item_ptr->key,
	                strlen(item_ptr->key), item_ptr);
	return table;
}

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
//========== GLOBAL DATA STRUCTURE OPERATIONS ================
//============================================================

Vector* get_constant_pool () {
	static Vector* constant_pool = NULL;
	if (constant_pool == NULL) {
		constant_pool = make_vector();
	}
	return constant_pool;
}

static Vector* global_slots = NULL;
void set_global_slots (Vector* slots) {
	global_slots = slots;
}
Vector* get_global_slots () {
	return global_slots;
}

static int entry_function_index = -1;
int get_entry_function () {
	return entry_function_index;
}
void set_entry_function (int entry_index) {
	entry_function_index = entry_index;
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
	static Vector* operand_stack = NULL;
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
//=============== BYTE CODE INTERPRETER ======================
//============================================================

Frame* create_frame (Vector* slot_vec_ptr, ByteIns* call_ins_ptr, Frame* call_frame_ptr) {
	Frame* ret = malloc(sizeof(Frame));
	ret->slot_vec_ptr = slot_vec_ptr;
	ret->call_ins_ptr = call_ins_ptr;
	ret->call_frame_ptr = (_Frame*)call_frame_ptr;
	return ret;
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

IndexedClassValue* create_class_value(Vector* values, ClassValue* v2) {
	IndexedClassValue* new_v =
	    (IndexedClassValue*)malloc(sizeof(IndexedClassValue));
	new_v->tag = v2->tag;
	new_v->slots = v2->slots;
	new_v->name_to_slot_table = init_hashtable();
	// iterate over all slots
	for (int i = 0; i < v2->slots->size; i++) {
		// get the slot value
		int slot_index = (int)vector_get(v2->slots, i);
		Value* value = vector_get(values, slot_index);
		if (value->tag != SLOT_OBJ && value->tag != SLOT_OBJ) {
			printf("non-slot or method value in class\n");
			exit(-1);
		}
		int name_index = ((SlotValue*)value)->name;
		// get the string value of name
		value = vector_get(values, name_index);
		if (value->tag != STR_OBJ) {
			printf("Slot value name is not a string.\n");
			exit(-1);
		}
		char* name_ptr = ((StringValue*)value)->value;
		// map the string name to the slot index in new_v->slots
		add_item(new_v->name_to_slot_table, name_ptr, i);
	}
	return new_v;
}

void addto_constant_pool (Vector* values, Value* v) {
	Vector* constant_pool = get_constant_pool();
	switch (v->tag) {
	case INT_VAL:
	case NULL_VAL:
	case STRING_VAL:
	case METHOD_VAL:
	case SLOT_VAL: {
		vector_add(constant_pool, v);
		break;
	}
	case CLASS_VAL: {
		ClassValue* v2 = (ClassValue*)v;
		IndexedClassValue* new_v = create_class_value(values, v2);
		vector_add(constant_pool, new_v);
		break;
	}
	default:
		printf("Value with unknown tag: %d\n", v->tag);
		exit(-1);
	}
}

void exec_prog (Program* p) {
	// constant pool
	Vector* vs = p->values;
	for (int i = 0; i < vs->size; i++) {
		addto_constant_pool(vs, vector_get(vs, i));
	}
	// global slot
	set_global_slots(p->slots);
	// entry function
	set_entry_function(p->entry);

	// TODO: call the entry function
}

void interpret_bc (Program* p) {
	// printf("Interpreting Bytecode Program:\n");
	// print_prog(p);
	// printf("\n");
	exec_prog(p);
}

