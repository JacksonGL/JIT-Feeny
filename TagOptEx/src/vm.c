#include <sys/timeb.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include "utils.h"
#include "bytecode.h"
#include "vm.h"

#define TAG_PTR_TO_INT( arg ) ( (arg >> 3) )
#define INT_TO_TAG_PTR( arg ) ( ((intptr_t)( (arg << 3) | 0x0000000000000001)) )

typedef struct {
	int size;
	int capacity;
	intptr_t* array;
} Vector2;

typedef enum {
	GET_GLOBAL_OP_QUICK = DROP_OP + 1,
	SET_GLOBAL_OP_QUICK,
	GOTO_OP_QUICK,
	BRANCH_OP_QUICK,
	CALL_OP_QUICK,
} QOpCode;

typedef struct {
	OpCode tag;
	int idx;
} GetGlobalQuickIns;

typedef struct {
	OpCode tag;
	int idx;
} SetGlobalQuickIns;

typedef struct {
	OpCode tag;
	int addr;
} GotoQuickIns;

typedef struct {
	OpCode tag;
	int addr;
} BranchQuickIns;

typedef struct {
	OpCode tag;
	int idx;
	int arity;
} CallQuickIns;

// #define DEBUG
#ifdef DEBUG
#include <execinfo.h>
#endif //DEBUG

// Copied from sys/time.h
// to not pollute our #define space
// WAS NOT WRITTEN BY LG OR BM - rights beintptr_t to original author
#define timersub(a, b, result)                  \
  do {                        \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;            \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;            \
    if ((result)->tv_usec < 0) {                \
      --(result)->tv_sec;                  \
      (result)->tv_usec += 1000000;                \
    }                        \
  } while (0)


//============================================================
//=============== BASIC DATA STRUCTURE =======================
//============================================================
void debugf(const char * format, ...);

void error(const char* msg) {
	printf("%s", msg);
#ifdef DEBUG
	void *array[10];
	size_t size = backtrace (array, sizeof(array) / sizeof(void*));
	char **strings = backtrace_symbols (array, size);

	debugf ("Obtained %zd stack frames.\n", size);

	for (size_t i = 0; i < size; i++)
		debugf ("%s\n", strings[i]);

	free (strings);
#endif
	exit(-1);
}

typedef enum {
	INT_TAG,
	OBJ_TAG,
	NULL_TAG
} PtrTag;

typedef enum {
	INT_OBJ,
	NULL_OBJ,
	STR_OBJ,
	METHOD_OBJ,
	SLOT_OBJ,
	CLASS_OBJ,
	ARRAY_OBJ,
	BROKEN_HEART,
	OBJ_OBJ,
} ObjTag;

typedef struct { //can go on heap
	intptr_t tag;
	intptr_t space;
} NullIValue;

/* reuse the value structure from bytecode.h */

typedef struct {
	intptr_t tag;
	intptr_t space;
} IValue;

typedef struct { //can go on heap
	intptr_t tag;
	intptr_t value;
} IntIValue;

typedef struct {
	intptr_t tag;
	char* value;
} StringIValue;

typedef struct {
	intptr_t tag;
	Vector2* code;
	int name;
	int nargs;
	int nlocals;
} MethodIValue;

typedef struct { //must go on heap
	intptr_t tag;
	intptr_t length;
	// IValue* slots[0];
	intptr_t slots[0];
} ArrayIValue;

typedef struct {
	intptr_t tag;
	// a vector of slot indices
	Vector2* slots;
	//linear search - fine when objects are small
	Vector2* name_to_slot_vec;
} ClassIValue;

typedef struct { //must go on heap?
	intptr_t tag;
	intptr_t value;
	int name;
} SlotIValue;

typedef struct { // must go on heap
	ClassIValue* class_ptr;
	IValue* parent_obj_ptr;
	SlotIValue* var_slots[0];
} ObjectIValue;

typedef struct Frame Frame; //forward declaration
struct Frame {
	// the values of all local variables
	// defined in the function
	Vector2* slot_vec_ptr;
	// address of the call byte
	// code instruction, this is
	// also the return address
	int call_ins_idx;
	// function slot pointer
	MethodIValue* func_ptr;
	// caller's frame
	Frame* call_frame_ptr;
};

void debugf(const char * format, ...) {
#ifdef DEBUG
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
#endif // DEBUG
}

// global data structure operations

// operand stack operations
intptr_t stack_pop ();
intptr_t stack_peek ();
Vector2* get_operand_stack ();
void stack_push (intptr_t val);

int get_entry_function ();
Vector2* get_global_slots ();
Vector2* get_constant_pool ();
intptr_t get_val_constant(int idx);
void set_global_slots (Vector2* slots);
void associate_labels(MethodIValue *val);
void set_val_constant(int idx, IValue* t);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector2* values, Value* v);
ClassIValue* create_class (Vector2* values, ClassValue* v2);

// frame operations
Frame* get_cur_frame ();
Frame* get_root_frame ();
void set_cur_frame (Frame* new_frame);
intptr_t get_frame_slot (Frame* frame, int idx);
void set_frame_slot (Frame* frame, int idx, intptr_t val);

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
void process_label_op (LabelIns * i, int i_ptr);
void call_func (MethodIValue * function_slot, int arity);

// slot related operations
SlotIValue* copy_var_slot (SlotIValue* slot);
int get_num_var_slots (ClassIValue* class_val);
IValue* find_slot_by_name(ObjectIValue* receiver, char* function_name);
IValue* get_global_slot_by_idx (int slot_idx);

Frame* create_frame (int call_ins_idx,
                     MethodIValue* func_ptr, Frame* call_frame);

// util operations
char* intToString (int i);
char* toString (intptr_t val_ptr);
char* copy_string (const char *string);
char* arrayToString (ArrayIValue *obj_ptr);
char* str_replace (char *orig, char *rep, char *with);


intptr_t make_null_obj ();
ObjTag obj_type (IValue* o);
intptr_t make_int_obj (int value);
PtrTag ptr_tag_intptr_t (intptr_t o);
ObjTag obj_type_intptr_t (intptr_t o);
intptr_t array_length (ArrayIValue* array);
intptr_t array_get (ArrayIValue* a, intptr_t i_ptr);
ArrayIValue* make_array_obj (int length, intptr_t init);
intptr_t array_set (ArrayIValue* a, intptr_t i_ptr, intptr_t v_ptr);

intptr_t eq (intptr_t x, intptr_t y);
intptr_t lt (intptr_t x, intptr_t y);
intptr_t le (intptr_t x, intptr_t y);
intptr_t gt (intptr_t x, intptr_t y);
intptr_t ge (intptr_t x, intptr_t y);
intptr_t int_obj_add (intptr_t x, intptr_t y);
intptr_t int_obj_sub (intptr_t x, intptr_t y);
intptr_t int_obj_mul (intptr_t x, intptr_t y);
intptr_t int_obj_div (intptr_t x, intptr_t y);
intptr_t int_obj_mod (intptr_t x, intptr_t y);

// assert functions
void assert_obj_obj (IValue* ptr);
void assert_not_null (void* ptr);
IntIValue* to_int_val (IValue* val);
SlotIValue* to_slot_val (IValue* val);
MethodIValue* to_function_val (IValue* val);
void exp_assert(int i, const char * fmt, ...);

// free functions
void free_frame(Frame* frame);

// vector that work with arbitrary pointer values
Vector2* make_vector_2 ();
void vector_free_2 (Vector2* v);
void vector_clear_2 (Vector2* v);
intptr_t vector_pop_2 (Vector2* v);
intptr_t vector_peek_2 (Vector2* v);
intptr_t vector_get_2 (Vector2* v, int i);
void vector_add_2 (Vector2* v, intptr_t val);
void vector_set_2 (Vector2* v, int i, intptr_t x);
void vector_ensure_capacity_2 (Vector2* v, int c);
void vector_set_length_2 (Vector2* v, int len, intptr_t x);

//============================================================
//========== GLOBAL DATA STRUCTURE OPERATIONS ================
//============================================================

typedef struct {
	char * name;
	int val;
} NamePair;

int find_item(Vector2* pairs, char* name) {
	for (int i = 0; i < pairs->size; ++i) {
		NamePair* np = vector_get_2(pairs, i);
		if (np->name == name || !strcmp(np->name, name)) {
			return np->val;
		}
	}
	return -1;
}

void add_item(Vector2* pairs, char*name, int idx) {
	NamePair* np = malloc(sizeof(NamePair));
	np->name = name;
	np->val = idx;
	vector_add_2(pairs, np);
}

IValue* find_slot_by_name(ObjectIValue* obj, char* function_name) {
	struct timeval start, end;
	// start collecting time

	IValue* ret = NULL;
	int ret_val_idx = find_item(obj->class_ptr->name_to_slot_vec,
	                            function_name);
	if (ret_val_idx != -1) {
		if (ret_val_idx >= get_num_var_slots(obj->class_ptr)) { // this is a method slot
			int slot_idx = (int)vector_get_2(obj->class_ptr->slots, ret_val_idx);
			return get_val_constant(slot_idx);
		}
		ret = (IValue*)obj->var_slots[ret_val_idx];
	} else {
		// search in the parent object
		// assumes that in Feeny, object can only inherit from
		// user defined object or null object
		IValue* parent = obj->parent_obj_ptr;
		if (obj_type_intptr_t(parent) == OBJ_OBJ) {
			ret = find_slot_by_name((ObjectIValue*)parent,
			                        function_name);
		} else {
			if (obj_type_intptr_t(parent) != NULL_OBJ) {
				// debugf("Has addr %x\n", obj);
				// debugf("Has addr %x\n", parent);
				// debugf("Has type %d\n", obj_type_intptr_t(parent));
				error("Error[2]: find_slot_by_name.\n");
			}
			ret = NULL;
		}
	}

	// end collecting time
	return ret;
}

// count the number of slots in the class
// that are variable slots
int get_num_var_slots (ClassIValue * class_val) {
	int var_slot_count = 0;
	for (int i = 0; i < class_val->slots->size; i++) {
		int slot_idx = (int)vector_get_2(class_val->slots, i);
		IValue* slot = get_val_constant(slot_idx);
		assert_not_null(slot);
		if (obj_type_intptr_t(slot) == SLOT_VAL) { //TODO: fix item
			var_slot_count++;
		}
	}
	return var_slot_count;
}

void* garbage_collector ();
void scan_root_set ();

#define HEAP_SIZE 100*1024*1024
char heap1[HEAP_SIZE];
char heap2[HEAP_SIZE];
char (*heap_cur)[HEAP_SIZE] = &heap1;
char (*heap_old)[HEAP_SIZE] = &heap2;
int next_free_cur = 0;
int next_free_old = 0;
int is_currently_collecting = 0;

void * _halloc(size_t size) {
	char* heap = *heap_cur;
	void * ret = &heap[next_free_cur];
	next_free_cur += size;
	if (next_free_cur >= HEAP_SIZE) {
		if (is_currently_collecting) {
			error("Ran out of heap while collecting garbage!\n");
		}
		garbage_collector();
		// redo the allocation
		return _halloc(size);
	}
	debugf("Allocating %x for size %d\n", ret, size);
	return ret;
}

size_t halloc_bytes = 0;
void * halloc(size_t size) {
	halloc_bytes += size;
	// garbage_collector();
	return _halloc(size);
}

void swap_heaps() {
	char (*heap_tmp)[HEAP_SIZE] = heap_cur;
	debugf("heap_cur %x, heap_old %x\n", heap_cur, heap_old);
	heap_cur = heap_old;
	heap_old = heap_tmp;
	debugf("heap_cur %x, heap_old %x\n", heap_cur, heap_old);

	next_free_old = next_free_cur;
	next_free_cur = 0; // current heap is empty
}

void scan_new_heap();

double gc_time = 0.0;
void* garbage_collector () {
	printf("GC is not handled\n");
	exit(-1);
	struct timeval total_start, start, end;
	gettimeofday(&start, NULL);
	if (is_currently_collecting) {
		error("Can't collect while already collecting!\n");
	}
	is_currently_collecting = 1;
	debugf("Starting garbage collection!!\n");
	swap_heaps();
	scan_root_set();
	scan_new_heap();
	debugf("Finished garbage collecting!\n");
	debugf("Finished checking garbage collector!\n");
	is_currently_collecting = 0;
	gettimeofday(&end, NULL);
	gc_time += (end.tv_sec * 1000.0 + end.tv_usec / 1000.0) - (start.tv_sec * 1000.0 + start.tv_usec / 1000.0);
}

size_t sizeIValue(IValue * t) {
	switch (obj_type_intptr_t(t)) {
	case INT_OBJ:
		// return sizeof(IntIValue);
		return sizeof(intptr_t);
	case NULL_OBJ:
		// return sizeof(NullIValue);
		return sizeof(intptr_t);
	//skipping IValue types that should not be
	// in the feeny heap
	case SLOT_OBJ:
		return sizeof(SlotIValue);
	case ARRAY_OBJ: {
		ArrayIValue* a = (ArrayIValue*) t;
		return sizeof(ArrayIValue) + a->length * sizeof(IValue*);
	}
	case OBJ_OBJ: {
		ObjectIValue* o = (ObjectIValue*) t;
		int num_slots = get_num_var_slots(o->class_ptr);
		return sizeof(ObjectIValue) + sizeof(IValue*) * (num_slots);
	}
	default:
		debugf("Has tag %d\n", obj_type_intptr_t(t));
		error("Unexpected IValue type to iterate through!\n");
	}
}

IValue * copyIValue(IValue* from) {
	size_t size = sizeIValue(from);
	IValue * t = _halloc(size);
	memcpy(t, from, size);
	return t;
}

IValue* get_post_gc_ptr(IValue* obj) {
	char* old_heap = *heap_old;
	if ((uintptr_t)obj >= (uintptr_t)&old_heap[HEAP_SIZE] || (uintptr_t)obj < (uintptr_t)&old_heap[0]) {
		//TODO: fix this - the reason we cannot fail here is becase
		// we intermix constants and globals (so when we fix a global
		// we may later hit it as a constant)
		return obj;
	}
	if (obj_type_intptr_t(obj) == BROKEN_HEART) {
		return (IValue*) obj->space;
	}

	IValue* new_obj = copyIValue(obj);
	obj->tag = BROKEN_HEART;
	obj->space = (intptr_t)new_obj;
	return new_obj;
}

void scan_root_set() {
	char *heap = *heap_old;
	char *heap_curs = *heap_cur;
	// scan through global variables
	debugf("Heap is %x to %x\n", &heap[0], &heap[HEAP_SIZE]);
	debugf("NEW Heap is %x to %x\n", &heap_curs[0], &heap_curs[HEAP_SIZE]);
	debugf("Global slots!\n");
	Vector2* global_slots = get_global_slots();
	for (int i = 0; i < global_slots->size; ++i) {
		int idx = (int) vector_get_2(global_slots, i);
		IValue * v = get_global_slot_by_idx(idx);
		// this is because we are storing silly things in
		// the constant pool
		if (!v || ( obj_type_intptr_t(v) != INT_VAL && obj_type_intptr_t(v) != NULL_VAL &&
		            obj_type_intptr_t(v) != SLOT_VAL)) {
			continue;
		}
		IValue* ptr = get_global_slot_by_idx(idx);
		IValue* old_ptr = ptr;
		ptr = get_post_gc_ptr(ptr);
		debugf("%x to ", old_ptr);
		debugf("%x with obj_type %d\n", ptr, obj_type_intptr_t(ptr));
		set_val_constant(idx, ptr);
	}

	debugf("Constants slots!\n");// TODO: this is silly - remove it
	int constant_pool_size = get_constant_pool()->size;
	for (int i = 0; i < constant_pool_size; ++i) {
		IValue * ptr = get_val_constant(i);
		// this is because we are storing silly things in
		// the constant pool
		if (!ptr || (obj_type_intptr_t(ptr) != INT_VAL && obj_type_intptr_t(ptr) != NULL_VAL &&
		             obj_type_intptr_t(ptr) != SLOT_VAL)) {
			continue;
		}
		IValue* old_ptr = ptr;
		ptr = get_post_gc_ptr(ptr);
		debugf("%x to ", old_ptr);
		debugf("%x with obj_type %d\n", ptr, obj_type_intptr_t(ptr));
		set_val_constant(i, ptr);
	}

	// scan through operand stack
	debugf("Operand Stack!\n");
	Vector2* operand_stack = get_operand_stack();
	for (int i = 0; i < operand_stack->size; ++i) {
		IValue* ptr = vector_get_2(operand_stack, i);
		debugf("%x to ", ptr);
		ptr = get_post_gc_ptr(ptr);
		debugf("%x with obj_type %d\n", ptr, obj_type_intptr_t(ptr));
		vector_set_2(operand_stack, i, ptr);
	}

	debugf("Frames!\n");
	Frame* cur_frame = get_cur_frame();
	while (cur_frame != get_root_frame()) {
		Vector2* slots = cur_frame->slot_vec_ptr;
		for (int i = 0; i < cur_frame->slot_vec_ptr->size; ++i) {
			IValue* ptr = vector_get_2(slots, i);
			debugf("%x to ", ptr);
			ptr = get_post_gc_ptr(ptr);
			debugf("%x with obj_type %d\n", ptr, obj_type_intptr_t(ptr));
			vector_set_2(slots, i, ptr);
		}
		cur_frame = cur_frame->call_frame_ptr;
	}
}

void scan_IValue(IValue* t);

void scan_new_heap() {
	char *heap = *heap_cur;
	// scan through global variables
	debugf("Heap is %x to %x\n", &heap[0], &heap[HEAP_SIZE]);
	debugf("Current Heap Scan!\n");
	int position = 0;
	while (position < next_free_cur) {
		IValue* iv = (IValue*) &heap[position];
		debugf("Working on %x\n", iv);
		position += sizeIValue(iv);
		scan_IValue(iv);
	}
}

void scan_IValue(IValue* t) {
	switch (obj_type_intptr_t(t)) {
	case INT_OBJ:
	case NULL_OBJ:
		debugf("Either null or int!\n");
		return; // no child items
	//skipping IValue types that should not be
	// in the feeny heap
	case SLOT_OBJ: {
		SlotIValue* s = (SlotIValue*) t;
		debugf("%x to ", s->value);
		s->value = get_post_gc_ptr(s->value);
		debugf("%x\n", s->value);
		return;
	}
	case ARRAY_OBJ: {
		ArrayIValue* a = (ArrayIValue*) t;
		for (int i = 0; i < a->length; ++i) {
			debugf("%x to ", a->slots[i]);
			a->slots[i] = get_post_gc_ptr(a->slots[i]);
			debugf("%x\n", a->slots[i]);
		}
		return;
	}
	case OBJ_OBJ: {
		ObjectIValue* o = (ObjectIValue*) t;
		int num_slots = get_num_var_slots(o->class_ptr);
		for (int i = 0; i < num_slots; ++i) {
			debugf("%x to ", o->var_slots[i]);
			o->var_slots[i] = (SlotIValue*) get_post_gc_ptr((IValue*)o->var_slots[i]);
			debugf("%x\n", o->var_slots[i]);
		}
		debugf("%x to ", o->parent_obj_ptr);
		o->parent_obj_ptr = get_post_gc_ptr(o->parent_obj_ptr);
		debugf("%x\n", o->parent_obj_ptr);
		return;
	}
	default:
		debugf("Has tag:%d\n", obj_type_intptr_t(t));
		error("Unexpected IValue type to get size of!\n");
	}
}

// TODO: this is unsafe - we should not
// have a pointer to feeny heap on our stack
// b/c we cannot fix it during garbage collection
SlotIValue* copy_var_slot (SlotIValue * slot) {
	debugf("Copying var slot\n");
	SlotIValue* rSlot =
	    (SlotIValue*)_halloc(sizeof(SlotIValue));
	// if we interupt this halloc, then the pointer src
	// is on our stack!
	rSlot->tag = slot->tag;
	rSlot->name = slot->name;
	rSlot->value = NULL;
	return rSlot;
}

Vector2* get_constant_pool () {
	static Vector2* constant_pool = NULL;
	if (constant_pool == NULL) {
		constant_pool = make_vector_2();
	}
	return constant_pool;
}

// get value from the constant pool
intptr_t get_val_constant (int idx) {
	Vector2* constant_pool = get_constant_pool();
	return vector_get_2(constant_pool, idx);
}

void set_val_constant(int idx, IValue * v) {
	Vector2* constant_pool = get_constant_pool();
	return vector_set_2(constant_pool, idx, v);
}

// returns the string content
// indexed by idx in the constant pool
char* get_str_constant(int idx) {
	IValue* val = get_val_constant(idx);
	if (val == NULL || obj_type_intptr_t(val) != STR_OBJ) {
		error("Error: get string constant.\n");
	}
	return ((StringIValue*)val)->value;
}

// instructor pointer
static int inst_ptr = -1;

static Vector2* inst_label_vec = NULL;
void init_label_vec() {
	if (!inst_label_vec) {
		inst_label_vec = make_vector_2();
	}
}

void set_label_addr (char* label, int addr) {
	if (!inst_label_vec) inst_label_vec = make_vector_2();
	add_item(inst_label_vec, label, addr);
}
int get_label_addr (char* label) {
	return find_item(inst_label_vec, label);
}
// hashtable that maps string names to global
// slots' index in the constant pool
// each index is guaranteed to refer to either
// a Slot object or a Method object
static Vector2* global_slots = NULL;
static Vector2* global_slot_vec = NULL;
void set_global_slots (Vector2* slots) {
	int val_idx, name_idx;
	IValue* value = NULL;
	char* name_str = NULL;
	global_slots = get_global_slots();
	// establish string name to
	// slot index hashtable
	global_slot_vec = make_vector_2();
	for (int i = 0; i < slots->size; i++) {
		val_idx = (int)vector_get_2(slots, i);
		value = get_val_constant(val_idx);
		name_idx = ((SlotIValue*)value)->name;
		name_str = get_str_constant(name_idx);
		add_item(global_slot_vec, name_str, val_idx);
	}
}
Vector2* get_global_slots () {
	static Vector2* global_slots = NULL;
	if (!global_slots) {
		global_slots = make_vector_2();
	}
	return global_slots;
}

IValue* get_global_slot_by_name (char* name) {
	int slot_idx = find_item(global_slot_vec, name);
	if (slot_idx < 0) {
		error("Error: get global slot by name.\n");
	}
	IValue* val = get_val_constant(slot_idx);
	if (val == NULL) {
		error("Error[2]: get global slot by name.\n");
	}
	return val;
}

IValue* get_global_slot_by_idx (int slot_idx) {
	IValue* val = get_val_constant(slot_idx);
	if (val == NULL) {
		error("Error[2]: get global slot by name.\n");
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

static Vector2* operand_stack = NULL;
Vector2* get_operand_stack () {
	if (operand_stack == NULL)
		operand_stack = make_vector_2();
	return operand_stack;
}

void stack_push (intptr_t val) {
	vector_add_2(operand_stack, val);
}
intptr_t stack_pop () {
	if (operand_stack->size == 0) {
		return NULL;
	}
	return vector_pop_2(operand_stack);
}
intptr_t stack_peek () {
	return vector_peek_2(operand_stack);
}

static Frame* current_frame_ptr = NULL;
Frame* get_root_frame () {
	static Frame* root_frame_ptr = NULL;
	if (root_frame_ptr == NULL) {
		root_frame_ptr = create_frame(-1, NULL, NULL);
		current_frame_ptr = root_frame_ptr;
	}
	return root_frame_ptr;
}
Frame* get_cur_frame () {
	if (!current_frame_ptr) {
		get_root_frame();
	}
	return current_frame_ptr;
}

void set_cur_frame (Frame * new_frame) {
	/*if (current_frame_ptr != (Frame*)(new_frame->call_frame_ptr)) {
	  printf("Error: set_cur_frame.\n");
	  exit(-1);
	}*/
	current_frame_ptr = new_frame;
}

void set_frame_slot (Frame * frame, int idx, intptr_t val) {
	if (idx == frame->slot_vec_ptr->size) {
		vector_add_2(frame->slot_vec_ptr, val);
	} else if (0 <= idx && idx < frame->slot_vec_ptr->size) {
		vector_set_2(frame->slot_vec_ptr, idx, val);
	} else {
		error("Error: set frame value.\n");
	}
}

intptr_t get_frame_slot (Frame * frame, int idx) {
	if (idx >= frame->slot_vec_ptr->size) {
		error("Error: get frame value.\n");
	}
	return (intptr_t)vector_get_2(frame->slot_vec_ptr, idx);
}

Frame* create_frame (int call_ins_idx,
                     MethodIValue * func_ptr, Frame * call_frame) {
	Frame* ret = malloc(sizeof(Frame));
	ret->slot_vec_ptr = make_vector_2();
	ret->call_ins_idx = call_ins_idx;
	ret->func_ptr = func_ptr;
	ret->call_frame_ptr = call_frame;
	return ret;
}

//============================================================
//=============== BYTE CODE INTERPRETER ======================
//============================================================

// Sets the i’th slot in the current
// local frame to the top value in
// the operand stack.
void exec_set_local_op (SetLocalIns * i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	intptr_t val = stack_peek();
	if (val == NULL) {
		error("Error: set local op.\n");
	}
	Frame* frame = get_cur_frame();
	set_frame_slot(frame, slot_idx, val);
	// NOTE: do not pop from the stack
	inst_ptr++;
}

// Retrieves the i’th slot in the
// current local frame and pushes
// it onto the operand stack.
void exec_get_local_op (GetLocalIns * i) {
	// slot_idx is the index of the slot
	// in the current local frame
	int slot_idx = i->idx;
	Frame* frame = get_cur_frame();
	if (frame == NULL) {
		error("Error: get local op.\n");
	}
	intptr_t val = get_frame_slot(frame, slot_idx);
	stack_push(val);
	inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_set_global_quick(SetGlobalIns* i) {
	int name_idx = i->name;
	char* name_str = get_str_constant(name_idx);

	name_idx = find_item(global_slot_vec, name_str);
	if (name_idx < 0) {
		error("Error: get global slot by name.\n");
	}
	i->tag = (OpCode)SET_GLOBAL_OP_QUICK;
	i->name = name_idx;
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_op (SetGlobalIns * i) {
	make_set_global_quick(i);
}

// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_quick_op (SetGlobalQuickIns * i) {
	int name_idx = i->idx;
	IValue* val = stack_peek();
	if (val == NULL) {
		error("Error: set global op.\n");
	}
	IValue* slot = get_global_slot_by_idx(name_idx);
	if (slot == NULL || obj_type_intptr_t(slot) != SLOT_OBJ) { // TODO: make safe?
		error("Error[2]: set global op.\n");
	}
	SlotIValue* rSlot = (SlotIValue*)slot;
	rSlot->value = val;
	// NOTE: do not pop from the stack
	inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_get_global_quick(GetGlobalIns* i) {
	int name_idx = i->name;
	char* name_str = get_str_constant(name_idx);

	name_idx = find_item(global_slot_vec, name_str);
	if (name_idx < 0) {
		error("Error: get global slot by name.\n");
	}
	i->tag = (OpCode)GET_GLOBAL_OP_QUICK;
	i->name = name_idx;
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_op (GetGlobalIns * i) {
	make_get_global_quick(i);
}

// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_quick_op (GetGlobalQuickIns * i) {
	int idx = i->idx;
	IValue* slot = get_global_slot_by_idx(idx);
	if (slot == NULL || obj_type_intptr_t(slot) != SLOT_OBJ) { // TODO: make safe?
		error("Error: get global op.\n");
	}
	SlotIValue* rSlot = (SlotIValue*)slot;
	IValue* val = rSlot->value;
	if (val == NULL) {
		error("Error[2]: get global op.\n");
	}
	stack_push(val);
	inst_ptr++;
}

void make_branch_quick(BranchIns *i) {
	int label_idx = i->name;
	char* label_str = get_str_constant(label_idx);
	int addr = get_label_addr(label_str);

	i->name = addr;
	i->tag = (OpCode)BRANCH_OP_QUICK;
}

// Pops a value from the operand
// stack. If this value is not Null,
// then sets the instruction pointer to
// the instruction address associated
// with the name given by the String
// object at index i.
void exec_branch_op (BranchIns * i) {
	make_branch_quick(i);
}

void exec_branch_quick_op (BranchQuickIns * i) {
	intptr_t val = stack_pop();
	debugf("branch: %d\n", val);
	if (val == NULL) {
		error("Error: branch op.\n");
	}
	if (obj_type_intptr_t(val) != NULL_OBJ) {
		inst_ptr = i->addr;
	} else {
		inst_ptr++;
	}
}

void make_goto_quick(GotoIns *i) {
	int label_idx = i->name;
	char* label_str = get_str_constant(label_idx);
	int addr = get_label_addr(label_str);

	i->name = addr;
	i->tag = (OpCode)GOTO_OP_QUICK;
}

// Sets the instruction pointer to the instruction
// address associated with the name given by
// the String object at index i.
void exec_goto_op (GotoIns * i) {
	make_goto_quick(i);
}

void exec_goto_quick_op (GotoQuickIns *i) {
	inst_ptr = i->addr;
}

// Pops and discards the top value from
// the operand stack.
void exec_drop_op () {
	stack_pop();
	inst_ptr++;
}

// Retrieves the object at index i in
// the constant pool, which refers to
// either an Int object or a Null object,
// and pushes it onto the operand stack.
void exec_lit_op (LitIns * i) {
	int val_idx = i->idx;
	intptr_t val = get_val_constant(val_idx);
	/*if (val == NULL || (obj_type_intptr_t(val) != INT_OBJ && obj_type_intptr_t(val) != NULL_OBJ)) {
		printf("Error in LIT_OP. %x\n", val);
		exit(-1);
	}*/
	stack_push(val);
	inst_ptr++;
}

// Associates a name with the address of
// this instruction. The name is given by
// the String object at index i.
void process_label_op (LabelIns * i, int i_ptr) {
	int label_idx = i->name;
	char* label_str = get_str_constant(label_idx);
	set_label_addr(label_str, i_ptr);
}

void exec_label_op (LabelIns * i) {
	inst_ptr++;
}

// First pops the initializing value from
// the operand stack, and then pops the
// length of the array from the operand
// stack. Creates a new array with the
// given length, with each element initizlied
// to the given value, and pushes the array
// onto the operand stack.
void exec_array_op () {
	IValue* init_value = stack_pop();
	assert_not_null(init_value);
	intptr_t lengthi = stack_pop();

	// IntIValue* len = to_int_val(lengthi);
	// int length = len->value;

	int length = TAG_PTR_TO_INT(lengthi);
	stack_push(init_value); // for safety

	ArrayIValue* t = _halloc(sizeof(ArrayIValue) + sizeof(IValue*)*length);
	t->tag = ARRAY_OBJ;
	t->length = length;
	init_value = stack_pop(); // for safety
	for (int i = 0; i < length; ++i) {
		t->slots[i] = init_value;
	}

	stack_push((IValue*) t);
	inst_ptr++;
}

// Pops n values from the operand
// stack, and then prints them out according to the given format string.
// Arguments are spliced in from the deepest value in the stack (last
// popped) to the shallowest value in the stack (first popped). Null is
// then pushed onto the operand stack.
void exec_printf_op (PrintfIns * i) {
	int format_idx = i->format;
	int arity = i->arity;
	char* format_str = get_str_constant(format_idx);
	char replace_char[] = "~";
	// replace all ~ in format string into the corresponding arguments
	char *new_str = copy_string(format_str);

	Vector2* args = make_vector_2();
	int n = arity;
	while (n-- > 0) {
		vector_add_2(args, stack_pop());
	}
	if (args->size != arity) {
		error("Error: exec_printf_op.\n");
	}
	n = arity;
	while (n-- > 0) {
		char *old_str = new_str;
		// pop one elemet from the stack
		intptr_t val = vector_pop_2(args);
		char *arg_str = toString(val);
		new_str = str_replace(old_str, replace_char, arg_str);
		if (arg_str != NULL) free(arg_str);
		if (old_str != NULL) free(old_str);
	}
	if (new_str != NULL) {
		printf("%s", new_str);
		free(new_str);
	}
	vector_free_2(args);
	stack_push(make_null_obj());
	inst_ptr++;
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
void exec_object_op (ObjectIns * i) {
	int class_idx = i->class;
	IValue* class = get_val_constant(class_idx);
	if (class == NULL || obj_type_intptr_t(class) != CLASS_VAL) {
		error("Error: exec_object_op.\n");
	}
	ClassIValue* class_val = (ClassIValue*)class;
	int num_slots = get_num_var_slots(class_val);
	// init new object value
	// debugf("Making object\n");
	ObjectIValue* obj = (ObjectIValue*)halloc(sizeof(ObjectIValue) + sizeof(IValue*) * (num_slots));
	for (int i = class_val->slots->size - 1; i >= 0 ; i--) {

		int slot_idx = (int)vector_get_2(class_val->slots, i);
		IValue* slot_in_class = get_val_constant(slot_idx);
		assert_not_null(slot_in_class);
		if (obj_type_intptr_t(slot_in_class) == METHOD_OBJ) {
			// Only copy all variable slots - stop on the first method slot
			continue;
		} else if (obj_type_intptr_t(slot_in_class) == SLOT_VAL) {
			// copy var slot
			SlotIValue* new_slot =
			    copy_var_slot((SlotIValue*)slot_in_class);
			// first var slot will be initialized
			// to the deepest value on the stack
			new_slot->value = stack_pop();
			if (new_slot->value == NULL) {
				error("Error[2]: exec_object_op.\n");
			}
			obj->var_slots[i] = new_slot;
		} else {
			error("Error[3]: exec_object_op.\n");
		}
	}
	obj->parent_obj_ptr = stack_pop();
	obj->class_ptr = class_val;
	stack_push((IValue*)obj);
	inst_ptr++;
}

// Pops a value from the operand
// stack assuming it is an object. Retrieves the value in the object stored
// at the variable slot with name given by the String object at index i,
// and pushes it onto the operand stack.
void exec_slot_op (SlotIns* i) {
	int name_idx = i->name;
	char* slot_name = get_str_constant(name_idx);
	assert_not_null(slot_name);
	// pop the object to get from
	IValue* obj = stack_pop();
	assert_obj_obj(obj);
	IValue* slot = find_slot_by_name((ObjectIValue*)obj, slot_name);
	// TODO: untag optimization
	SlotIValue* slotVal = to_slot_val(slot);
	stack_push(slotVal->value);
	inst_ptr++;
}

// Pops the value to store, x, from
// the operand stack, and then pops the object to store it into. Stores
// x into the object at the variable slot with name given by the String
// object at index i. x is then pushed onto the operand stack.
void exec_set_slot_op (SetSlotIns * i) {
	int name_idx = i->name;
	char* slot_name = get_str_constant(name_idx);
	assert_not_null(slot_name);
	// pop the value to be stored
	IValue* val = stack_pop();
	assert_not_null(val);
	// pop the object to store into
	IValue* obj = stack_pop();
	assert_obj_obj(obj);
	IValue* slot = find_slot_by_name((ObjectIValue*)obj, slot_name);
	// TODO: untag optimization
	// debugf("Called from set slot op\n");
	SlotIValue* slotVal = to_slot_val(slot);
	slotVal->value = val;
	stack_push(val);
	inst_ptr++;
}

void call_func (MethodIValue * function_slot, int arity) {
	// pops arguments from the stack
	Vector2* tmp_vec = make_vector_2();
	// create a new local frame
	Frame* local_frame = create_frame(inst_ptr,
	                                  function_slot, get_cur_frame());
	// the first arity slots are the arguments
	int n = arity;
	while (n-- > 0) {
		vector_add_2(tmp_vec, stack_pop());
	}
	n = arity;
	while (n-- > 0) {
		vector_add_2(local_frame->slot_vec_ptr, vector_pop_2(tmp_vec));
	}
	vector_free_2(tmp_vec);
	set_cur_frame(local_frame);
	// set the instructor pointer to the first
	// instruction in the function body
	inst_ptr = 0;
}

void make_call_quick(CallIns * i) {
	int function_name_idx = i->name;
	int arity = i->arity;

	char* function_name = get_str_constant(function_name_idx);
	int name_idx = find_item(global_slot_vec, function_name);
	if (name_idx < 0) {
		error("Error: get global slot by name.\n");
	}

	i->tag = (OpCode)CALL_OP_QUICK;
	i->name = name_idx;
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
void exec_call_op (CallIns * i) {
	make_call_quick(i);
}

void exec_call_quick_op(CallQuickIns * i) {
	IValue* function_slot = get_global_slot_by_idx(i->idx);
	if (function_slot == NULL || obj_type_intptr_t(function_slot) != METHOD_OBJ) {
		error("Error: exec_call_op.\n");
	}
	call_func((MethodIValue*)function_slot, i->arity);
	// instruction pointer is updated in call_func
}

// Registers the parent frame of the
// current local frame as the new current
// frame. Execution proceeds by
// setting the instruction pointer to the
// return address stored in the current
// local frame. The local frame is no intptr_ter
// used after a Return instruction, and any
// storage allocated for it may be reclaimed.
void exec_return_op () {
	Frame* frame = get_cur_frame();
	inst_ptr = frame->call_ins_idx + 1;
	Frame* call_frame = (Frame*)frame->call_frame_ptr;
	set_cur_frame(call_frame);
	free_frame(frame);
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
void exec_call_slot_op (CallSlotIns * i) {
	// debugf("Starting exec_call_slot_op\n");
	int method_name_idx = i->name;
	char* method_name = get_str_constant(method_name_idx);
	int arity = i->arity > 0 ? i->arity - 1 : 0;
	// pops arguments from the stack
	Vector2* tmp_arg_vec = make_vector_2();
	// the first arity slots are the arguments
	int n = arity;
	while (n-- > 0) {
		vector_add_2(tmp_arg_vec, stack_pop());
	}
	// pop the receiver object
	intptr_t receiver_ptr = stack_pop();
	assert_not_null(receiver_ptr);
	exp_assert(obj_type_intptr_t(receiver_ptr) != NULL_OBJ,
	           "Null object does not have methods!");
	intptr_t retVal = NULL;

	switch (obj_type_intptr_t(receiver_ptr)) {
	// handle built in functions
	case INT_OBJ: {
		intptr_t arg = vector_get_2(tmp_arg_vec, 0);
		// debugf("arg tag: %d\n", obj_type_intptr_t(arg));
		// debugf("arg: %d\n", arg);
		exp_assert(arity == 1 && obj_type_intptr_t(arg) == INT_OBJ,
		           "native int function error - %s",
		           arity != 1 ? "not enough arguments!" :
		           "wrong argument type!");
		if (!strcmp(method_name, "add")) {
			retVal = int_obj_add(receiver_ptr, arg);
		} else if (!strcmp(method_name, "sub")) {
			retVal = int_obj_sub(receiver_ptr, arg);
		} else if (!strcmp(method_name, "mul")) {
			retVal = int_obj_mul(receiver_ptr, arg);
		} else if (!strcmp(method_name, "div")) {
			retVal = int_obj_div(receiver_ptr, arg);
		} else if (!strcmp(method_name, "mod")) {
			retVal = int_obj_mod(receiver_ptr, arg);
		} else if (!strcmp(method_name, "gt")) {
			retVal = gt(receiver_ptr, arg);
		} else if (!strcmp(method_name, "ge")) {
			retVal = ge(receiver_ptr, arg);
		} else if (!strcmp(method_name, "lt")) {
			retVal = lt(receiver_ptr, arg);
		} else if (!strcmp(method_name, "le")) {
			retVal = le(receiver_ptr, arg);
		} else if (!strcmp(method_name, "eq")) {
			retVal = eq(receiver_ptr, arg);
		} else {
			exp_assert(0, "unknown native int function");
		}
		stack_push(retVal);
		inst_ptr++;
		break;
	}
	case ARRAY_OBJ: {
		intptr_t result = NULL;
		if (!strcmp(method_name, "length") && arity == 0) {
			result = array_length((ArrayIValue*) receiver_ptr);
		} else {
			Vector2* args = make_vector_2();
			n = arity;
			while (n-- > 0) {
				vector_add_2(args, vector_pop_2(tmp_arg_vec));
			}
			intptr_t first_arg = vector_get_2(args, 0);
			exp_assert(arity > 0 && obj_type_intptr_t(first_arg) == INT_OBJ,
			           "native array function error - %s",
			           arity < 1 ? "not enough arguments!" :
			           "incorrect argument type!");
			if (!strcmp(method_name, "set") && arity == 2) {
				result = array_set((ArrayIValue*)receiver_ptr,
				                   first_arg, (intptr_t)vector_get_2(args, 1));
			} else if (!strcmp(method_name, "get") && arity == 1) {
				result = array_get((ArrayIValue*)receiver_ptr, first_arg);
			} else {
				exp_assert(0, "unknown native array function");
			}
			vector_free_2(args);
		}
		stack_push(result);
		inst_ptr++;
		break;
	}
	case OBJ_OBJ: {
		IValue* slot = find_slot_by_name((ObjectIValue*)receiver_ptr, method_name);
		assert_not_null(slot);
		MethodIValue* method_slot = to_function_val(slot);
		Vector2* args = make_vector_2();
		// create a new local frame
		Frame* local_frame = create_frame(inst_ptr,
		                                  method_slot, get_cur_frame());
		// the slot 0 is the receiver object
		vector_add_2(local_frame->slot_vec_ptr, receiver_ptr);
		n = arity;
		while (n-- > 0) {
			vector_add_2(local_frame->slot_vec_ptr, vector_pop_2(tmp_arg_vec));
		}
		set_cur_frame(local_frame);
		inst_ptr = 0;
		vector_free_2(args);
		break;
	}
	default:
		// printf("receiver_ptr->tag: %d\n", receiver_ptr->tag);
		exp_assert(0, "Cannot call method on null object!");
		exit(-1);
	}
	vector_free_2(tmp_arg_vec);
}

void exec_ins (ByteIns * ins) {
	// print_ins(ins);
	// printf("\n");
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
		//break;
	}
	case CALL_OP_QUICK: {
		CallQuickIns* i = (CallQuickIns*)ins;
		exec_call_quick_op(i);
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
		//break; // exec_set_global_op transforms the type
	}
	case SET_GLOBAL_OP_QUICK: {
		SetGlobalQuickIns* i = (SetGlobalQuickIns*)ins;
		exec_set_global_quick_op(i);
		break;
	}
	case GET_GLOBAL_OP: {
		GetGlobalIns* i = (GetGlobalIns*)ins;
		exec_get_global_op(i);
		//break; // exec_get_global_op transforms the type
	}
	case GET_GLOBAL_OP_QUICK: {
		GetGlobalQuickIns* i = (GetGlobalQuickIns*)ins;
		exec_get_global_quick_op(i);
		break;
	}
	case BRANCH_OP: {
		BranchIns* i = (BranchIns*)ins;
		exec_branch_op(i);
		//break; // exec_branch_op transforms the type
	}
	case BRANCH_OP_QUICK: {
		BranchQuickIns* i = (BranchQuickIns*)ins;
		exec_branch_quick_op(i);
		break;
	}
	case GOTO_OP: {
		GotoIns* i = (GotoIns*)ins;
		exec_goto_op(i);
		//break; // exec_goto_op transforms the type
	}
	case GOTO_OP_QUICK: {
		GotoQuickIns* i = (GotoQuickIns*) ins;
		exec_goto_quick_op(i);
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

ClassIValue* create_class(Vector2* values, ClassValue * v2) {
	ClassIValue* new_v =
	    (ClassIValue*)malloc(sizeof(ClassIValue));
	new_v->tag = CLASS_OBJ;
	new_v->slots = v2->slots;
	new_v->name_to_slot_vec = make_vector_2();

	int index_slot = 0, index_method = v2->slots->size - 1;
	while (index_slot < index_method && index_method >= 0) {
		while (index_method >= 0 && obj_type_intptr_t((IValue*)vector_get_2(values, (int)vector_get_2(v2->slots, index_method))) == METHOD_OBJ) {
			index_method--;
		}
		while (index_slot <= index_method && obj_type_intptr_t((IValue*)vector_get_2(values, (int)vector_get_2(v2->slots, index_slot))) == SLOT_OBJ) {
			index_slot++;
		}
		if (index_slot < index_method && index_method >= 0) {
			void * ind = vector_get_2(v2->slots, index_slot);
			vector_set_2(v2->slots, index_slot, vector_get_2(v2->slots, index_method));
			vector_set_2(v2->slots, index_method, ind);
			index_slot++;
			index_method--;
		}
	}

	// iterate over all slots
	for (int i = 0; i < v2->slots->size; i++) {
		// get the slot value
		int slot_index = (int)vector_get_2(v2->slots, i);
		IValue* value = vector_get_2(values, slot_index);
		if (obj_type_intptr_t(value) != SLOT_OBJ && obj_type_intptr_t(value) != METHOD_OBJ) {
			error("non-slot or method value in class\n");
		}
		int name_index = ((SlotValue*)value)->name;
		// get the string value of name
		value = vector_get_2(values, name_index);
		if (obj_type_intptr_t(value) != STR_OBJ) {
			error("Slot value name is not a string.\n");
		}
		char* name_ptr = ((StringValue*)value)->value;
		// map the string name to the slot index in new_v->slots
		add_item(new_v->name_to_slot_vec, name_ptr, i);
	}
	return new_v;
}

void associate_labels(MethodIValue *method) {
	for (int i = 0; i < method->code->size; i++) {
		ByteIns* ins = (ByteIns*)vector_get_2(method->code, i);
		if (ins->tag == LABEL_OP) {
			process_label_op((LabelIns*)ins, i);
		}
	}
}

intptr_t toIntIValue(IntValue* v) {
	// return make_int_obj(v->value);
	return INT_TO_TAG_PTR(v->value);
}

StringIValue* toStringIValue(StringValue*v) {
	StringIValue * ret = malloc(sizeof(StringIValue));
	ret->tag = STR_OBJ;
	ret->value = v->value;
	return ret;
}

MethodIValue* toMethodIValue(MethodValue*v) {
	MethodIValue * ret = malloc(sizeof(MethodIValue));
	ret->tag = METHOD_OBJ;
	ret->code = v->code;
	ret->name = v->name;
	ret->nargs = v->nargs;
	ret->nlocals = v->nlocals;
	return ret;
}

// add values in the byte code syntax tree into
// the runtime constatnt pool
void addto_constant_pool (Vector2* values, Value * v) {
	Vector2* constant_pool = get_constant_pool();
	switch (v->tag) {
	case INT_VAL:
		vector_add_2(constant_pool, toIntIValue((IntValue*)v));
		break;
	case NULL_VAL:
		vector_add_2(constant_pool, make_null_obj());
		break;
	case STRING_VAL:
		// tagging optimization
		// val = tag_opt(val);
		vector_add_2(constant_pool, toStringIValue((StringValue*)v));
		break;
	case METHOD_VAL: {
		MethodIValue* m = toMethodIValue((MethodValue*)v);
		// tagging optimization
		// m = tag_opt(m);
		vector_add_2(constant_pool, m);
		associate_labels(m);
		break;
	}
	case SLOT_VAL: {
		// these are global variables and should be stored thereby.
		// currently I am using a hack!
		SlotValue* v2 = (SlotValue*)v;

		// debugf("Working on var slot\n");
		SlotIValue* new_v = (SlotIValue*)halloc(sizeof(SlotIValue));
		new_v->tag = SLOT_VAL;
		new_v->name = v2->name;
		new_v->value = NULL;
		// tagging optimization
		// new_v = tag_opt(new_v);
		vector_add_2(constant_pool, new_v);
		break;
	}
	case CLASS_VAL: {
		ClassValue* v2 = (ClassValue*)v;
		ClassIValue* new_v = create_class(values, v2);
		// tagging optimization
		// new_v = tag_opt(new_v);
		vector_add_2(constant_pool, new_v);
		break;
	}
	default:
		printf("Value with unknown tag: %d\n", v->tag);
		exit(-1);
	}
}

void start_exec() {
	// init the root frame
	get_root_frame();
	// init the instruction label table
	init_label_vec ();
	// init the operand stack
	get_operand_stack ();
	// call the entry function
	IValue* slot = get_val_constant(get_entry_function());
	call_func(to_function_val(slot), 0);
	// start executing the program
	int i = 0;
	while (1) {
		// get the current instruction
		Frame* cur_frame = get_cur_frame();
		if (cur_frame == get_root_frame()) {
			break; // execution stops
		}
		Vector2* code = cur_frame->func_ptr->code;
		if (inst_ptr < 0 || inst_ptr >= code->size) {
			printf("Error: wrong inst_ptr: %d.\n", inst_ptr);
			exit(-1);
		}
		//debugf("%d, %d\n", code->size, inst_ptr);
		//debugf("stack size: %d\n", operand_stack->size);
		ByteIns* ins = vector_get_2(code, inst_ptr);
		exec_ins(ins);
	}
}

void exec_prog (Program * p) {
	// constant pool
	Vector2* vs = p->values;
	for (int i = 0; i < vs->size; i++) {
		addto_constant_pool(vs, vector_get_2(vs, i));
	}
	// global slot
	set_global_slots(p->slots);
	// entry function
	set_entry_function(p->entry);
	// start exec the program
	start_exec();
}

void interpret_bc (Program * p) {
	/*printf("Interpreting Bytecode Program:\n");
	print_prog(p);
	printf("\n");*/
	exec_prog(p);
}

//============================================================
//================== UTIL FUNCITONS ==========================
//============================================================

void free_frame(Frame* frame) {
	vector_free_2(frame->slot_vec_ptr);
	free(frame);
}

MethodIValue* to_function_val (IValue* val) {
	if (val == NULL || obj_type_intptr_t(val) != METHOD_OBJ) {
		error("Error: not a function value.\n");
	}
	return (MethodIValue*) val;
}

SlotIValue* to_slot_val (IValue* val) {
	if (val == NULL || obj_type_intptr_t(val) != SLOT_VAL) {
		// debugf("Has addr: %x\n", val);
		// debugf("Has tag: %d\n", obj_type_intptr_t(val));
		error("Error: not a var slot.\n");
	}
	return (SlotIValue*) val;
}

IntIValue* to_int_val (IValue* val) {
	if (val == NULL || obj_type_intptr_t(val) != INT_OBJ) {
		error("Error: to_int_val.\n");
	}
	return (IntIValue*)val;
}

void assert_not_null (void* ptr) {
	if (ptr == NULL) {
		error("Error: assert_not_null.\n");
	}
}

void assert_obj_obj (IValue* ptr) {
	if (ptr == NULL || obj_type_intptr_t(ptr) != OBJ_OBJ) {
		error("Error: assert_obj_obj.\n");
	}
}

void exp_assert (int i, const char * fmt, ...) {
	va_list args;
	va_start(args, fmt);
	if (!i) {
		printf("Encountered error: ");
		vprintf(fmt, args);
		printf("\n");
		exit(-1);
	}
	va_end(args);
}

PtrTag ptr_tag_intptr_t (intptr_t o) {
	int val = (o & 0x0000000000000007);
	switch (val) {
	case 0: return OBJ_TAG;
	case 1: return INT_TAG;
	case 2: return NULL_TAG;
	}
}

ObjTag obj_type_intptr_t (intptr_t o) {
	switch (ptr_tag_intptr_t(o)) {
	case INT_TAG:
		return INT_OBJ;
	case NULL_TAG:
		return NULL_OBJ;
	case OBJ_TAG:
		return obj_type((IValue*)o);
	default:
		error("unknown tag in obj tag");
	}
	//return min((ObjTag)o->tag, OBJ_OBJ);
}

ObjTag obj_type (IValue * o) {
	switch ((ObjTag)(o->tag)) {
	case INT_OBJ:
		printf("INT_OBJ does not exist!\n");
		exit(-1);
	case NULL_OBJ:
		printf("NULL_OBJ does not exist!\n");
		exit(-1);
	case STR_OBJ:
	case METHOD_OBJ:
	case SLOT_OBJ:
	case CLASS_OBJ:
	case ARRAY_OBJ:
	case BROKEN_HEART:
	case OBJ_OBJ:
		return (ObjTag)o->tag;
	default:
		return OBJ_OBJ;
	}
	//return min((ObjTag)o->tag, OBJ_OBJ);
}

intptr_t make_null_obj () {
	// debugf("Making null\n");
	/*NullIValue* n = halloc(sizeof(NullIValue));
	n->tag = NULL_OBJ;
	n->space = 0;
	return (intptr_t)n;
	*/
	return (intptr_t)2;
}

size_t halloc_int_bytes = 0;
intptr_t make_int_obj (int value) {
	// debugf("Making int\n");
	// intptr_t tmp = value;
	//return (IntIValue*)(8 * tmp);
	return INT_TO_TAG_PTR(value);
	/*
	IntIValue* t = halloc(sizeof(IntIValue));
	halloc_int_bytes += sizeof(IntIValue);
	t->tag = INT_OBJ;
	t->value = value;
	return t;
	*/
}

intptr_t array_length (ArrayIValue* array) {
	return make_int_obj(array->length);
}

intptr_t array_set (ArrayIValue* a, intptr_t i_ptr, intptr_t v_ptr) {
	int i2 = TAG_PTR_TO_INT(i_ptr);
	if (i2 >= a->length || i2 < 0) {
		printf("array index out of bound. array length: %d. index: %d", a->length, i2);
		exit(-1);
	}
	a->slots[i2] = v_ptr;
	return make_null_obj();
}

intptr_t array_get (ArrayIValue* a, intptr_t i_ptr) {
	int i2 = TAG_PTR_TO_INT(i_ptr);
	if (i2 >= a->length || i2 < 0) {
		printf("array index out of bound. array length: %d. index: %d", a->length, i2);
		exit(-1);
	}
	return a->slots[i2];
}


// TODO: this is unsafe - we should not have a pointer
// to an object in the feeny heap while we are alloc'ing
// TODO: this is now never called
ArrayIValue* make_array_obj (int length, intptr_t init) {
	// debugf("Making array\n");
	ArrayIValue* t = _halloc(sizeof(ArrayIValue) + sizeof(IValue*)*length);
	t->tag = ARRAY_OBJ;
	t->length = length;
	for (int i = 0; i < length; ++i) {
		t->slots[i] = init;
	}
	return t;
}

intptr_t int_obj_add (intptr_t x, intptr_t y) {
	return INT_TO_TAG_PTR(TAG_PTR_TO_INT(x) + TAG_PTR_TO_INT(y));
	// return make_int_obj(x->value + y->value);
}
intptr_t int_obj_sub (intptr_t x, intptr_t y) {
	return INT_TO_TAG_PTR(TAG_PTR_TO_INT(x) - TAG_PTR_TO_INT(y));
	// return make_int_obj(x->value - y->value);
}
intptr_t int_obj_mul (intptr_t x, intptr_t y) {
	return INT_TO_TAG_PTR(TAG_PTR_TO_INT(x) * TAG_PTR_TO_INT(y));
	// return make_int_obj(x->value * y->value);
}
intptr_t int_obj_div (intptr_t x, intptr_t y) {
	return INT_TO_TAG_PTR(TAG_PTR_TO_INT(x) / TAG_PTR_TO_INT(y));
	// return make_int_obj(x->value / y->value);
}
intptr_t int_obj_mod (intptr_t x, intptr_t y) {
	return INT_TO_TAG_PTR(TAG_PTR_TO_INT(x) % TAG_PTR_TO_INT(y));
	// return make_int_obj(x->value % y->value);
}

intptr_t eq(intptr_t x, intptr_t y) {
	//return x->value == y->value ?
	//       (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
	return (TAG_PTR_TO_INT(x)) == (TAG_PTR_TO_INT(y)) ? (INT_TO_TAG_PTR(0)) : (intptr_t)make_null_obj();
}
intptr_t lt(intptr_t x, intptr_t y) {
	// return x->value < y->value ?
	//       (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
	return (TAG_PTR_TO_INT(x)) < (TAG_PTR_TO_INT(y)) ? (INT_TO_TAG_PTR(0)) : (intptr_t)make_null_obj();
}
intptr_t le(intptr_t x, intptr_t y) {
	// return x->value <= y->value ?
	//       (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
	return (TAG_PTR_TO_INT(x)) <= (TAG_PTR_TO_INT(y)) ? (INT_TO_TAG_PTR(0)) : (intptr_t)make_null_obj();
}
intptr_t gt(intptr_t x, intptr_t y) {
	// return x->value > y->value ?
	//       (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
	return (TAG_PTR_TO_INT(x)) > (TAG_PTR_TO_INT(y)) ? (INT_TO_TAG_PTR(0)) : (intptr_t)make_null_obj();
}
intptr_t ge(intptr_t x, intptr_t y) {
	// return x->value >= y->value ?
	//       (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
	return (TAG_PTR_TO_INT(x)) >= (TAG_PTR_TO_INT(y)) ? (INT_TO_TAG_PTR(0)) : (intptr_t)make_null_obj();
}

// convert an interpreter object into a string
char* toString (intptr_t val_ptr) {
	switch (obj_type_intptr_t(val_ptr)) {
	case INT_OBJ: {
		// return intToString(((IntIValue*)val_ptr)->value);
		int val = TAG_PTR_TO_INT(val_ptr);
		return intToString(val);
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
	case ARRAY_OBJ: {
		return arrayToString((ArrayIValue*)val_ptr);
	}
	default:
		error("Error: toString.\n");
	}
	return NULL;
}

char* arrayToString (ArrayIValue *obj_ptr) {
	char** strs = malloc(sizeof(char*) * obj_ptr->length);
	int size_of_str = 1; //opening brace
	for (int i = 0; i < obj_ptr->length; ++i) {
		strs[i] = toString(obj_ptr->slots[i]);
		size_of_str += strlen(strs[i]);
		if (i) {
			size_of_str += 1; //space
		}
	}
	size_of_str += 1; // closing brace
	size_of_str += 1; // null at end of string

	char *result = malloc(size_of_str * sizeof(char));
	*result = '\0';

	char* t = result;
	t = strcat(t, "[");
	for (int i = 0; i < obj_ptr->length; ++i) {
		t = strcat(t, strs[i]);
	}
	t = strcat(t, "]");
	return result;
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

//============================================================
//===================== VECTORS ==============================
//============================================================

Vector2* make_vector_2 () {
	Vector2* v = (Vector2*)malloc(sizeof(Vector2));
	v->size = 0;
	v->capacity = 8;
	v->array = malloc(sizeof(intptr_t) * v->capacity);
	return v;
}

void vector_ensure_capacity_2 (Vector2* v, int c) {
	if (v->capacity < c) {
		int c2 = max(v->capacity * 2, c);
		intptr_t* a2 = malloc(sizeof(intptr_t) * c2);
		memcpy(a2, v->array, sizeof(intptr_t) * v->size);
		free(v->array);
		v->capacity = c2;
		v->array = a2;
	}
}

void vector_set_length_2 (Vector2* v, int len, intptr_t x) {
	if (len < 0) {
		printf("Negative length given to vector.\n");
		exit(-1);
	}
	if (len <= v->size) {
		v->size = len;
	} else {
		while (v->size < len)
			vector_add_2(v, x);
	}
}

void vector_add_2 (Vector2* v, intptr_t val) {
	vector_ensure_capacity_2(v, v->size + 1);
	v->array[v->size] = val;
	v->size++;
}

intptr_t vector_pop_2 (Vector2* v) {
	if (v->size == 0) {
		printf("Pop from empty vector.\n");
		exit(-1);
	}
	v->size--;
	return v->array[v->size];
}

intptr_t vector_peek_2 (Vector2* v) {
	if (v->size == 0) {
		printf("Peek from empty vector.\n");
		exit(-1);
	}
	return v->array[v->size - 1];
}

void vector_clear_2 (Vector2* v) {
	v->size = 0;
}

void vector_free_2 (Vector2* v) {
	free(v->array);
	free(v);
}

intptr_t vector_get_2 (Vector2* v, int i) {
	if (i < 0 || i >= v->size) {
		printf("Index %d out of bounds.\n", i);
		exit(-1);
	}
	return v->array[i];
}

void vector_set_2 (Vector2* v, int i, intptr_t x) {
	if (i < 0 || i > v->size) {
		printf("Index %d out of bounds.\n", i);
		exit(-1);
	} else if (i == v->size) {
		vector_add_2(v, x);
	} else {
		v->array[i] = x;
	}
}

