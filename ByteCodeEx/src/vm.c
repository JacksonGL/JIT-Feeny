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

typedef struct {
	ObjTag tag;
} RValue;

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
} IdxClassValue;

typedef struct {
	ValTag tag;
	int name;
	Value* value;
} RSlotValue;

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

// operand stack operations
Value* stack_pop ();
Value* stack_peek ();
void stack_push (Value* val);
Vector* get_operand_stack ();

int get_entry_function ();
Vector* get_global_slots ();
Vector* get_constant_pool ();
Value* get_val_constant(int idx);
void set_global_slots (Vector* slots);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector* values, Value* v);
IdxClassValue* create_class (Vector* values, ClassValue* v2);

// frame operations
Frame* get_cur_frame ();
Frame* get_root_frame ();
Value* get_frame_slot (Frame* frame, int idx);
void set_frame_slot (Frame* frame, int idx, Value* val);

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

RSlotValue* copy_var_slot (SlotValue* slot);
int get_num_var_slots (ClassValue* class_val);

Frame* create_frame (Vector* slots, ByteIns* call_ins, Frame* call_frame);

// util operations
char* intToString(int i);
char* toString(Value *val_ptr);
char* copy_string (const char *string);
char* str_replace (char *orig, char *rep, char *with);


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

// count the number of slots in the class
// that are variable slots
int get_num_var_slots (ClassValue* class_val) {
	int var_slot_count = 0;
	for (int i = 0; i < class_val->slots->size; i++) {
		Value* slot = vector_get(class_val->slots, i);
		if (slot->tag == SLOT_VAL) {
			var_slot_count++;
		}
	}
	return var_slot_count;
}

RSlotValue* copy_var_slot (SlotValue* slot) {
	RSlotValue* rSlot =
	    (RSlotValue*)malloc(sizeof(RSlotValue));
	rSlot->tag = slot->tag;
	rSlot->name = slot->name;
	rSlot->value = NULL;
	return rSlot;
}

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

// returns the string content
// indexed by idx in the constant pool
char* get_str_constant(int idx) {
	Value* val = get_val_constant(idx);
	if (val == NULL || val->tag != STR_OBJ) {
		printf("Error: get string constant.\n");
		exit(-1);
	}
	return ((StringValue*)val)->value;
}

// instructor pointer
static int inst_ptr = -1;
// hashtable that associate a string label
// with an instruction address
static Hashable* inst_label_table = NULL;
Hashable* get_inst_label_table () {
	if (inst_label_table == NULL)
		inst_label_table = init_hashtable();
	return inst_label_table;
}
void set_label_addr (char* label, int addr) {
	add_item(get_inst_label_table(), label, addr);
}
int get_label_addr (char* label) {
	return find_item(get_inst_label_table(), label);
}

// hashtable that maps string names to global
// slots' index in the constant pool
// each index is guaranteed to refer to either
// a Slot object or a Method object
static Hashable* global_slot_table = NULL;
static Vector* global_slots = NULL;
void set_global_slots (Vector* slots) {
	int val_idx, name_idx;
	Value* value = NULL;
	char* name_str = NULL;
	global_slots = slots;
	// establish string name to
	// slot index hashtable
	global_slot_table = init_hashtable();
	for (int i = 0; i < slots->size; i++) {
		val_idx = (int)vector_get(slots, i);
		value = get_val_constant(val_idx);
		name_idx = ((SlotValue*)value)->name;
		name_str = get_str_constant(name_idx);
		add_item(global_slot_table, name_str, val_idx);
	}
}
Vector* get_global_slots () {
	return global_slots;
}
Value* get_global_slot_by_name (char* name) {
	int slot_idx = find_item(global_slot_table, name);
	if (slot_idx < 0) {
		printf("Error: get global slot by name.\n");
		exit(-1);
	}
	Value* val = get_val_constant(slot_idx);
	if (val == NULL) {
		printf("Error[2]: get global slot by name.\n");
		exit(-1);
	}
	return val;
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
Value* stack_peek () {
	return vector_peek(operand_stack);
}

Frame* get_root_frame () {
	static Frame* root_frame_ptr = NULL;
	if (root_frame_ptr == NULL)
		root_frame_ptr = create_frame(NULL, NULL, NULL);
	return root_frame_ptr;
}

Frame* get_cur_frame () {
	static Frame* current_frame_ptr = NULL;
	return current_frame_ptr;
}

void set_frame_slot (Frame* frame, int idx, Value* val) {
	if (idx >= frame->slot_vec_ptr->size) {
		printf("Error set frame value.\n");
		exit(-1);
	}
	vector_set(frame->slot_vec_ptr, idx, val);
}

Value* get_frame_slot (Frame* frame, int idx) {
	if (idx >= frame->slot_vec_ptr->size) {
		printf("Error get frame value.\n");
		exit(-1);
	}
	return vector_get(frame->slot_vec_ptr, idx);
}

Frame* create_frame (Vector* slot_vec,
                     ByteIns* call_ins, Frame* call_frame) {
	Frame* ret = malloc(sizeof(Frame));
	ret->slot_vec_ptr = slot_vec;
	ret->call_ins_ptr = call_ins;
	ret->call_frame_ptr = (_Frame*)call_frame;
	return ret;
}

//============================================================
//=============== BYTE CODE INTERPRETER ======================
//============================================================

// Sets the i’th slot in the current
// local frame to the top value in
// the operand stack.
void exec_set_local_op (SetLocalIns* i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	Value* val = stack_peek();
	if (val == NULL) {
		printf("Error: set local op.\n");
		exit(-1);
	}
	Frame* frame = get_cur_frame();
	set_frame_slot(frame, slot_idx, val);
	// NOTE: do not pop from the stack
}

// Retrieves the i’th slot in the
// current local frame and pushes
// it onto the operand stack.
void exec_get_local_op (GetLocalIns* i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	Frame* frame = get_cur_frame();
	if (frame == NULL) {
		printf("Error: get local op.\n");
		exit(-1);
	}
	Value* val = get_frame_slot(frame, slot_idx);
	stack_push(val);
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_op (SetGlobalIns* i) {
	int name_idx = i->name;
	char* name_str = get_str_constant(name_idx);
	Value* val = stack_peek();
	if (val == NULL) {
		printf("Error: set global op.\n");
		exit(-1);
	}
	Value* slot = get_global_slot_by_name(name_str);
	if (slot == NULL || slot->tag != SLOT_OBJ) {
		printf("Error[2]: set global op.\n");
		exit(-1);
	}
	RSlotValue* rSlot = (RSlotValue*)slot;
	rSlot->value = val;
	// NOTE: do not pop from the stack
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_op (GetGlobalIns* i) {
	int name_idx = i->name;
	char* name_str = get_str_constant(name_idx);
	Value* slot = get_global_slot_by_name(name_str);
	if (slot == NULL || slot->tag != SLOT_OBJ) {
		printf("Error: get global op.\n");
		exit(-1);
	}
	RSlotValue* rSlot = (RSlotValue*)slot;
	Value* val = rSlot->value;
	if (val == NULL) {
		printf("Error[2]: get global op.\n");
		exit(-1);
	}
	stack_push(val);
}

// Pops a value from the operand
// stack. If this value is not Null,
// then sets the instruction pointer to
// the instruction address associated
// with the name given by the String
// object at index i.
void exec_branch_op (BranchIns* i) {
	Value* val = stack_pop();
	if (val == NULL) {
		printf("Error: branch op.\n");
		exit(-1);
	}
	if (val->tag != NULL_OBJ) {
		int label_idx = i->name;
		char* label_str = get_str_constant(label_idx);
		int addr = get_label_addr(label_str);
		inst_ptr = addr;
	}
}

// Sets the instruction pointer to the instruction
// address associated with the name given by
// the String object at index i.
void exec_goto_op (GotoIns* i) {
	int label_idx = i->name;
	char* label_str = get_str_constant(label_idx);
	int addr = get_label_addr(label_str);
	inst_ptr = addr;
}

// Pops and discards the top value from
// the operand stack.
void exec_drop_op () {
	stack_pop();
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

// Associates a name with the address of
// this instruction. The name is given by
// the String object at index i.
void exec_label_op (LabelIns* i) {
	int label_idx = i->name;
	char* label_str = get_str_constant(label_idx);
	set_label_addr(label_str, inst_ptr);
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
	char* format_str = get_str_constant(format_idx);
	char replace_char[] = "~";
	// replace all ~ in format string into the corresponding arguments
	char *new_str = copy_string(format_str);

	Vector* args = make_vector();
	int n = arity;
	while (n-- > 0) {
		vector_add(args, stack_pop());
	}
	if (args->size != arity) {
		printf("Error: exec_printf_op.\n");
		exit(-1);
	}
	n = arity;
	while (n-- > 0) {
		char *old_str = new_str;
		// pop one elemet from the stack
		Value* val = vector_pop(args);
		char *arg_str = toString(val);
		new_str = str_replace(old_str, replace_char, arg_str);
		if (arg_str != NULL) free(arg_str);
		if (old_str != NULL) free(old_str);
	}
	if (new_str != NULL) {
		printf("%s", new_str);
		free(new_str);
	}
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
	Value* class = get_val_constant(class_idx);
	if (class->tag != CLASS_VAL) {
		printf("Error: exec_object_op.\n");
		exit(-1);
	}
	ClassValue* class_val = (ClassValue*)class;
	int num_slots = get_num_var_slots(class_val);
	// init new object value
	ObjectValue* obj = (ObjectValue*)malloc(sizeof(ObjectValue));
	obj->tag = OBJ_OBJ;
	obj->slot_vec_ptr = make_vector();
	vector_set_length(obj->slot_vec_ptr, class_val->slots->size, NULL);
	// iterate from last slot to the first slot
	for (int i = class_val->slots->size - 1; i >= 0 ; i--) {
		Value* slot_in_class = vector_get(class_val->slots, i);
		if (slot_in_class->tag == METHOD_VAL) {
			// copy method slot
			vector_set(obj->slot_vec_ptr, i, slot_in_class);
		} else if (slot_in_class->tag == SLOT_VAL) {
			// copy var slot
			RSlotValue* new_slot =
			    copy_var_slot((SlotValue*)slot_in_class);
			// first var slot will be initialized
			// to the deepest value on the stack
			new_slot->value = stack_pop();
			if (new_slot->value == NULL) {
				printf("Error[2]: exec_object_op.\n");
				exit(-1);
			}
		} else {
			printf("Error[3]: exec_object_op.\n");
			exit(-1);
		}
	}
	obj->parent_obj_ptr = stack_pop();
	obj->class_ptr = class_val;
	stack_push((Value*)obj);
}

// Pops a value from the operand
// stack assuming it is an object. Retrieves the value in the object stored
// at the variable slot with name given by the String object at index i,
// and pushes it onto the operand stack.
void exec_slot_op (SlotIns* i) {
	int slot_idx = i->name;
	printf("exec_slot_op is not implemented.\n");
	exit(-1);
	// TBD
}

// Pops the value to store, x, from
// the operand stack, and then pops the object to store it into. Stores
// x into the object at the variable slot with name given by the String
// object at index i. x is then pushed onto the operand stack.
void exec_set_slot_op (SetSlotIns* i) {
	int slot_idx = i->name;
	printf("exec_set_slot_op is not implemented.\n");
	exit(-1);
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
	printf("exec_call_op is not implemented.\n");
	exit(-1);
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
	printf("exec_return_op is not implemented.\n");
	exit(-1);
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

	printf("exec_call_slot_op is not implemented.\n");
	exit(-1);
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

IdxClassValue* create_class(Vector* values, ClassValue* v2) {
	IdxClassValue* new_v =
	    (IdxClassValue*)malloc(sizeof(IdxClassValue));
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
	case METHOD_VAL: {
		vector_add(constant_pool, v);
		break;
	}
	case SLOT_VAL: {
		SlotValue* v2 = (SlotValue*)v;
		RSlotValue* new_v = (RSlotValue*)malloc(sizeof(RSlotValue));
		new_v->tag = v2->tag;
		new_v->name = v2->name;
		new_v->value = NULL;
		vector_add(constant_pool, new_v);
		break;
	}
	case CLASS_VAL: {
		ClassValue* v2 = (ClassValue*)v;
		IdxClassValue* new_v = create_class(values, v2);
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

//============================================================
//================== UTIL FUNCITONS ==========================
//============================================================

// convert an interpreter object into a string
char* toString(Value *val_ptr) {
	switch (((RValue*)val_ptr)->tag) {
	case INT_OBJ: {
		return intToString(((IntValue*)val_ptr)->value);
	}
	case OBJ_OBJ: {
		char *result = malloc(9 * sizeof(char));
		strcpy(result, "[OBJECT]");
		return result;
	}
	case NULL_OBJ: {
		char *result = malloc(5 * sizeof(char));
		strcpy(result, "Null");
		return result;
	}
	/*
	case ARRAY_OBJ: {
	  return arrayToString((ArrayObj*)obj_ptr);
	}
	*/
	default:
		printf("Error: toString.\n");
		exit(-1);
	}
	return NULL;
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

char* copy_string (const char *string) {
	char *stringcopy = malloc (1 + strlen (string));
	if (stringcopy) {
		strcpy (stringcopy, string);
	} else {
		fprintf (stderr, "malloc failure!");
	}
	return stringcopy;
}

// You must free the result if result is non-NULL.
// only replace the first occurrance of *rep
char* str_replace (char *orig, char *rep, char *with) {
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

// TODO: update instruction pointer