#include <sys/timeb.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include "utils.h"
#include "bytecode.h"
#include "vm.h"

typedef enum {
	LIT_NULL_OP = DROP_OP+1, // just a tag
	//PRINTF_OP, // a tag and str index
	//OBJ_OP, // a tag and a class index
	//TODO: consider using static object slot get/set
	//GET_SLOT_OP, // a tag and a str index
	//SET_SLOT_OP, // a tag and a str index
	//GET_GSLOT_OP, // a tag and a global var index
	//SET_GSLOT_OP,  // a tag and a global var index
	//GET_LSLOT_OP,  // a tag and a local var index(same?)
	//SET_LSLOT_OP,  // a tag and a local var index (same?)
	//CALL_SLOT_OP,  // a tag and a str index
	//TODO: consider static call slot - for methods names
	// that only refer to one implementation
	//CALL_OP,  // a tag and a code index
	//BRANCH_OP, // check if null before going.  a tag and a code index
	//GOTO_OP, // a tag and a code index
	//ARRAY_OP, // a tag
	//OBJECT_OP,  // a tag and a class index
	//RETURN_OP, // a tag
	//DROP_OP, // a tag
} QOpCode;


typedef union {
	ByteIns i;
	LitIns li;
	PrintfIns pi;
	ObjectIns oi;
	SlotIns si;
	SetSlotIns ssi;
	CallSlotIns csi;
	CallIns ci;
	SetLocalIns sli;
	GetLocalIns gli;
	SetGlobalIns sgi;
	GetGlobalIns ggi;
	BranchIns bi;
	GotoIns gi;
} AllIns;

typedef struct {
	AllIns i;
	int locals;
} AllInsData;

//============================================================
//=============== BASIC DATA STRUCTURE =======================
//============================================================

typedef struct {
	int idx;
	void* ptr;
} PtrPair;

typedef struct {
	int name;
	int value;
} IntPair;

typedef struct{
	int num_methods;
	int num_slots;
	IntPair slots_and_methods[0];
} ClassLayout;


typedef enum {
  INT_OBJ,
  NULL_OBJ,
  ARRAY_OBJ,
  BROKEN_HEART,
  OBJ_OBJ,
} ObjTag;

/* reuse the value structure from bytecode.h */

typedef struct IValue IValue;
struct IValue {
  intptr_t _tag;
  IValue* _forward_space;
};

typedef struct { //can go on heap
  intptr_t tag;
  intptr_t null_space;
} NullIValue;

typedef struct { //can go on heap
  intptr_t tag;
  intptr_t int_value;
} IntIValue;

typedef struct { //must go on heap
  intptr_t tag;
  intptr_t length;
  IValue* slots[0];
} ArrayIValue;

typedef struct { // must go on heap
  ClassLayout* class_ptr;
  IValue* parent_obj_ptr;
  IValue* var_slots[0];
} ObjectIValue;

typedef struct Frame Frame; //forward declaration
struct Frame {
	Frame* parent;
	int return_addr;
	IValue* slots[0];
};



// operand stack operations
IValue* stack_pop ();
IValue* stack_peek ();
void stack_push (IValue* val);

// frame operations
void push_frame(int return_addr, int num_locals, int num_args);
int pop_frame();
IValue* get_local(int idx);
void set_local(int idx, IValue*);

// code section operations
ByteIns* get_ins(int i);

// byte code interpreter operations
void exec_prog (Program* p);
int exec_ins (int pc);

void exec_array_op ();
int exec_return_op ();
void exec_lit_op (LitIns* i);
void exec_lit_null_op (ByteIns* i);

void exec_set_slot_op (SetSlotIns* i);
void exec_slot_op (SlotIns* i); // get_slot
int exec_call_slot_op (CallSlotIns* i, int pc);
int exec_built_in_method(CallSlotIns* i);
// returns true if it is a built-in method and executes it

int exec_goto_op (GotoIns* i);
int exec_branch_op (BranchIns* i, int pc);

void exec_object_op (ObjectIns* i);
void exec_printf_op (PrintfIns* i);

void exec_get_local_op (GetLocalIns* i);
void exec_set_local_op (SetLocalIns* i);

void exec_get_global_op (GetGlobalIns* i);
void exec_set_global_op (SetGlobalIns* i);
int exec_call_op(CallIns* i, int pc);

// garbage collection
void * halloc(size_t size);

void* garbage_collector();
IValue* get_post_gc_ptr(IValue* obj);
void swap_heaps();
void scan_root_set();
void scan_IValue(IValue* t);
void scan_new_heap();
IValue * copyIValue(IValue* from);
size_t sizeIValue(IValue * t);

// slot related operations
IValue* get_slot_by_idx(ObjectIValue* receiver, int name_idx);
void set_slot_by_idx(ObjectIValue* receiver, int name_idx, IValue* v);
int find_method_by_name(ObjectIValue* receiver, int name_idx);

IValue* get_global_slot_by_idx (int slot_idx);
void set_global_slot_by_idx (int slot_idx, IValue* iv);
char* get_str_constant_by_idx(int idx);
ClassLayout* get_class_by_idx(int idx);

// constructors

ObjTag obj_type (IValue* o);
NullIValue* make_null_obj ();
IntIValue* make_int_obj (int value);
ArrayIValue* make_array_obj (int length, IValue* init);
IntIValue* array_length (ArrayIValue* array);
IValue* array_get (ArrayIValue* a, IntIValue* i);
NullIValue* array_set (ArrayIValue* a, IntIValue* i, IValue* v);

// built-in methods

IValue* eq(IValue* x, IValue* y);
IValue* lt(IValue* x, IValue* y);
IValue* le(IValue* x, IValue* y);
IValue* gt(IValue* x, IValue* y);
IValue* ge(IValue* x, IValue* y);
IntIValue* int_obj_add (IValue* x, IValue* y);
IntIValue* int_obj_sub (IValue* x, IValue* y);
IntIValue* int_obj_mul (IValue* x, IValue* y);
IntIValue* int_obj_div (IValue* x, IValue* y);
IntIValue* int_obj_mod (IValue* x, IValue* y);

//utils for printing
void print_ivalue(IValue* t);
void print_arrayivalue(ArrayIValue* t);

void print_stack();
void print_frame();

// assert functions
//#define DEBUG

#ifdef DEBUG
#include <execinfo.h>
#endif //DEBUG


#ifndef DEBUG
#define debugf(format, ...)((void) 0)
#define errorif(ignore, format, ...) assert(!(ignore))
#define assert_msg(ignore, format, ...) assert(ignore)
#define error(format, ...) __builtin_unreachable()
#else
#define debugf(format, ...) _debugf(format, ##__VA_ARGS__)
#define errorif(ignore, format, ...) _errorif(ignore, format, ##__VA_ARGS__)
#define assert_msg(ignore, format, ...) _assert_msg(ignore, format, ##__VA_ARGS__)
#define error(format, ...) _error(format, ##__VA_ARGS__)
#endif

void _debugf(const char * format, ...);
void _error(const char* format, ...);
void _errorif(int boolean, const char* format, ...);
void _assert_msg(int boolean, const char* format, ...);
void v_errorif(int boolean, const char* format, va_list va);

// safe conversions
IntIValue* to_int_val (IValue* val);
NullIValue* to_null_val(IValue* val);
ObjectIValue* to_obj_val(IValue* val);
ArrayIValue* to_array_val(IValue* val);

int to_int (IntIValue* val);
IValue* from_int_val(IntIValue* val);
IValue* from_null_val(NullIValue* val);
IValue* from_obj_val(ObjectIValue* val);
IValue* from_array_val(ArrayIValue* val);

intptr_t _get_tag(IValue* v);
void set_tag(IValue* v, ObjTag o);
IValue* get_forward_ptr(IValue* v);
void set_forward_ptr(IValue* v, IValue* c);

void print_code (int pc);
void print_objectivalue (ObjectIValue* t);

int INT_ADD_NAME = -1;
int INT_SUB_NAME = -1;
int INT_MUL_NAME = -1;
int INT_DIV_NAME = -1;
int INT_MOD_NAME = -1;
int INT_GT_NAME = -1;
int INT_GE_NAME = -1;
int INT_LT_NAME = -1;
int INT_LE_NAME = -1;
int INT_EQ_NAME = -1;
int ARRAY_LENGTH_NAME = -1;
int ARRAY_SET_NAME = -1;
int ARRAY_GET_NAME = -1;



#define SLOT_ITEM -3
intptr_t CLEAR_ARRAY_OBJ_MASK = (intptr_t)(((intptr_t)-1)<<3);
intptr_t ARRAY_OBJ_MASK = ((intptr_t)1);

IValue* get_slot_by_idx(ObjectIValue* receiver, int name_idx){
	ClassLayout* cl = receiver->class_ptr;
	int slot_idx = 0;
	for(int i = 0; i < cl->num_slots+cl->num_methods; ++i){
		if(cl->slots_and_methods[i].value != SLOT_ITEM){
			continue;
		}
		debugf("Reading slot with addr %lx\n", receiver->var_slots[slot_idx]);
		if(cl->slots_and_methods[i].name == name_idx){
			return receiver->var_slots[slot_idx];
		}
		slot_idx +=1;
	}
	debugf("Receiver's parent has tag %d\n", obj_type(receiver->parent_obj_ptr));
	return get_slot_by_idx(to_obj_val(receiver->parent_obj_ptr), name_idx);
}

void set_slot_by_idx(ObjectIValue* receiver, int name_idx, IValue* v){
	ClassLayout* cl = receiver->class_ptr;
	int slot_idx = 0;
	for(int i = 0; i < cl->num_slots+cl->num_methods; ++i){
		if(cl->slots_and_methods[i].name == name_idx){
			receiver->var_slots[slot_idx] = v;
			return;
		}
		if(cl->slots_and_methods[i].value == SLOT_ITEM){
			slot_idx +=1;
		}
	}
	return set_slot_by_idx(to_obj_val(receiver->parent_obj_ptr), name_idx, v);
}

int find_method_by_name(ObjectIValue* receiver, int name_idx){
	ClassLayout* cl = receiver->class_ptr;
	for(int i = 0; i< cl->num_methods+cl->num_slots; ++i){
		if(cl->slots_and_methods[i].name == name_idx){
			return cl->slots_and_methods[i].value;
		}
	}
	return find_method_by_name(to_obj_val(receiver->parent_obj_ptr), name_idx);
}

#define GLOBAL_SIZE 100
IValue* globals[GLOBAL_SIZE];
int max_globals = 0;
IValue* get_global_slot_by_idx (int slot_idx) {
	return globals[slot_idx];
}

void set_global_slot_by_idx(int slot_idx, IValue* iv){
	globals[slot_idx] = iv;
}

void init_global_slots(int size){
	max_globals = size;
	for(int i = 0; i < max_globals; ++i){
		globals[i] = from_null_val(make_null_obj());
	}

}


#define STACK_SIZE 1000
IValue* stack[STACK_SIZE];
int stack_top = 0;
void stack_push (IValue * val) {
	debugf("Stack push height from %d with addr %lx" , stack_top, val);
	stack[stack_top] = val;
	stack_top+=1;
	debugf("to %d with tag %d\n", stack_top, obj_type(val));
}

IValue* stack_pop () {
	debugf("Stack popping from %d at %lx ", stack_top-1, stack[stack_top-1]);
	IValue* v = stack[stack_top-1];
	stack_top-=1;
	debugf("with tag %d\n", obj_type(v));
	return v;
}

IValue* stack_peek () {
  return stack[stack_top-1];
}


#define FRAME_SIZE 1024*1024
char frames[FRAME_SIZE];
Frame* current = NULL;
int frame_top = 0;
Frame* frame_alloc(int slots){
	size_t frame_size = sizeof(Frame)+slots*sizeof(IValue*);
	Frame* t = &frames[frame_top];
	frame_top+=frame_size;
	errorif(frame_top >= FRAME_SIZE, "Ran out of frame space!");
	return t;
}

void push_frame(int return_addr, int num_locals, int num_args){
	debugf("Called push frame with return address = %d!\n", return_addr);
	Frame* next = frame_alloc(num_locals+num_args);
	next->return_addr = return_addr;
	next->parent = current;
	current = next;
	for(int i = 0; i < (num_locals+num_args); ++i){
		set_local(i, NULL); // to prevent a very confused GC
	}
	for(int i = 0; i < (num_locals+num_args); ++i){
		set_local(i, from_null_val(make_null_obj()));
	}
}

int pop_frame(){
	debugf("Called pop frame and will return with parent frame = %lx\n", current->parent);
	int ret = current->return_addr;

	int size_of_current_frame = (frames+frame_top)-(char*)current;
	frame_top -= size_of_current_frame;
	debugf("Called pop frame and will return with return addr = %d at %lx\n", ret, current->parent);
	current = current->parent;
	return ret;
}

IValue* get_local(int idx){
	debugf("Getting slot %d from frame to item with tag %d\n", idx, obj_type(current->slots[idx]));
	return current->slots[idx];
}

void set_local(int idx, IValue* v){
	debugf("Setting slot %d in frame to item with tag %d\n", idx, v?obj_type(v):-1);
	current->slots[idx] = v;
}

//============================================================
//========== GLOBAL DATA STRUCTURE OPERATIONS ================
//============================================================

#define HEAP_SIZE 1024*1024
char heap1[HEAP_SIZE];
char heap2[HEAP_SIZE];
char (*heap_cur)[HEAP_SIZE] = &heap1;
char (*heap_old)[HEAP_SIZE] = &heap2;
int next_free_cur = 0;
int next_free_old = 0;
int is_currently_collecting = 0;

void * _halloc(size_t size){
	char* heap = *heap_cur;
	void * ret = &heap[next_free_cur];
	next_free_cur += size;
	if(next_free_cur >= HEAP_SIZE){
		if(is_currently_collecting){
			error("Ran out of heap while collecting garbage!\n");
		}
		garbage_collector();
		// redo the allocation
		return _halloc(size);
	}
	debugf("Allocating %lx for size %d\n", ret, size);
	return ret;
}

void * halloc(size_t size){
	//garbage_collector();
	return _halloc(size);
}

void swap_heaps(){
	char (*heap_tmp)[HEAP_SIZE] = heap_cur;
	debugf("heap_cur %lx, heap_old %lx\n", heap_cur, heap_old);
	heap_cur = heap_old;
	heap_old = heap_tmp;
	debugf("heap_cur %lx, heap_old %lx\n", heap_cur, heap_old);

	next_free_old = next_free_cur;
	next_free_cur = 0; // current heap is empty
}

double gc_time=0.0;
void* garbage_collector(){
  struct timeval total_start,start, end;
		gettimeofday(&start, NULL);
	if(is_currently_collecting){
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
		gc_time+= (end.tv_sec * 1000.0 + end.tv_usec/1000.0) - (start.tv_sec * 1000.0 + start.tv_usec/1000.0);
#ifdef DEBUG
	printf("\n-----------\n");
	print_frame();
	print_stack();
	printf("\n-----------\n");
#endif
}

size_t sizeIValue(IValue * t){
	switch(obj_type(t)){
		case INT_OBJ:
			return sizeof(IntIValue);
		case NULL_OBJ:
			return sizeof(NullIValue);
		case ARRAY_OBJ:{
			ArrayIValue* a = to_array_val(t);
			return sizeof(ArrayIValue) + a->length*sizeof(IValue*);
		}
		case OBJ_OBJ:{
			ObjectIValue* o = to_obj_val(t);
			int num_slots = o->class_ptr->num_slots;
			return sizeof(ObjectIValue)+sizeof(IValue*)*(num_slots);
		}
		default:
			debugf("Has tag %d\n", obj_type(t));
			error("Unexpected IValue type to iterate through!\n");
	}
}

IValue * copyIValue(IValue* from){
	size_t size = sizeIValue(from);
	IValue * t = _halloc(size);
	memcpy(t, (void*)(((intptr_t)from) & CLEAR_ARRAY_OBJ_MASK), size);
	return (IValue*)(((uintptr_t)t)|ARRAY_OBJ_MASK);
}

IValue* get_post_gc_ptr(IValue* obj){
	if(!obj){
		debugf("Cannot transfer NULL PTR\n");
		return NULL;
	}

	// Due to tagging, we do not need to transfer these
	if(obj_type(obj) == NULL_OBJ || obj_type(obj) == INT_OBJ){
		return obj;
	}

	char* old_heap = *heap_old;
	if((uintptr_t)obj >= (uintptr_t)&old_heap[HEAP_SIZE] || (uintptr_t)obj < (uintptr_t)&old_heap[0]){
		//TODO: fix this - the reason we cannot fail here is becase
		// we intermix constants and globals (so when we fix a global
		// we may later hit it as a constant)
		return obj;
	}


	if(obj_type(obj) == BROKEN_HEART){
		return get_forward_ptr(obj);
	}


	IValue* new_obj = copyIValue(obj);
	set_tag(obj, BROKEN_HEART);
	set_forward_ptr(obj, new_obj);

	debugf("Moving from ptr %lx to %lx\n", obj, new_obj);
	return new_obj;
}

void scan_root_set(){
	char *heap = *heap_old;
	char *heap_curs = *heap_cur;
	// scan through global variables
	debugf("Heap is %lx to %lx\n", &heap[0], &heap[HEAP_SIZE]);
	debugf("NEW Heap is %lx to %lx\n", &heap_curs[0], &heap_curs[HEAP_SIZE]);
	debugf("Global slots!\n");

	for(int i = 0; i < max_globals; ++i){
		set_global_slot_by_idx(i,
			get_post_gc_ptr(get_global_slot_by_idx(i)));
	}

	// scan through operand stack
	debugf("Operand Stack!\n");
	for(int i = 0; i < stack_top; ++i){
		stack[i] = get_post_gc_ptr(stack[i]);
	}

	debugf("Frames!\n");
	Frame* cur_frame = current;
	uintptr_t last = (uintptr_t) &frames[frame_top];
	while(cur_frame != NULL){
		for(int i = 0; (uintptr_t)(cur_frame->slots+i)< last; ++i){
			//print_ivalue(cur_frame->slots[i]); printf(" = before\n");
			cur_frame->slots[i] = get_post_gc_ptr(cur_frame->slots[i]);
			//print_ivalue(cur_frame->slots[i]); printf(" = after\n");
		}
		last = (uintptr_t)cur_frame;
		cur_frame = cur_frame->parent;
	}
}


void scan_new_heap(){
	char *heap = *heap_cur;
	// scan through global variables
	debugf("Heap is %lx to %lx\n", &heap[0], &heap[HEAP_SIZE]);
	debugf("Current Heap Scan!\n");
	int position = 0;
	while(position < next_free_cur){
		IValue* iv = (IValue*) &heap[position];
		debugf("Working on %lx\n", iv);
		position += sizeIValue(iv);
		scan_IValue(iv);
	}
}

void scan_IValue(IValue* t){
	switch(obj_type(t)){
		case INT_OBJ:
		case NULL_OBJ:
			debugf("Either null or int!\n");
			return; // no child items
		case ARRAY_OBJ:{
			ArrayIValue* a = to_array_val(t);
			for(int i = 0; i < a->length; ++i){
				debugf("Moving array ptr %lx to ", a->slots[i]);
				a->slots[i] = get_post_gc_ptr(a->slots[i]);
				debugf("%lx\n", a->slots[i]);
			}
			return;
		}
		case OBJ_OBJ:{
			ObjectIValue* o = to_obj_val(t);
			int num_slots = o->class_ptr->num_slots;
			for(int i = 0; i < num_slots; ++i){
				//print_objectivalue(o);
				debugf("Moving object slot %lx to ", o->var_slots[i]);
				o->var_slots[i] = get_post_gc_ptr(o->var_slots[i]);
				debugf("%lx\n", o->var_slots[i]);
			}
			debugf("Moving parent ptr %lx to ", o->parent_obj_ptr);
			o->parent_obj_ptr = get_post_gc_ptr(o->parent_obj_ptr);
			debugf("%lx\n", o->parent_obj_ptr);
			return;
		}
		default:
			debugf("Has tag:%d\n", obj_type(t));
			error("Unexpected IValue type to get size of!\n");
	}
}


//============================================================
//=============== BYTE CODE INTERPRETER ======================
//============================================================

// Sets the i’th slot in the current
// local frame to the top value in
// the operand stack.
void exec_set_local_op (SetLocalIns * i) {
	set_local(i->idx, stack_peek());
}

// Retrieves the i’th slot in the
// current local frame and pushes
// it onto the operand stack.
void exec_get_local_op (GetLocalIns * i) {
	stack_push(get_local(i->idx));
}


// Sets the global variable with name
// specified by the String object at index
// i to the top value in the operand stack.
void exec_set_global_op (SetGlobalIns * i) {
	set_global_slot_by_idx(i->name, stack_peek());
}


// Retrieves the value of the
// global variable with name specified
// by the String object at index i, and
// pushes it onto the operand stack.
void exec_get_global_op (GetGlobalIns * i) {
	stack_push(get_global_slot_by_idx(i->name));
}

// Pops a value from the operand
// stack. If this value is not Null,
// then sets the instruction pointer to
// the instruction address associated
// with the name given by the String
// object at index i.
int exec_branch_op (BranchIns * i, int pc) {
  IValue* val = stack_pop();
  debugf("Received %lx from stack bottom: %lx\n", val, &stack[0]);
  if (obj_type(val) != NULL_OBJ) {
    return i->name;
  } else {
    return pc+1;
  }
}


// Sets the instruction pointer to the instruction
// address associated with the name given by
// the String object at index i.
int exec_goto_op (GotoIns * i) {
	return i->name;
}

// Pops and discards the top value from
// the operand stack.
void exec_drop_op () {
  stack_pop();
}

void exec_lit_op (LitIns * i) { // changed the semantics
	stack_push(from_int_val(make_int_obj(i->idx)));
}

void exec_lit_null_op (ByteIns * i) {
	stack_push(from_null_val(make_null_obj()));
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
  IValue* lengthi = stack_pop();

  IntIValue* len = to_int_val(lengthi);

  int length = to_int(len);
  stack_push(init_value); // for safety

  ArrayIValue* t = halloc(sizeof(ArrayIValue)+sizeof(IValue*)*length);
  t->tag = ARRAY_OBJ;
  t->length = length;
  init_value = stack_pop(); // for safety
  for (int i = 0; i < length; ++i) {
	  t->slots[i] = init_value;
  }

  stack_push(from_array_val(t));
}

// Pops n values from the operand
// stack, and then prints them out according to the given format string.
// Arguments are spliced in from the deepest value in the stack (last
// popped) to the shallowest value in the stack (first popped). Null is
// then pushed onto the operand stack.
void exec_printf_op (PrintfIns * i) {
	static const char replace = '~';
	int format_idx = i->format;
	int arity = i->arity;
	char* format_str = get_str_constant_by_idx(format_idx);
	debugf("In printf with %s\n", format_str);
	char* start_for_print = format_str;

	// this is a little dirty
	int from_stack = stack_top - arity;
	int str_len = strlen(format_str);
	for(int i = 0; i< str_len; ++i){
		if(format_str[i] == replace){
			format_str[i] = '\0';
			printf("%s", start_for_print);
			print_ivalue(stack[from_stack++]);
			start_for_print = format_str+i+1;
			format_str[i]= replace;
		}
	}
	// if there are no ~, we do not print in the loop
	// here we must print then.
	if(start_for_print < format_str+str_len){
		printf("%s", start_for_print);
	}

	for(int i = 0; i < arity; ++i){
		stack_pop();
	}
	stack_push(from_null_val(make_null_obj()));
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
	ClassLayout * cl = get_class_by_idx(i->class);

	// init new object value
	debugf("Making object\n");
	ObjectIValue* obj = halloc(sizeof(ObjectIValue)+sizeof(IValue*)*cl->num_slots);

	debugf("Num slots expected: %d\n", cl->num_slots);
	for (int i = cl->num_slots-1; i >= 0 ; --i) {
		obj->var_slots[i] = stack_pop();
	}
	obj->parent_obj_ptr = stack_pop();
	obj->class_ptr = cl;
	stack_push(from_obj_val(obj));
}

// Pops a value from the operand
// stack assuming it is an object. Retrieves the value in the object stored
// at the variable slot with name given by the String object at index i,
// and pushes it onto the operand stack.
void exec_slot_op (SlotIns* i) {
	stack_push(get_slot_by_idx(to_obj_val(stack_pop()), i->name));
}

// Pops the value to store, x, from
// the operand stack, and then pops the object to store it into. Stores
// x into the object at the variable slot with name given by the String
// object at index i. x is then pushed onto the operand stack.
void exec_set_slot_op (SetSlotIns * i) {
	IValue* val = stack_pop();
	set_slot_by_idx(to_obj_val(stack_pop()), i->name, val);
	stack_push(val);
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
// TODO: finish
int exec_call_op (CallIns * i, int pc) {
	AllInsData* aid = (AllInsData*) get_ins(i->name);
	push_frame(pc+1, aid->locals, i->arity);
	for(int j = i->arity-1; j >= 0; --j ){
		IValue* v = stack_pop();
		set_local(j, v);
	}
	return i->name;
}

// Registers the parent frame of the
// current local frame as the new current
// frame. Execution proceeds by
// setting the instruction pointer to the
// return address stored in the current
// local frame. The local frame is no longer
// used after a Return instruction, and any
// storage allocated for it may be reclaimed.
int exec_return_op () {
	int ret = pop_frame();
	debugf("Returning with frame = %lx and address = %d\n", current, ret);
	return ret;
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
int exec_call_slot_op (CallSlotIns * i, int pc) {
	if(exec_built_in_method(i)){
		debugf("Was the built-in %s method!\n", get_str_constant_by_idx(i->name));
		#ifdef DEBUG
		debugf("Result is"); print_ivalue(stack_peek());
		#endif
		return pc+1;
	}
	ObjectIValue* receiver_ptr = to_obj_val(stack[stack_top-i->arity]);

	int method_idx = find_method_by_name(receiver_ptr, i->name);
	AllInsData* aid = (AllInsData*) get_ins(method_idx);
	push_frame(pc+1, aid->locals, i->arity);
	for(int j = max(i->arity-1,0); j >= 0; --j ){
		set_local(j, stack_pop());
	}
	debugf("Going to method at %d\n", method_idx);
	return method_idx;
}

int exec_built_in_method(CallSlotIns* i){
	debugf("stack_top:%d arity:%d\n", stack_top, i->arity);
	IValue* receiver_ptr = stack[stack_top - i->arity];
	debugf("Working with type:%d\n", obj_type(receiver_ptr));
	int arity = max(i->arity-1, 0);
	int method_name = i->name;
	switch (obj_type(receiver_ptr)) {
		case INT_OBJ: {
			assert_msg(arity == 1, "Not enough arguments!\n");
			IValue* arg = stack_pop();
			assert_msg(obj_type(arg) == INT_OBJ, "Wrong argument type!");
			stack_pop();
			if (method_name == INT_ADD_NAME) {
				stack_push(from_int_val(int_obj_add(receiver_ptr, arg)));
				return 1;
			} else if (method_name == INT_SUB_NAME) {
				stack_push(from_int_val(int_obj_sub(receiver_ptr, arg)));
				return 1;
			} else if (method_name == INT_MUL_NAME) {
				stack_push(from_int_val(int_obj_mul(receiver_ptr, arg)));
				return 1;
			} else if (method_name == INT_DIV_NAME) {
				stack_push(from_int_val(int_obj_div(receiver_ptr, arg)));
				return 1;
			} else if (method_name == INT_MOD_NAME) {
				stack_push(from_int_val(int_obj_mod(receiver_ptr, arg)));
				return 1;
			} else if (method_name == INT_GT_NAME) {
				stack_push(gt(receiver_ptr, arg));
				return 1;
			} else if (method_name == INT_GE_NAME) {
				stack_push(ge( receiver_ptr, arg));
				return 1;
			} else if (method_name == INT_LT_NAME) {
				stack_push(lt(receiver_ptr, arg));
				return 1;
			} else if (method_name == INT_LE_NAME) {
				stack_push(le(receiver_ptr, arg));
				return 1;
			} else if (method_name == INT_EQ_NAME) {
				stack_push(eq(receiver_ptr, arg));
				return 1;
			}
			error("unknown native int function %s\n", get_str_constant_by_idx(method_name));
		}
		case ARRAY_OBJ: {
			if(method_name == ARRAY_LENGTH_NAME){
				stack_pop();
				assert(arity == 0);
				stack_push(from_int_val(array_length(to_array_val(receiver_ptr))));
				return 1;
			} else if(method_name == ARRAY_SET_NAME){
				assert(arity == 2);
				IValue* arg2 = stack_pop();
				IValue* arg1 = stack_pop();
				stack_pop();
				stack_push(from_null_val(array_set(to_array_val(receiver_ptr), to_int_val(arg1), arg2)));
				return 1;
			} else if(method_name == ARRAY_GET_NAME){
				assert(arity == 1);
				IValue* arg1 = stack_pop();
				stack_pop();
				stack_push(array_get(to_array_val(receiver_ptr), to_int_val(arg1)));
				return 1;
			}
			error("unknown native array function %s\n",
				get_str_constant_by_idx(method_name));
		}
		default:
			return 0;
	}
	error("Wat?");
}

int exec_ins (int pc) {
	ByteIns * ins = get_ins(pc);
  switch (ins->tag) {
  case LIT_OP: {
    LitIns* i = (LitIns*)ins;
    exec_lit_op(i);
    break;
  }
  case LIT_NULL_OP: {
    exec_lit_null_op(ins);
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
    return exec_call_slot_op(i, pc);
  }
  case CALL_OP: {
    CallIns* i = (CallIns*)ins;
    return exec_call_op(i, pc);
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
    return exec_branch_op(i, pc);
  }
  case GOTO_OP: {
    GotoIns* i = (GotoIns*)ins;
    return exec_goto_op(i);
  }
  case RETURN_OP: {
    return exec_return_op();
  }
  case DROP_OP: {
    exec_drop_op();
    break;
  }
  default: {
    error("Unknown instruction with tag: %u\n", ins->tag);
  }
  }
  return pc +1;
}

void start_exec(int pc) {
	CallIns ci = {CALL_OP, pc, 0};
	pc = exec_call_op(&ci, -2); // return address is incremented so -2 + 1 == -1
	while(pc != -1){
		pc = exec_ins(pc);
#ifdef DEBUG
		printf("\n\n");
		print_code(pc);
		print_stack();
		printf("\n\n");
		print_frame();
		printf("\n");
#endif
	}
}

void print_stack(){
	printf("\nOperand stack:\nTOP\n");
	for(int i = stack_top-1; i >=0; --i){
		print_ivalue(stack[i]); printf("\n");
	}
}

void print_frame(){
	printf("Frames!\n");
	const Frame* cur_frame = current;
	uintptr_t last = (uintptr_t) &frames[frame_top];

	// if last == cur_frame, then there are no frames, since frames
	// take up space
	while(cur_frame != NULL && last != (uintptr_t)cur_frame){
		printf("Frame %lx until %lx\n", cur_frame, last);
		for(int i = 0; (uintptr_t)(cur_frame->slots+i)< last; ++i){
			printf("Working on %lx at %lx\n", cur_frame->slots[i], cur_frame->slots+i);
			print_ivalue(cur_frame->slots[i]);
			printf("\n");
		}
		printf("Return addr: %d\n", cur_frame->return_addr);
		printf("Parent frame: %lx\n", cur_frame->parent);
		last = (uintptr_t)cur_frame;
		cur_frame = cur_frame->parent;
	}
}

int get_global_var_by_idx(int position, Program *p){
	int slot_val = 0;
	for(int i = 0; i < p->slots->size; ++i){
		int value = vector_get(p->slots, i);
		Value* v = vector_get(p->values, value);
		if(v->tag != SLOT_VAL){
			continue;
		}
		SlotValue * sv = (SlotValue*) v;
		if(sv->name == position){
			return slot_val;
		}
		slot_val += 1;
	}
	error("Could not find global!\n");
	return -1;
}



#define CLASS_SIZE 1024
char class_objs[CLASS_SIZE];

ClassLayout* get_class_by_idx(int idx){
	return &class_objs[idx];
}

int make_class(ClassValue *ci, Program *p, Vector* class_layouts){
	static int position = 0;
	int methods = 0;
	int slots = 0;
	for(int i = 0; i < ci->slots->size; ++i){
		Value* v = vector_get(p->values, (int) vector_get(ci->slots, i));
		if(v->tag == SLOT_VAL){
			slots++;
		} else if (v->tag == METHOD_VAL){
			methods++;
		} else {
			error("Unexpected slot!");
		}
	}

	int index = position;
	size_t class_size = sizeof(ClassLayout)+(slots+methods)*sizeof(IntPair);
	ClassLayout* cl = &class_objs[position];
	cl->num_methods = methods;
	cl->num_slots = slots;
	position += class_size;
	errorif(position >= CLASS_SIZE, "Ran out of room to store class layouts!");

	for(int i = 0; i < ci->slots->size; ++i){
		Value* v = vector_get(p->values, (int) vector_get(ci->slots, i));
		if(v->tag == SLOT_VAL){
			SlotValue* sv = (SlotValue*) v;
			debugf("Working on slot with name index %d\n", sv->name);
			cl->slots_and_methods[i].name = get_str_constant_value((StringValue*) vector_get(p->values, sv->name));
			debugf("Resolved loc is %d with str %s\n", cl->slots_and_methods[slots].name, get_str_constant_by_idx(cl->slots_and_methods[slots].name));
			cl->slots_and_methods[i].value = SLOT_ITEM;
		} else if (v->tag == METHOD_VAL){
			MethodValue* mv = (MethodValue*) v;
			cl->slots_and_methods[i].name= (int)vector_get(ci->slots, i);
			cl->slots_and_methods[i].value= -1; //We need to resolve these later
		} else {
			error("Unexpected slot!");
		}
	}

	vector_add(class_layouts, cl);
	return index;
}

int get_class_constant_value(ClassValue* ci, Program * prog, Vector* class_layouts){
	static Vector * v = NULL;
	if(!v) { v = make_vector(); }
	for(int i = 0; i < v->size; ++i){
		PtrPair* p = vector_get(v, i);
		if(p->ptr == ci){
			return p->idx;
		}
	}

	PtrPair* p = malloc(sizeof(PtrPair));
	p->ptr = ci;
	p->idx = make_class(ci,prog, class_layouts);
	vector_add(v, p);
	return p->idx;
}

void init_default_str_values(char* s, int v){
	if(!strcmp(s, "add")){
		INT_ADD_NAME = v;
	} else if(!strcmp(s, "sub")){
		INT_SUB_NAME = v;
	} else if(!strcmp(s, "mul")){
		INT_MUL_NAME = v;
	} else if(!strcmp(s, "div")){
		INT_DIV_NAME = v;
	} else if(!strcmp(s, "mod")){
		INT_MOD_NAME = v;
	} else if(!strcmp(s, "gt")){
		INT_GT_NAME = v;
	} else if(!strcmp(s, "ge")){
		INT_GE_NAME = v;
	} else if(!strcmp(s, "lt")){
		INT_LT_NAME = v;
	} else if(!strcmp(s, "le")){
		INT_LE_NAME = v;
	} else if(!strcmp(s, "eq")){
		INT_EQ_NAME = v;
	} else if(!strcmp(s, "length")){
		ARRAY_LENGTH_NAME = v;
	} else if(!strcmp(s, "set")){
		ARRAY_SET_NAME = v;
	} else if(!strcmp(s, "get")){
		ARRAY_GET_NAME = v;
	}
}

Vector* strs = NULL;
char* get_str_constant_by_idx(int idx){
	return vector_get(strs, idx);
}

int get_str_constant_value(StringValue* si){
	if(!strs){
		strs = make_vector();
	}
	for(int i = 0; i < strs->size; ++i){
		if(si->value == vector_get(strs, i)){
			return i;
		}
	}
	vector_add(strs, si->value);
	init_default_str_values(si->value, strs->size-1);
	return strs->size-1;
}


#define CODE_SIZE 1024*10
AllInsData code_data[CODE_SIZE];
int code_index(void* ptr){
	int ret = (AllInsData*)ptr - &code_data[0];
	return (int)ret;
}

int end_code_section=0;
void * code_alloc(){
	void* ptr = &code_data[end_code_section];
	end_code_section++;
	errorif(end_code_section >= CODE_SIZE, "Ran out of code space!");
	return ptr;
}

ByteIns * get_ins(int i){
	return (ByteIns*) &code_data[i];
}

int make_code_ins(ByteIns* ins, Program* p, Vector* goto_branch, Vector* call_ins, Vector* class_layouts){
	debugf("In make code ins\n");
	Vector* cp = p->values;
	int ret = -1;
	switch(ins->tag){
		case LABEL_OP:
			return -1;
		case LIT_OP:{
			LitIns* lins = (LitIns*) ins;
			Value* v = vector_get(cp, lins->idx);
			if(v->tag == INT_VAL){
				IntValue* iv = (IntValue*) v;
				LitIns* bi = code_alloc();
				bi->tag = LIT_OP;
				bi->idx = iv->value;
				return code_index(bi);
			} else if(v->tag == NULL_VAL){
				ByteIns* bi = code_alloc();
				bi->tag = LIT_NULL_OP;
				return code_index(bi);
			}

			error("Bad literal value!");
		}
		case PRINTF_OP:{
			PrintfIns* pi = code_alloc();
			PrintfIns* opi = (PrintfIns*) ins;
			pi->format =  get_str_constant_value((StringValue*) vector_get(cp, opi->format));
			pi->arity = ((PrintfIns*)ins)->arity;
			pi->tag = PRINTF_OP;
			return code_index(pi);
		}
		case ARRAY_OP:
		case DROP_OP:
		case RETURN_OP:{
			ByteIns* ii = code_alloc();
			ii->tag = ins->tag;
			return code_index(ii);
		}
		case OBJECT_OP:{
			ObjectIns* oi = code_alloc();
			ObjectIns* ooi = (ObjectIns*) ins;
			oi->class = get_class_constant_value((ClassValue*) vector_get(cp, ooi->class), p, class_layouts);
			oi->tag = ooi->tag;
			return code_index(oi);
		}
		case SET_SLOT_OP: // these happen to be the same
		case SLOT_OP: {
			assert(sizeof(SlotIns) == sizeof(SetSlotIns));
			SlotIns* si = code_alloc();
			SlotIns* osi = (SlotIns*) ins;
			si->tag = osi->tag;
			si->name = get_str_constant_value((StringValue*) vector_get(cp, osi->name));
			return code_index(si);
		}
		case CALL_SLOT_OP:{
			CallSlotIns* si = code_alloc();
			CallSlotIns* osi = (CallSlotIns*) ins;
			si->tag = osi->tag;
			si->name = get_str_constant_value((StringValue*) vector_get(cp, osi->name));
			si->arity = osi->arity;
			return code_index(si);
		}
		case CALL_OP: {
			CallIns* ci = code_alloc();
			CallIns* oci = (CallIns*) ins;
			ci->tag = oci->tag;
			ci->arity = oci->arity;
			ci->name = oci->name;
			// needs to be fixed
			vector_add(call_ins, ci);
			return code_index(ci);
		}// *LOCAL_OP do not change
		case GET_LOCAL_OP:
		case SET_LOCAL_OP:{
			assert(sizeof(SetLocalIns) == sizeof(GetLocalIns));
			GetLocalIns* gi = code_alloc();
			GetLocalIns* ogi= (GetLocalIns*) ins;
			gi->tag = ogi->tag;
			gi->idx = ogi->idx;
			return code_index(gi);
		}
		case GET_GLOBAL_OP:
		case SET_GLOBAL_OP:{
			assert(sizeof(SetGlobalIns) == sizeof(GetGlobalIns));
			GetGlobalIns* gi = code_alloc();
			GetGlobalIns* ogi= (GetGlobalIns*) ins;
			gi->tag = ogi->tag;
			gi->name = get_global_var_by_idx(ogi->name, p);
			return code_index(gi);
		}
		// GLOBAL_OP's the slots need to be resolved to an index
		// BRANCH & GOTO are just copied
		case BRANCH_OP:
		case GOTO_OP:{
			assert(sizeof(GotoIns) == sizeof(BranchIns));
			GotoIns* gi = code_alloc();
			GotoIns* ogi = (GotoIns*) ins;
			gi->tag = ogi->tag;
			gi->name = ogi->name;
			vector_add(goto_branch, gi);
			return code_index(gi);
		}
		// DROP and RETURN are done above

		default:
			error("Bad ins tag!");
	}
	return -1;
}


// we need two passes
// once to get label strings to label code points
int make_code(MethodValue* mv, Program* p, Vector* call_ins, Vector * class_layouts){
	debugf("In make code\n");
	Vector* constant_idx_to_label_idx = make_vector();
	Vector* goto_or_branch_todo = make_vector();

	int label_index = -1;
	int first_entry = -1;
	int t = -1;
	for(int i = 0; i < mv->code->size; ++i){
		ByteIns* ins = vector_get(mv->code, i);

		t = make_code_ins(ins, p, goto_or_branch_todo, call_ins, class_layouts);
		debugf("Finished make code ins with %d\n", t);
		if(first_entry == -1){
			first_entry = t;
		}

		if(t == -1 && label_index == -1){ // if the ins was a label
			label_index = i;
		} else if (t != -1 && label_index != -1){ // if we have a "real" instruction and labels to resolve
			for(int j = label_index; j < i; ++j){
				//assign all labels to t
				LabelIns* lins = vector_get(mv->code,j);
				errorif(lins->tag != LABEL_OP, "Unexpected label tag!\n");
				IntPair* p = malloc(sizeof(IntPair));
				p->name = lins->name;
				p->value = t;
				vector_add(constant_idx_to_label_idx, p);
			}
			label_index = -1;
		}
	}

	for(int i = 0; i < goto_or_branch_todo->size; ++i){
		// if it is a branch or goto instruction
		// change the name idx
		ByteIns* ins = vector_get(goto_or_branch_todo, i);

		errorif(ins->tag != BRANCH_OP && ins->tag != GOTO_OP, "Unexpected tag for goto or branch!");

		assert(sizeof(GotoIns) == sizeof(BranchIns));

		GotoIns* gins = (GotoIns*) ins;
		int code_point = -1;
		for(int i = 0; i < constant_idx_to_label_idx->size; ++i){
			IntPair* p = vector_get(constant_idx_to_label_idx, i);
			if(p->name == gins->name){
				code_point = p->value;
			}
		}
		errorif(code_point == -1, "Unresolved label!");
		gins->name = code_point;
	}
	// free vector stuff
	return first_entry;
}

int quicken(Program * p){
	debugf("In quicken\n");
	int entry_point = -1;
	// classes need string_idx to method_entry table
	// slot method calls need string_idx
	// otherwise everything else just needs few tables

	Vector* constant_idx_to_method = make_vector();
	Vector* call_instr_to_redo = make_vector();
	Vector* class_layouts = make_vector(); // we must rewrite the int pairs in the layout to be method indexes

	for(int i = 0; i < p->values->size; ++i){
		Value* v = vector_get(p->values, i);
		switch(v->tag){
			case INT_VAL:
			case NULL_VAL:
			case STRING_VAL:
			case SLOT_VAL: // this is tricky
			case CLASS_VAL:
				// we skip
				break;
			case METHOD_VAL:{
				MethodValue* mv = (MethodValue*) v;
				int entry = make_code(mv, p, call_instr_to_redo, class_layouts);

				// so we can get the frame
				AllInsData* aid = (AllInsData*)get_ins(entry);
				aid->locals = mv->nlocals;

				debugf("entry:%d\n", entry);
				IntPair* pp = malloc(sizeof(IntPair));
				pp->value = entry;
				pp->name = i;
				vector_add(constant_idx_to_method, pp);
				if(i == p->entry){
					entry_point = entry;
				}
				break;
			}
			default:
				error("UNKNOWN TAG!");
		}
	}

	// go through call slots and redo their destinations
	for(int i= 0; i < call_instr_to_redo->size; ++i){
		CallIns* ci = vector_get(call_instr_to_redo, i);
		int name = -1;
		for(int j = 0; j < p->slots->size; ++j){ //converts string name to constant name by going through globals
			debugf("Slot size:%d\n", p->slots->size);
			int constant_idx = (int)vector_get(p->slots, j);
			Value* v = vector_get(p->values, constant_idx);
			debugf("Global slot is %d\n", constant_idx);
			if(v->tag != METHOD_VAL){
				continue;
			}
			MethodValue* mv = (MethodValue*)v;
			if(ci->name == mv->name){
				ci->name = constant_idx;
				debugf("Global slot is %d\n", constant_idx);
				break;
			}
		}
		for(int j = 0; j < constant_idx_to_method->size; ++j){
			IntPair* p = vector_get(constant_idx_to_method, j);
			debugf("Method name index is %d\n", p->name);
			if(p->name == ci->name){
				debugf("Method index is %d\n", p->value);
				name = p->value;
				break;
			}
		}
		errorif(name == -1, "Could not find method!\n");
		ci->name = name;
	}

	// go through call slots and redo their destinations
	for(int i= 0; i < class_layouts->size; ++i){
		debugf("Class %d!\n", i);
		ClassLayout* cl = vector_get(class_layouts, i);
		for(int j = 0; j < cl->num_methods+cl->num_slots; ++j){
			if(cl->slots_and_methods[j].value == SLOT_ITEM) { //skip "var slots"
				continue;
			}
			int code_point = -1;
			debugf("Working on class %d's %d method\n", i, j);
			for(int k = 0; k < constant_idx_to_method->size; ++k){
				IntPair* p = vector_get(constant_idx_to_method, k);
				debugf("Checking method %d with name %d for %d\n", k,
						p->name, cl->slots_and_methods[j].name);
				if(p->name == cl->slots_and_methods[j].name){
					code_point = p->value;
					break;
				}
			}
			errorif(code_point == -1, "Could not find method!\n");
			MethodValue* mv = vector_get(p->values, cl->slots_and_methods[j].name);
			cl->slots_and_methods[j].name = get_str_constant_value((StringValue*)vector_get(p->values, mv->name));
			cl->slots_and_methods[j].value = code_point;
		}
	}

#ifdef DEBUG
	printf("Printing classes (total=%d)\n", class_layouts->size);
	for(int i = 0; i < class_layouts->size; ++i){
		ClassLayout* cl = vector_get(class_layouts, i);
		printf("Working on class\n");
		for(int j = 0; j < cl->num_slots; ++j){
			if(cl->slots_and_methods[j].value == SLOT_ITEM){
				printf("Has slot with name %s at %d\n", get_str_constant_by_idx(cl->slots_and_methods[j].name), cl->slots_and_methods[j].name);
			} else {
				printf("Has method with name %s at %d with entry location %d\n", get_str_constant_by_idx(cl->slots_and_methods[j].name), cl->slots_and_methods[j].name, cl->slots_and_methods[j].value);
			}
		}
	}
#endif
	return entry_point;
}

void print_code (int pc) {
	for(int j = 0; j < end_code_section; ++j){
		printf("%s | %5d |", (pc == j)?"pc->":"    ", j);
		ByteIns* i = get_ins(j);
		if(i->tag == LIT_NULL_OP){
			printf("   lit_null");
		} else {
			print_ins(i);
		}
		printf("\n");
	}
}

void exec_prog (Program * p) {
#ifdef DEBUG
	print_prog(p);
	printf("\n\n");
#endif
	int entry = quicken(p);
#ifdef DEBUG
	print_code(entry);
#endif
	int count_of_globals =0;
	for(int i = 0; i < p->slots->size; ++i){
		int value = vector_get(p->slots, i);
		Value* v = vector_get(p->values, value);
		if(v->tag != SLOT_VAL){
			continue;
		}
		count_of_globals += 1;
	}
	// need to allocate globals
	init_global_slots(count_of_globals);
	start_exec(entry);
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
IntIValue* to_int_val (IValue* val) {
  errorif(obj_type(val) != INT_OBJ, "Error: to_int_val.\n");
  return (IntIValue*)val;
}

NullIValue* to_null_val(IValue* val){
  errorif(obj_type(val) != NULL_OBJ, "Error: to_null_val.\n");
  return (NullIValue*)val;
}

ObjectIValue* to_obj_val(IValue* val){
  errorif(obj_type(val) != OBJ_OBJ, "Error: to_obj_val.\n");
  return (ObjectIValue*)(((uintptr_t)val) & CLEAR_ARRAY_OBJ_MASK);
}

ArrayIValue* to_array_val(IValue* val){
  errorif(obj_type(val) != ARRAY_OBJ, "Error: to_array_val.\n");
  return (ArrayIValue*)(((uintptr_t)val) & CLEAR_ARRAY_OBJ_MASK);
}

IValue* from_array_val(ArrayIValue* val){
	return (IValue*)(((uintptr_t)val) | ARRAY_OBJ_MASK);
}

IValue* from_obj_val(ObjectIValue* val){
	return (IValue*)(((uintptr_t)val) | ARRAY_OBJ_MASK);
}

IValue* from_int_val(IntIValue* val){
	return (IValue*)val;
}

IValue* from_null_val(NullIValue* val){
	return (IValue*)val;
}

void set_forward_ptr(IValue* v, IValue* c){
	assert_msg(obj_type(v) == ARRAY_OBJ || obj_type(v) == OBJ_OBJ, "Cannot get forward ptr for some types!");
	IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	tv->_forward_space = c;
}

IValue* get_forward_ptr(IValue* v){
	assert_msg(obj_type(v) == ARRAY_OBJ || obj_type(v) == OBJ_OBJ, "Cannot get forward ptr for some types!");
	IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	return tv->_forward_space;
}

void set_tag(IValue* v, ObjTag o){
	assert_msg(obj_type(v) == ARRAY_OBJ || obj_type(v) == OBJ_OBJ, "Cannot assign tag for some types!");
	IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	tv->_tag = o;
}

intptr_t _get_tag(IValue* v){
	IValue* tv = (((uintptr_t)v) & CLEAR_ARRAY_OBJ_MASK);
	return tv->_tag;
}

int to_int(IntIValue* val){
	intptr_t v = (intptr_t)val;
	return v>>3;
}

ObjTag obj_type (IValue * o) {
	intptr_t v = (intptr_t)o;
	if(v == 2){
		return NULL_OBJ;
	} else if (v%2 == 1){
		uintptr_t tag = _get_tag(o);
  		return tag > OBJ_OBJ? OBJ_OBJ : tag;
	} else {
		return INT_OBJ;
	}
}

NullIValue* make_null_obj () {
	return (NullIValue*)((intptr_t)2);
}

IntIValue* make_int_obj (int value) {
	return (IntIValue*) (((intptr_t) value) << 3);
}

IntIValue* array_length (ArrayIValue* array) {
  return make_int_obj(array->length);
}

NullIValue* array_set (ArrayIValue* a, IntIValue* i, IValue* v) {
  if (to_int(i) >= a->length || to_int(i) < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, to_int(i));
    exit(-1);
  }
  a->slots[to_int(i)] = v;
  return make_null_obj();
}

IValue* array_get (ArrayIValue* a, IntIValue* i) {
  if (to_int(i) >= a->length || to_int(i) < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, to_int(i));
    exit(-1);
  }
  return a->slots[to_int(i)];
}


IntIValue* int_obj_add (IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  IntIValue* v = (IntIValue*)(xi + yi);
#ifdef DEBUG
  assert_msg(to_int(v) == to_int(to_int_val(x))+ to_int(to_int_val(y)), "Math failed for 0x%lx = 0x%lx + 0x%lx\n", v, x, y);
#endif
  return v;
}

IntIValue* int_obj_sub (IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  IntIValue* v = (IntIValue*)(xi - yi);
#ifdef DEBUG
  assert_msg(to_int(v) == to_int(to_int_val(x)) - to_int(to_int_val(y)), "Math failed for 0x%lx = 0x%lx - 0x%lx\n", v, x, y);
#endif
  return v;
}

IntIValue* int_obj_mul (IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  IntIValue* v =  (IntIValue*)(xi * to_int(to_int_val(y)));
#ifdef DEBUG
  assert_msg(to_int(v) == to_int(to_int_val(x)) * to_int(to_int_val(y)), "Math failed for 0x%lx = 0x%lx * 0x%lx\n", v, x, y);
#endif
  return v;
}

IntIValue* int_obj_div (IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  IntIValue* v = make_int_obj((int)(xi/yi));
#ifdef DEBUG
  assert_msg(to_int(v) == to_int(to_int_val(x)) / to_int(to_int_val(y)), "Math failed for 0x%lx = 0x%lx / 0x%lx\n", v, x, y);
#endif
  return v;
}

IntIValue* int_obj_mod (IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  IntIValue * v = (IntIValue*)(xi % yi);
#ifdef DEBUG
  assert_msg(to_int(v) == to_int(to_int_val(x)) % to_int(to_int_val(y)), "Math failed for 0x%lx = 0x%lx %% 0x%lx\n", v, x, y);
#endif
  return v;
}

IValue* eq(IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  return x == y ?
         from_int_val(make_int_obj(0)) : from_null_val(make_null_obj());
}
IValue* lt(IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  return xi < yi ?
         from_int_val(make_int_obj(0)) : from_null_val(make_null_obj());
}
IValue* le(IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  return xi <= yi ?
         from_int_val(make_int_obj(0)) : from_null_val(make_null_obj());
}
IValue* gt(IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  return xi > yi ?
         from_int_val(make_int_obj(0)) : from_null_val(make_null_obj());
}
IValue* ge(IValue * x, IValue * y) {
  assert_msg(obj_type(x) == INT_OBJ && obj_type(y) == INT_OBJ, "Expected int arguments!\n");
  intptr_t xi = (intptr_t)x;
  intptr_t yi = (intptr_t)y;
  return xi >= yi ?
         from_int_val(make_int_obj(0)) : from_null_val(make_null_obj());
}

void print_ivalue(IValue* t){
#ifdef DEBUG
	printf("[0x%lx]", t);
#endif
	switch(obj_type(t)){
		case INT_OBJ:
			printf("%d", to_int(to_int_val(t)));
			return;
		case NULL_OBJ:
			printf("Null");
			return;
		case ARRAY_OBJ:{
			print_arrayivalue(to_array_val(t));
			return;
		}
		case OBJ_OBJ:{
#ifdef DEBUG
			print_objectivalue(to_obj_val(t));
#else
			printf("[OBJECT]");
#endif
			return;
		}
		case BROKEN_HEART:{
			printf("BROKEN_HEART to %lx", get_forward_ptr(t));
			return;
		}
		default:
			error("Unexpected IValue type to iterate through with tag %d!\n", obj_type(t));
	}
}

void print_arrayivalue(ArrayIValue* t){
	printf("[");
	for(int i = 0; i < t->length; ++i){
		if(i){
			printf(",");
		}
		print_ivalue(t->slots[i]);
	}
	printf("]");
}

void print_objectivalue (ObjectIValue* t) {
	printf("{");
	for(int i = 0; i < t->class_ptr->num_slots; ++i){
		if(i){
			printf(",");
		}
		print_ivalue(t->var_slots[i]);
	}
	printf("}");
}

void _error(const char* format, ...){
	va_list args;
	va_start(args, format);
	v_errorif(1, format, args);
	va_end(args);
}

void _errorif (int boolean, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	v_errorif(boolean, msg, args);
	va_end(args);
}

void _assert_msg (int boolean, const char* msg, ...){
	va_list args;
	va_start(args, msg);
	v_errorif(!boolean, msg, args);
	va_end(args);
}

void v_errorif (int boolean, const char* msg, va_list va){
	if(!boolean){ return; }

	const char * surround = "########################";
	printf("%s\nERROR:", surround);
	vprintf(msg, va);
	printf("%s\n", surround);
#ifdef DEBUG
	void *array[10];
	size_t size = backtrace (array, sizeof(array)/sizeof(void*));
	char **strings = backtrace_symbols (array, size);
	debugf ("Obtained %zd stack frames.\n", size);
	for (size_t i = 0; i < size; i++)
		 debugf ("%s\n", strings[i]);
	free (strings);
#endif
	abort();
}

void _debugf(const char * format, ...){
	va_list args;
	va_start(args, format);
	vprintf(/*stderr,*/ format, args);
	va_end(args);
}
