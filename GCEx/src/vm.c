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

typedef enum {
	GET_GLOBAL_OP_QUICK = DROP_OP+1,
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

// #define DEBUG 1

// Copied from sys/time.h
// to not pollute our #define space
// WAS NOT WRITTEN BY LG OR BM - rights belong to original author
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



typedef enum {
  INT_OBJ,
  NULL_OBJ,
  STR_OBJ,
  METHOD_OBJ,
  SLOT_OBJ,
  CLASS_OBJ,
  OBJ_OBJ,
  ARRAY_OBJ
} ObjTag;

typedef struct {
  intptr_t tag;
  intptr_t space;
} NullIValue;

/* reuse the value structure from bytecode.h */

typedef struct {
  intptr_t tag;
  intptr_t space;
} IValue;

typedef struct {
  intptr_t tag;
  intptr_t value;
} IntIValue;

typedef struct {
  intptr_t tag;
  char* value;
} StringIValue;

typedef struct {
  intptr_t tag;
  Vector* code;
  int name;
  int nargs;
  int nlocals;
} MethodIValue;

typedef struct {
  intptr_t tag;
  intptr_t length;
  IValue* slots[0];
} ArrayIValue;

typedef struct {
  intptr_t tag;
  // a vector of slot indices
  Vector* slots;
  //linear search - fine when objects are small
  Vector* name_to_slot_vec;
} ClassIValue;

typedef struct {
  intptr_t tag;
  IValue* value;
  int name;
} SlotIValue;

typedef struct {
  intptr_t tag;
  ClassIValue* class_ptr;
  IValue* parent_obj_ptr;
  SlotIValue* var_slots[0];
} ObjectIValue;

typedef struct {
  // the values of all local variables
  // defined in the function
  Vector* slot_vec_ptr;
  // address of the call byte
  // code instruction, this is
  // also the return address
  ByteIns* call_ins_idx;
  // function slot pointer
  MethodIValue* func_ptr;
} _Frame;

typedef struct {
  // the values of all local variables
  // defined in the function
  Vector* slot_vec_ptr;
  // address of the call byte
  // code instruction, this is
  // also the return address
  int call_ins_idx;
  // function slot pointer
  MethodIValue* func_ptr;
  // caller's frame
  _Frame* call_frame_ptr;
} Frame;

// global data structure operations

// operand stack operations
IValue* stack_pop ();
IValue* stack_peek ();
void stack_push (IValue* val);
Vector* get_operand_stack ();

int get_entry_function ();
Vector* get_global_slots ();
Vector* get_constant_pool ();
IValue* get_val_constant(int idx);
void associate_labels(MethodIValue *val);
void set_global_slots (Vector* slots);
void set_entry_function (int entry_index);
void addto_constant_pool (Vector* values, Value* v);
ClassIValue* create_class (Vector* values, ClassValue* v2);

// frame operations
Frame* get_cur_frame ();
Frame* get_root_frame ();
void set_cur_frame (Frame* new_frame);
IValue* get_frame_slot (Frame* frame, int idx);
void set_frame_slot (Frame* frame, int idx, IValue* val);

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

Frame* create_frame (int call_ins_idx,
                     MethodIValue* func_ptr, Frame* call_frame);

// util operations
char* intToString (int i);
char* toString (IValue *val_ptr);
char* copy_string (const char *string);
char* arrayToString (ArrayIValue *obj_ptr);
char* str_replace (char *orig, char *rep, char *with);

ObjTag obj_type (IValue* o);
NullIValue* make_null_obj ();
IntIValue* make_int_obj (int value);
IntIValue* array_length (ArrayIValue* array);
IValue* array_get (ArrayIValue* a, IntIValue* i);
ArrayIValue* make_array_obj (int length, IValue* init);
NullIValue* array_set (ArrayIValue* a, IntIValue* i, IValue* v);

IValue* eq(IntIValue* x, IntIValue* y);
IValue* lt(IntIValue* x, IntIValue* y);
IValue* le(IntIValue* x, IntIValue* y);
IValue* gt(IntIValue* x, IntIValue* y);
IValue* ge(IntIValue* x, IntIValue* y);
IntIValue* int_obj_add (IntIValue* x, IntIValue* y);
IntIValue* int_obj_sub (IntIValue* x, IntIValue* y);
IntIValue* int_obj_mul (IntIValue* x, IntIValue* y);
IntIValue* int_obj_div (IntIValue* x, IntIValue* y);
IntIValue* int_obj_mod (IntIValue* x, IntIValue* y);

// assert functions
void assert_obj_obj (IValue* ptr);
void assert_not_null (void* ptr);
IntIValue* to_int_val (IValue* val);
SlotIValue* to_slot_val (IValue* val);
MethodIValue* to_function_val (IValue* val);
void exp_assert(int i, const char * fmt, ...);

// free functions
void free_frame(Frame* frame);

#ifndef PRE_SUBMIT
// statistics data structure
typedef struct {
  struct timeval total_time;               // total time in ms
  long total_method_call;        // # of method calls
  long total_int_method_call;    // # of method calls with integer receiver
  long total_array_method_call;  // # of method calls with array receiver
  long total_envobj_method_call; // # of method calls with env obj receiver
  struct timeval total_time_lookup_entry;  // total time in ms spend looking
  // up an entry in env obj
} Stat;

// collect statistics operations
void init_stat ();
void print_help ();
int is_collect_stat ();
void write_stat (char* filename);
void set_collect_stat (int flag);
void inc_total_time (const struct timeval *total_time);
void inc_entry_lookup_time (const struct timeval* time);
void inc_method_call (IValue* receiver_ptr);
void start_time_counter (struct timeval *t);
void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* diff);
#endif // PRE_SUBMIT

//============================================================
//========== GLOBAL DATA STRUCTURE OPERATIONS ================
//============================================================

typedef struct {
	char * name;
	int val;
} NamePair;

int find_item(Vector * pairs, char* name){
	for(int i = 0; i < pairs->size; ++i){
		NamePair* np = vector_get(pairs, i);
		if(np->name == name || !strcmp(np->name, name)){
			return np->val;
		}
	}
	return -1;
}

void add_item(Vector * pairs, char*name, int idx){
	NamePair* np = malloc(sizeof(NamePair));
	np->name = name;
	np->val = idx;
	vector_add(pairs, np);
}

IValue* find_slot_by_name(ObjectIValue* obj, char* function_name) {
  struct timeval start, end;
  // start collecting time
  if (is_collect_stat())
    start_time_counter(&start);

  IValue* ret = NULL;
  int ret_val_idx = find_item(obj->class_ptr->name_to_slot_vec,
                              function_name);
  if (ret_val_idx != -1) {
	  if (ret_val_idx >= get_num_var_slots(obj->class_ptr)) { // this is a method slot
			int slot_idx = (int)vector_get(obj->class_ptr->slots, ret_val_idx);
		  return get_val_constant(slot_idx);
		}
    ret = (IValue*)obj->var_slots[ret_val_idx];
  } else {
    // search in the parent object
    // assumes that in Feeny, object can only inherit from
    // user defined object or null object
    IValue* parent = obj->parent_obj_ptr;
    if (obj_type(parent) == OBJ_OBJ) {
      ret = find_slot_by_name((ObjectIValue*)parent,
                               function_name);
    } else {
      if (obj_type(parent) != NULL_OBJ) {
        printf("Error[2]: find_slot_by_name.\n");
        exit(-1);
      }
      ret = NULL;
    }
  }

  // end collecting time
  if (is_collect_stat()) {
    struct timeval time;
    end_time_counter(&start, &end, &time);
    inc_entry_lookup_time(&time);
  }
  return ret;
}

// count the number of slots in the class
// that are variable slots
int get_num_var_slots (ClassIValue * class_val) {
  int var_slot_count = 0;
  for (int i = 0; i < class_val->slots->size; i++) {
    int slot_idx = (int)vector_get(class_val->slots, i);
    IValue* slot = get_val_constant(slot_idx);
    assert_not_null(slot);
    if (obj_type(slot) == SLOT_VAL) { //TODO: fix item
      var_slot_count++;
    }
  }
  return var_slot_count;
}

SlotIValue* copy_var_slot (SlotIValue * slot) {
  SlotIValue* rSlot =
    (SlotIValue*)malloc(sizeof(SlotIValue));
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
IValue* get_val_constant(int idx) {
  Vector* constant_pool = get_constant_pool();
  return vector_get(constant_pool, idx);
}

// returns the string content
// indexed by idx in the constant pool
char* get_str_constant(int idx) {
  IValue* val = get_val_constant(idx);
  if (val == NULL || obj_type(val) != STR_OBJ) {
    printf("Error: get string constant.\n");
    exit(-1);
  }
  return ((StringIValue*)val)->value;
}

// instructor pointer
static int inst_ptr = -1;

static Vector* inst_label_vec = NULL;
void init_label_vec(){
  if(!inst_label_vec){
    inst_label_vec = make_vector();
  }
}

void set_label_addr (char* label, int addr) {
  if(!inst_label_vec) inst_label_vec = make_vector();
  add_item(inst_label_vec, label, addr);
}
int get_label_addr (char* label) {
  return find_item(inst_label_vec, label);
}
// hashtable that maps string names to global
// slots' index in the constant pool
// each index is guaranteed to refer to either
// a Slot object or a Method object
static Vector* global_slots = NULL;
static Vector* global_slot_vec = NULL;
void set_global_slots (Vector * slots) {
  int val_idx, name_idx;
  IValue* value = NULL;
  char* name_str = NULL;
  global_slots = slots;
  // establish string name to
  // slot index hashtable
  global_slot_vec = make_vector();
  for (int i = 0; i < slots->size; i++) {
    val_idx = (int)vector_get(slots, i);
    value = get_val_constant(val_idx);
    name_idx = ((SlotIValue*)value)->name;
    name_str = get_str_constant(name_idx);
    add_item(global_slot_vec, name_str, val_idx);
  }
}
Vector* get_global_slots () {
  return global_slots;
}

IValue* get_global_slot_by_name (char* name) {
  int slot_idx = find_item(global_slot_vec, name);
  if (slot_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
  }
  IValue* val = get_val_constant(slot_idx);
  if (val == NULL) {
    printf("Error[2]: get global slot by name.\n");
    exit(-1);
  }
  return val;
}

IValue* get_global_slot_by_idx (int slot_idx) {
  IValue* val = get_val_constant(slot_idx);
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

static Vector* operand_stack = NULL;
Vector* get_operand_stack () {
  if (operand_stack == NULL)
    operand_stack = make_vector();
  return operand_stack;
}

void stack_push (IValue * val) {
  vector_add(operand_stack, val);
}
IValue* stack_pop () {
  if (operand_stack->size == 0) {
    return NULL;
  }
  return vector_pop(operand_stack);
}
IValue* stack_peek () {
  return vector_peek(operand_stack);
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
  return current_frame_ptr;
}

void set_cur_frame (Frame * new_frame) {
  /*if (current_frame_ptr != (Frame*)(new_frame->call_frame_ptr)) {
    printf("Error: set_cur_frame.\n");
    exit(-1);
  }*/
  current_frame_ptr = new_frame;
}

void set_frame_slot (Frame * frame, int idx, IValue * val) {
  if (idx == frame->slot_vec_ptr->size) {
    vector_add(frame->slot_vec_ptr, val);
  } else if (0 <= idx && idx < frame->slot_vec_ptr->size) {
    vector_set(frame->slot_vec_ptr, idx, val);
  } else {
    printf("Error: set frame value.\n");
    exit(-1);
  }
}

IValue* get_frame_slot (Frame * frame, int idx) {
  if (idx >= frame->slot_vec_ptr->size) {
    printf("Error: get frame value.\n");
    exit(-1);
  }
  return vector_get(frame->slot_vec_ptr, idx);
}

Frame* create_frame (int call_ins_idx,
                     MethodIValue * func_ptr, Frame * call_frame) {
  Frame* ret = malloc(sizeof(Frame));
  ret->slot_vec_ptr = make_vector();
  ret->call_ins_idx = call_ins_idx;
  ret->func_ptr = func_ptr;
  ret->call_frame_ptr = (_Frame*)call_frame;
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
  IValue* val = stack_peek();
  if (val == NULL) {
    printf("Error: set local op.\n");
    exit(-1);
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
    printf("Error: get local op.\n");
    exit(-1);
  }
  IValue* val = get_frame_slot(frame, slot_idx);
  stack_push(val);
  inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_set_global_quick(SetGlobalIns* i){
  int name_idx = i->name;
  char* name_str = get_str_constant(name_idx);

  name_idx = find_item(global_slot_vec, name_str);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
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
    printf("Error: set global op.\n");
    exit(-1);
  }
  IValue* slot = get_global_slot_by_idx(name_idx);
  if (slot == NULL || obj_type(slot) != SLOT_OBJ) { // TODO: make safe?
    printf("Error[2]: set global op.\n");
    exit(-1);
  }
  SlotIValue* rSlot = (SlotIValue*)slot;
  rSlot->value = val;
  // NOTE: do not pop from the stack
  inst_ptr++;
}

// In-place makes a quick version of the global ins
void make_get_global_quick(GetGlobalIns* i){
  int name_idx = i->name;
  char* name_str = get_str_constant(name_idx);

  name_idx = find_item(global_slot_vec, name_str);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
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
  if (slot == NULL || obj_type(slot) != SLOT_OBJ) { // TODO: make safe?
    printf("Error: get global op.\n");
    exit(-1);
  }
  SlotIValue* rSlot = (SlotIValue*)slot;
  IValue* val = rSlot->value;
  if (val == NULL) {
    printf("Error[2]: get global op.\n");
    exit(-1);
  }
  stack_push(val);
  inst_ptr++;
}

void make_branch_quick(BranchIns *i){
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
  IValue* val = stack_pop();
  if (val == NULL) {
    printf("Error: branch op.\n");
    exit(-1);
  }
  if (obj_type(val) != NULL_OBJ) {
    inst_ptr = i->addr;
  } else {
    inst_ptr++;
  }
}

void make_goto_quick(GotoIns *i){
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

void exec_goto_quick_op (GotoQuickIns *i){
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
  IValue* val = get_val_constant(val_idx);
  if (val == NULL || (obj_type(val) != INT_OBJ && obj_type(val) != NULL_OBJ)) {
    printf("Error in LIT_OP.\n");
    exit(-1);
  }
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
  IValue* length = stack_pop();
  IntIValue* len = to_int_val(length);
  IValue* array = (IValue*)make_array_obj(len->value, init_value);
  stack_push(array);
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
    IValue* val = vector_pop(args);
    char *arg_str = toString(val);
    new_str = str_replace(old_str, replace_char, arg_str);
    if (arg_str != NULL) free(arg_str);
    if (old_str != NULL) free(old_str);
  }
  if (new_str != NULL) {
    printf("%s", new_str);
    free(new_str);
  }
  vector_free(args);
  stack_push((IValue*)make_null_obj());
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
  if (class == NULL || obj_type(class) != CLASS_VAL) {
    printf("Error: exec_object_op.\n");
    exit(-1);
  }
  ClassIValue* class_val = (ClassIValue*)class;
  int num_slots = get_num_var_slots(class_val);
  // init new object value
  ObjectIValue* obj = (ObjectIValue*)malloc(sizeof(ObjectIValue)+sizeof(IValue*)*(num_slots));
  obj->tag = OBJ_OBJ;
	for (int i = class_val->slots->size - 1; i >= 0 ; i--) {

    int slot_idx = (int)vector_get(class_val->slots, i);
    IValue* slot_in_class = get_val_constant(slot_idx);
    assert_not_null(slot_in_class);
    if (obj_type(slot_in_class) == METHOD_OBJ) {
		 // Only copy all variable slots - stop on the first method slot
		continue;
    } else if (obj_type(slot_in_class) == SLOT_VAL) {
      // copy var slot
      SlotIValue* new_slot =
        copy_var_slot((SlotIValue*)slot_in_class);
      // first var slot will be initialized
      // to the deepest value on the stack
      new_slot->value = stack_pop();
      if (new_slot->value == NULL) {
        printf("Error[2]: exec_object_op.\n");
        exit(-1);
      }
      obj->var_slots[i]=new_slot;
    } else {
      printf("Error[3]: exec_object_op.\n");
      exit(-1);
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
  SlotIValue* slotVal = to_slot_val(slot);
  slotVal->value = val;
  stack_push(val);
  inst_ptr++;
}

void call_func (MethodIValue * function_slot, int arity) {
  // pops arguments from the stack
  Vector* tmp_vec = make_vector();
  // create a new local frame
  Frame* local_frame = create_frame(inst_ptr,
                                    function_slot, get_cur_frame());
  // the first arity slots are the arguments
  int n = arity;
  while (n-- > 0) {
    vector_add(tmp_vec, stack_pop());
  }
  n = arity;
  while (n-- > 0) {
    vector_add(local_frame->slot_vec_ptr, vector_pop(tmp_vec));
  }
  vector_free(tmp_vec);
  set_cur_frame(local_frame);
  // set the instructor pointer to the first
  // instruction in the function body
  inst_ptr = 0;
}

void make_call_quick(CallIns * i){
  int function_name_idx = i->name;
  int arity = i->arity;

  char* function_name = get_str_constant(function_name_idx);
  int name_idx = find_item(global_slot_vec, function_name);
  if (name_idx < 0) {
    printf("Error: get global slot by name.\n");
    exit(-1);
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

void exec_call_quick_op(CallQuickIns * i){
  IValue* function_slot = get_global_slot_by_idx(i->idx);
  if (function_slot == NULL || obj_type(function_slot) != METHOD_OBJ) {
    printf("Error: exec_call_op.\n");
    exit(-1);
  }
  call_func((MethodIValue*)function_slot, i->arity);
  // instruction pointer is updated in call_func
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
  int method_name_idx = i->name;
  char* method_name = get_str_constant(method_name_idx);
  int arity = i->arity > 0 ? i->arity - 1 : 0;
  // pops arguments from the stack
  Vector* tmp_arg_vec = make_vector();
  // the first arity slots are the arguments
  int n = arity;
  while (n-- > 0) {
    vector_add(tmp_arg_vec, stack_pop());
  }
  // pop the receiver object
  IValue* receiver_ptr = stack_pop();
  assert_not_null(receiver_ptr);
  exp_assert(obj_type(receiver_ptr) != NULL_OBJ,
             "Null object does not have methods!");
  IValue* retVal = NULL;
  inc_method_call(receiver_ptr);

  switch (obj_type(receiver_ptr)) {
  // handle built in functions
  case INT_OBJ: {
    IValue* arg;
    exp_assert(arity == 1 && obj_type(arg = vector_get(tmp_arg_vec, 0)) == INT_OBJ,
               "native int function error - %s",
               arity != 1 ? "not enough arguments!" :
               "wrong argument type!");
    if (!strcmp(method_name, "add")) {
      retVal = (IValue*) int_obj_add((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "sub")) {
      retVal = (IValue*) int_obj_sub((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "mul")) {
      retVal = (IValue*) int_obj_mul((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "div")) {
      retVal = (IValue*) int_obj_div((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "mod")) {
      retVal = (IValue*) int_obj_mod((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "gt")) {
      retVal = (IValue*) gt((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "ge")) {
      retVal = (IValue*) ge((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "lt")) {
      retVal = (IValue*) lt((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "le")) {
      retVal = (IValue*) le((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else if (!strcmp(method_name, "eq")) {
      retVal = (IValue*) eq((IntIValue*) receiver_ptr, (IntIValue*)arg);
    } else {
      exp_assert(0, "unknown native int function");
    }
    stack_push(retVal);
    inst_ptr++;
    break;
  }
  case ARRAY_OBJ: {
    IValue* result = NULL;
    if (!strcmp(method_name, "length") && arity == 0) {
      result = (IValue*)array_length((ArrayIValue*) receiver_ptr);
    } else {
      Vector* args = make_vector();
      n = arity;
      while (n-- > 0) {
        vector_add(args, vector_pop(tmp_arg_vec));
      }
      IValue* first_arg;
      exp_assert(arity > 0 && obj_type(first_arg = vector_get(args, 0)) == INT_OBJ,
                 "native array function error - %s",
                 arity < 1 ? "not enough arguments!" :
                 "incorrect argument type!");
      if (!strcmp(method_name, "set") && arity == 2) {
        result = (IValue*)array_set((ArrayIValue*)receiver_ptr,
                                   (IntIValue*)first_arg, vector_get(args, 1));
      } else if (!strcmp(method_name, "get") && arity == 1) {
        result = (IValue*)array_get((ArrayIValue*)receiver_ptr, (IntIValue*)first_arg);
      } else {
        exp_assert(0, "unknown native array function");
      }
      vector_free(args);
    }
    stack_push(result);
    inst_ptr++;
    break;
  }
  case OBJ_OBJ: {
    IValue* slot = find_slot_by_name((ObjectIValue*)receiver_ptr, method_name);
    assert_not_null(slot);
    MethodIValue* method_slot = to_function_val(slot);
    Vector* args = make_vector();
    // create a new local frame
    Frame* local_frame = create_frame(inst_ptr,
                                      method_slot, get_cur_frame());
    // the slot 0 is the receiver object
    vector_add(local_frame->slot_vec_ptr, receiver_ptr);
    n = arity;
    while (n-- > 0) {
      vector_add(local_frame->slot_vec_ptr, vector_pop(tmp_arg_vec));
    }
    set_cur_frame(local_frame);
    inst_ptr = 0;
    vector_free(args);
    break;
  }
  default:
    printf("receiver_ptr->tag: %d\n", receiver_ptr->tag);
    exp_assert(0, "Cannot call method on null object!");
    exit(-1);
  }
  vector_free(tmp_arg_vec);
}

void exec_ins (ByteIns * ins) {
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

ClassIValue* create_class(Vector * values, ClassValue * v2) {
  ClassIValue* new_v =
    (ClassIValue*)malloc(sizeof(ClassIValue));
  new_v->tag = v2->tag;
  new_v->slots = v2->slots;
  new_v->name_to_slot_vec = make_vector();

  int index_slot = 0, index_method = v2->slots->size-1;
	while(index_slot < index_method && index_method >= 0){
		while(index_method >= 0 && obj_type((IValue*)vector_get(values, (int)vector_get(v2->slots, index_method))) == METHOD_OBJ){
			index_method--;
		}
		while(index_slot <= index_method && obj_type((IValue*)vector_get(values, (int)vector_get(v2->slots, index_slot))) == SLOT_OBJ){
			index_slot++;
		}
		if(index_slot < index_method && index_method >=0){
			void * ind = vector_get(v2->slots, index_slot);
			vector_set(v2->slots, index_slot, vector_get(v2->slots, index_method));
			vector_set(v2->slots, index_method, ind);
			index_slot++;
			index_method--;
		}
	}

  // iterate over all slots
  for (int i = 0; i < v2->slots->size; i++) {
    // get the slot value
    int slot_index = (int)vector_get(v2->slots, i);
    IValue* value = vector_get(values, slot_index);
    if (obj_type(value) != SLOT_OBJ && obj_type(value) != METHOD_OBJ) {
      printf("non-slot or method value in class\n");
      exit(-1);
    }
    int name_index = ((SlotValue*)value)->name;
    // get the string value of name
    value = vector_get(values, name_index);
    if (obj_type(value) != STR_OBJ) {
      printf("Slot value name is not a string.\n");
      exit(-1);
    }
    char* name_ptr = ((StringValue*)value)->value;
    // map the string name to the slot index in new_v->slots
    add_item(new_v->name_to_slot_vec, name_ptr, i);
  }
  return new_v;
}

void associate_labels(MethodIValue *method) {
  for (int i = 0; i < method->code->size; i++) {
    ByteIns* ins = (ByteIns*)vector_get(method->code, i);
    if (ins->tag == LABEL_OP) {
      process_label_op((LabelIns*)ins, i);
    }
  }
}

IntIValue* toIntIValue(IntValue* v){
	return make_int_obj(v->value);
}

StringIValue* toStringIValue(StringValue*v){
	StringIValue * ret = malloc(sizeof(StringIValue));
	ret->tag = STR_OBJ;
	ret->value = v->value;
	return ret;
}

MethodIValue* toMethodIValue(MethodValue*v){
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
void addto_constant_pool (Vector * values, Value * v) {
  Vector* constant_pool = get_constant_pool();
  switch (v->tag) {
  case INT_VAL:
	  vector_add(constant_pool, toIntIValue((IntValue*)v));
	  break;
  case NULL_VAL:
	  vector_add(constant_pool, make_null_obj());
	  break;
  case STRING_VAL:
	  vector_add(constant_pool, toStringIValue((StringValue*)v));
	  break;
  case METHOD_VAL: {
	MethodIValue* m = toMethodIValue((MethodValue*)v);
    vector_add(constant_pool, m);
      associate_labels(m);
    break;
  }
  case SLOT_VAL: {
    SlotValue* v2 = (SlotValue*)v;
    SlotIValue* new_v = (SlotIValue*)malloc(sizeof(SlotIValue));
    new_v->tag = v2->tag;
    new_v->name = v2->name;
    new_v->value = NULL;
    vector_add(constant_pool, new_v);
    break;
  }
  case CLASS_VAL: {
    ClassValue* v2 = (ClassValue*)v;
    ClassIValue* new_v = create_class(values, v2);
    vector_add(constant_pool, new_v);
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
  while (1) {
    // get the current instruction
    Frame* cur_frame = get_cur_frame();
    if (cur_frame == get_root_frame()) {
      break; // execution stops
    }
    Vector* code = cur_frame->func_ptr->code;
    if (inst_ptr < 0 || inst_ptr >= code->size) {
      printf("Error: wrong inst_ptr: %d.\n", inst_ptr);
      exit(-1);
    }
#ifdef DEBUG
    printf("%d, %d\n", code->size, inst_ptr);
    printf("stack size: %d\n", operand_stack->size);
#endif
    ByteIns* ins = vector_get(code, inst_ptr);
#ifdef DEBUG
    print_ins(ins);
    printf("\n");
#endif
    exec_ins(ins);
  }
}

void exec_prog (Program * p) {
  // constant pool
  Vector* vs = p->values;
  for (int i = 0; i < vs->size; i++) {
    addto_constant_pool(vs, vector_get(vs, i));
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
//==================== COLLECT STAT ==========================
//============================================================

void print_help () {
  printf("Command line:\n\tcfeeny filename [-s path]\n\n\t-s print statistics to the specified file\n");
}


static int collectStat = 0;
static Stat* stat = NULL;

void set_collect_stat (int flag) {
  collectStat = flag;
  if (flag) {
    init_stat();
  }
}

int is_collect_stat () {
  return collectStat != 0;
}

void init_stat () {
  stat = malloc(sizeof(Stat));
  stat->total_time.tv_usec = 0;
  stat->total_time.tv_sec = 0;
  stat->total_method_call = 0;
  stat->total_int_method_call = 0;
  stat->total_array_method_call = 0;
  stat->total_envobj_method_call = 0;
  stat->total_time_lookup_entry.tv_sec = 0;
  stat->total_time_lookup_entry.tv_usec = 0;
}

void start_time_counter (struct timeval *t) {
  gettimeofday(t, NULL);
}

void end_time_counter (struct timeval *start, struct timeval *end, struct timeval* result) {
  gettimeofday(end, NULL);
  timersub(end, start, result);
}

void inc_total_time (const struct timeval *total_time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time.tv_sec += total_time->tv_sec;
  stat->total_time.tv_usec += total_time->tv_usec;
}

void inc_entry_lookup_time (const struct timeval* time) {
  if (collectStat == 0 || stat == NULL)
    return;
  stat->total_time_lookup_entry.tv_sec += time->tv_sec;
  stat->total_time_lookup_entry.tv_usec += time->tv_usec;
}

void inc_method_call (IValue* receiver_ptr) {
  if (receiver_ptr == NULL
          || collectStat == 0
          || stat == NULL)
    return;

  stat->total_method_call++;
  switch (obj_type(receiver_ptr)) {
  case INT_OBJ:
    stat->total_int_method_call++;
    break;
  case ARRAY_OBJ:
    stat->total_array_method_call++;
    break;
  case OBJ_OBJ:
    stat->total_envobj_method_call++;
    break;
  default:
    break;
  }
}

void write_stat (char* filename) {
  if (!is_collect_stat() || stat == NULL) return;
  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }

  fprintf(f, "Total Time: %f ms\n", 0.0+stat->total_time.tv_sec*1000.0+stat->total_time.tv_usec/1000.0);
  fprintf(f, "Total Time Lookup Entry: %f ms\n", 0.0+stat->total_time_lookup_entry.tv_sec*1000.0+stat->total_time_lookup_entry.tv_usec/1000.0);
  fprintf(f, "Total Method Call: %ld\n", stat->total_method_call);
  fprintf(f, "Total Integer Method Call: %ld\n", stat->total_int_method_call);
  fprintf(f, "Total Array Method Call: %ld\n", stat->total_array_method_call);
  fprintf(f, "Total EnvObj Method Call: %ld\n", stat->total_envobj_method_call);

  fclose(f);
}

//============================================================
//================== UTIL FUNCITONS ==========================
//============================================================

void free_frame(Frame* frame) {
  vector_free(frame->slot_vec_ptr);
  free(frame);
}

MethodIValue* to_function_val (IValue* val) {
  if (val == NULL || obj_type(val) != METHOD_OBJ) {
    printf("Error: not a function value.\n");
    exit(-1);
  }
  return (MethodIValue*) val;
}

SlotIValue* to_slot_val (IValue* val) {
  if (val == NULL || obj_type(val) != SLOT_VAL) {
    printf("Error: not a var slot.\n");
    exit(-1);
  }
  return (SlotIValue*) val;
}

IntIValue* to_int_val (IValue* val) {
  if (val == NULL || obj_type(val) != INT_OBJ) {
    printf("Error: to_int_val.\n");
    exit(-1);
  }
  return (IntIValue*)val;
}

void assert_not_null (void* ptr) {
  if (ptr == NULL) {
    printf("Error: assert_not_null.\n");
    exit(-1);
  }
}

void assert_obj_obj (IValue* ptr) {
  if (ptr == NULL || obj_type(ptr) != OBJ_OBJ) {
    printf("Error: assert_obj_obj.\n");
    exit(-1);
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

ObjTag obj_type (IValue * o) {
  return (ObjTag)o->tag;
}

NullIValue* make_null_obj () {
  static NullIValue n = {NULL_OBJ, 0};
  return &n;
}

IntIValue* make_int_obj (int value) {
  //cache for memory usage - could make the cache larger
  static int cache_initted = 0;
  static IntIValue cached[101];
  if (! cache_initted) {
    for (int i = 0; i < sizeof(cached) / sizeof(IntIValue); ++i) {
      cached[i].tag = INT_OBJ;
      cached[i].value = i;
    }
    cache_initted = 1;
  }
  if (value >= 0 && value < sizeof(cached) / sizeof(IntIValue)) {
    return &cached[value];
  }

  IntIValue* t = malloc(sizeof(IntIValue));
  t->tag = INT_OBJ;
  t->value = value;
  return t;
}

IntIValue* array_length (ArrayIValue* array) {
  return make_int_obj(array->length);
}

NullIValue* array_set (ArrayIValue* a, IntIValue* i, IValue* v) {
  if (i->value >= a->length || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, i->value);
    exit(-1);
  }
  a->slots[i->value] = v;
  return make_null_obj();
}

IValue* array_get (ArrayIValue* a, IntIValue* i) {
  if (i->value >= a->length || i->value < 0) {
    printf("array index out of bound. array length: %d. index: %d", a->length, i->value);
    exit(-1);
  }
  return a->slots[i->value];
}

ArrayIValue* make_array_obj(int length, IValue* init) {
  ArrayIValue* t = malloc(sizeof(ArrayIValue)+sizeof(IValue*)*length);
  t->tag = ARRAY_OBJ;
  t->length = length;
  for (int i = 0; i < length; ++i) {
	  t->slots[i] = init;
  }
  return t;
}

IntIValue* int_obj_add (IntIValue * x, IntIValue * y) {
  return make_int_obj(x->value + y->value);
}
IntIValue* int_obj_sub (IntIValue * x, IntIValue * y) {
  return make_int_obj(x->value - y->value);
}
IntIValue* int_obj_mul (IntIValue * x, IntIValue * y) {
  return make_int_obj(x->value * y->value);
}
IntIValue* int_obj_div (IntIValue * x, IntIValue * y) {
  return make_int_obj(x->value / y->value);
}
IntIValue* int_obj_mod (IntIValue * x, IntIValue * y) {
  return make_int_obj(x->value % y->value);
}

IValue* eq(IntIValue * x, IntIValue * y) {
  return x->value == y->value ?
         (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
}
IValue* lt(IntIValue * x, IntIValue * y) {
  return x->value < y->value ?
         (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
}
IValue* le(IntIValue * x, IntIValue * y) {
  return x->value <= y->value ?
         (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
}
IValue* gt(IntIValue * x, IntIValue * y) {
  return x->value > y->value ?
         (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
}
IValue* ge(IntIValue * x, IntIValue * y) {
  return x->value >= y->value ?
         (IValue*)make_int_obj(0) : (IValue*)make_null_obj();
}

// convert an interpreter object into a string
char* toString (IValue * val_ptr) {
  switch (obj_type(val_ptr)) {
  case INT_OBJ: {
    return intToString(((IntIValue*)val_ptr)->value);
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
    printf("Error: toString.\n");
    exit(-1);
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

