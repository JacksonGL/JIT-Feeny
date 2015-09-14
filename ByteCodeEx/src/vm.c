#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include "utils.h"
#include "bytecode.h"
#include "uthash.h"
#include "vm.h"

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

Value* stack_pop ();
void stack_push (Value* val);
Vector* get_operand_stack ();

int get_entry_function ();
Vector* get_global_slots ();
Vector* get_constant_pool ();
Value* get_val_constant(int idx);
void set_global_slots (Vector* slots);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector* values, Value* v);
IndexedClassValue* create_class_value(Vector* values, ClassValue* v2);

// byte code interpreter operations
void exec_prog (Program* p);
void exec_ins (ByteIns* ins);

void exec_array_op ();
void exec_return_op ();
void exec_lit_op (LitIns* i);
void exec_slot_op (SlotIns* i);
void exec_goto_op (GotoIns* i);
void exec_label_op (LabelIns* i);
void exec_branch_op (BranchIns* i);
void exec_object_op (ObjectIns* i);
void exec_printf_op (PrintfIns* i);
void exec_set_slot_op (SetSlotIns* i);
void exec_set_local_op (SetLocalIns* i);
void exec_call_slot_op (CallSlotIns* i);
void exec_get_local_op (GetLocalIns* i);
void exec_set_global_op (SetGlobalIns* i);

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

// get value from the constant pool
Value* get_val_constant(int idx) {
	Vector* constant_pool = get_constant_pool();
	return vector_get(constant_pool, idx);
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

static Vector* operand_stack = NULL;
Vector* get_operand_stack () {
	if (operand_stack == NULL)
		operand_stack = make_vector();
	return operand_stack;
}

void stack_push (Value* val) {
	vector_add(operand_stack, val);
}
Value* stack_pop () {
	return vector_pop(operand_stack);
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

// Sets the i’th slot in the current
// local frame to the top value in
// the operand stack.
void exec_set_local_op (SetLocalIns* i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	// TBD
}

// Retrieves the i’th slot in the
// current local frame and pushes
// it onto the operand stack.
void exec_get_local_op (GetLocalIns* i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	// TBD
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_op (SetGlobalIns* i) {
	int name_idx = i->name;
	// TBD
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_op (GetGlobalIns* i) {
	int name_idx = i->name;
	// TBD
}

//  Pops a value from the operand
// stack. If this value is not Null, 
// then sets the instruction pointer to
// the instruction address associated 
// with the name given by the String
// object at index i.
void exec_branch_op (BranchIns* i) {
	int label_idx = i->name;
	// TBD
}

// Sets the instruction pointer to the instruction
// address associated with the name given by 
// the String object at index i.
void exec_goto_op (GotoIns* i) {
	int label_idx = i->name;
	// TBD
}

// Registers the parent frame of the
// current local frame as the new current 
// frame. Execution proceeds by
// setting the instruction pointer to the 
// return address stored in the current
// local frame. The local frame is no longer 
// used after a Return instruction, and any 
// storage allocated for it may be reclaimed.
void exec_return_op () {
	// TBD
}

// Pops and discards the top value from
// the operand stack.
void exec_drop_op () {
	// TBD
}

// Retrieves the object at index i in
// the constant pool, which refers to
// either an Int object or a Null object,
// and pushes it onto the operand stack.
void exec_lit_op (LitIns* i) {
	int val_idx = i->idx;
	Value* val = get_val_constant(val_idx);
	if (val->tag != INT_OBJ && val->tag != NULL_OBJ) {
		printf("Error in LIT_OP.\n");
		exit(-1);
	}
	stack_push(val);
}

// First pops the initializing value from
// the operand stack, and then pops the
// length of the array from the operand
// stack. Creates a new array with the
// given length, with each element initizlied
// to the given value, and pushes the array
// onto the operand stack.
void exec_array_op () {
	// TBD
}

// Pops n values from the operand
// stack, and then prints them out according to the given format string.
// Arguments are spliced in from the deepest value in the stack (last
// popped) to the shallowest value in the stack (first popped). Null is
// then pushed onto the operand stack.
void exec_printf_op (PrintfIns* i) {
	int format_idx = i->format;
	int arity = i->arity;
	// TBD
}

// Associates a name with the address of
// this instruction. The name is given by
// the String object at index i.
void exec_label_op (LabelIns* i) {
	int label_idx = i->name;
	// TBD
}

// Retrieves the Class object at index c.
// Suppose that the class object contains n Slot objects, and m Method
// objects. This instruction will pop n values from the operand stack for
// use as the initial values of the variable slots in the object, and then an
// additional value for use as the parent of the object. The first variable
// slot is initialized to the deepest value on the stack (last popped) and the
// last variable slot is initialized to the shallowest value on the stack (first
// popped). A new object is created with the variable slots indicated by
// the Class object, initialized to the given values on the stack, with the
// method slots indicated by the Class object, and with the given parent
// object, and is pushed onto the operand stack.
void exec_object_op (ObjectIns* i) {
	int class_idx = i->class;
	// TBD
}

// Pops a value from the operand
// stack assuming it is an object. Retrieves the value in the object stored
// at the variable slot with name given by the String object at index i,
// and pushes it onto the operand stack.
void exec_slot_op (SlotIns* i) {
	int slot_idx = i->name;
	// TBD
}

// Pops the value to store, x, from
// the operand stack, and then pops the object to store it into. Stores
// x into the object at the variable slot with name given by the String
// object at index i. x is then pushed onto the operand stack.
void exec_set_slot_op (SetSlotIns* i) {
	int slot_idx = i->name;
	// TBD
}

// Pops n values from the
// operand stack for the arguments to the call. Then pops the receiver
// object from the operand stack. The name of the method to call is given
// by the String object at index i. If the receiver is an integer or array,
// then the result of the method call (as specified by the semantics of
// Feeny) is pushed onto the operand stack. If the receiver is an object,
// then a new local frame is created for the context of the call. Slot 0
// in the new local frame holds the receiver object, and the following n
// slots holds the argument values starting with the deepest value on the
// stack (last popped) and ending with the shallowest value on the stack
// (first popped). The new local frame has the current frame as its parent,
// and the current instruction pointer as the return address. Execution
// proceeds by registering the newly created frame as the current frame,
// and setting the instruction pointer to the address of the body of the
// method.
void exec_call_slot_op (CallSlotIns* i) {
	int method_name_idx = i->name;
	int arity = i->arity;
	// TBD
}

// Pops n values from the operand stack
// for the arguments to the call. The name of the function to call is given
// by the String object at index i. A new local frame is created for the context
// of the call. The first n slots in the frame holds the argument values
// starting with the deepest value on the stack (last popped) and ending
// with the shallowest value on the stack (first popped). The new local
// frame has the current frame as its parent, and the current instruction
// pointer as the return address. Execution proceeds by registering the
// newly created frame as the current frame, and setting the instruction
// pointer to the address of the body of the function.
void exec_call_op (CallIns* i) {
	int function_name_idx = i->name;
	int arity = i->arity;
	// TBD
}

void exec_ins (ByteIns* ins) {
	switch (ins->tag) {
	case LABEL_OP: {
		LabelIns* i = (LabelIns*)ins;
		exec_label_op(i);
		break;
	}
	case LIT_OP: {
		LitIns* i = (LitIns*)ins;
		exec_lit_op(i);
		break;
	}
	case PRINTF_OP: {
		PrintfIns* i = (PrintfIns*)ins;
		exec_printf_op(i);
		break;
	}
	case ARRAY_OP: {
		exec_array_op();
		break;
	}
	case OBJECT_OP: {
		ObjectIns* i = (ObjectIns*)ins;
		exec_object_op(i);
		break;
	}
	case SLOT_OP: {
		SlotIns* i = (SlotIns*)ins;
		exec_slot_op(i);
		break;
	}
	case SET_SLOT_OP: {
		SetSlotIns* i = (SetSlotIns*)ins;
		exec_set_slot_op(i);
		break;
	}
	case CALL_SLOT_OP: {
		CallSlotIns* i = (CallSlotIns*)ins;
		exec_call_slot_op(i);
		break;
	}
	case CALL_OP: {
		CallIns* i = (CallIns*)ins;
		exec_call_op(i);
		break;
	}
	case SET_LOCAL_OP: {
		SetLocalIns* i = (SetLocalIns*)ins;
		exec_set_local_op(i);
		break;
	}
	case GET_LOCAL_OP: {
		GetLocalIns* i = (GetLocalIns*)ins;
		exec_get_local_op(i);
		break;
	}
	case SET_GLOBAL_OP: {
		SetGlobalIns* i = (SetGlobalIns*)ins;
		exec_set_global_op(i);
		break;
	}
	case GET_GLOBAL_OP: {
		GetGlobalIns* i = (GetGlobalIns*)ins;
		exec_get_global_op(i);
		break;
	}
	case BRANCH_OP: {
		BranchIns* i = (BranchIns*)ins;
		exec_branch_op(i);
		break;
	}
	case GOTO_OP: {
		GotoIns* i = (GotoIns*)ins;
		exec_goto_op(i);
		break;
	}
	case RETURN_OP: {
		exec_return_op();
		break;
	}
	case DROP_OP: {
		exec_drop_op();
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
		if (value->tag != SLOT_OBJ && value->tag != METHOD_OBJ) {
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

// add values in the byte code syntax tree into
// the runtime constatnt pool
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
	/*printf("Interpreting Bytecode Program:\n");
	print_prog(p);
	printf("\n");*/
	exec_prog(p);
}

