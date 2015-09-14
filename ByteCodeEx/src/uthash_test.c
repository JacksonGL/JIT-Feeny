#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/mman.h>
#include "uthash.h"

typedef int ValIndex;

typedef struct {
	const char *key;	// hashtable item key
	ValIndex value;		// hashtable item value
	UT_hash_handle hh;	// used for internal book keeping, ignore it
} Hashable;

ValIndex find_item(Hashable* table, char* key);
void add_item(Hashable* table, char* key, ValIndex value);

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

int main (int argc, char** argvs) {
  Hashable* table = init_hashtable();
  add_item(table, "test", 1);
  printf("%d\n", find_item(table, "test"));
  printf("%d\n", find_item(table, "test2"));
  add_item(table, "test2", 3);
  printf("%d\n", find_item(table, "test2"));
  add_item(table, "test", 4);
  printf("%d\n", find_item(table, "test"));
  return 0;
}
