#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

enum{
	DS_NULL_ERR = 0,
	DS_MEM_ERR,
	DS_INDEX_ERR,
	DS_BOUNDS_ERR,
	DS_INVALID,
};

extern uint16_t ds_error;
#define DS_ERROR_MSG ds_error_messages[ds_error]
extern const char* const ds_error_messages[];

typedef struct {
	size_t size;
	size_t memsize;
	size_t data_size;
	const char* data;
} dynamic_array_t;

// data HAS to be a pointer type (e.g. const char*)
#define DYNAMIC_ARRAY(_data) struct { size_t size; size_t memsize; size_t data_size; _data; }
#define NEW_DYNAMIC_ARRAY(_data_size) {0,0,(_data_size),NULL}
#define DYNAMIC_ARRAY_GROW(x) (x) * 2
#define DYNAMIC_ARRAY_START 16

bool dynamic_array_alloc(dynamic_array_t*);
bool dynamic_array_pushback(dynamic_array_t*,const void*);
bool dynamic_array_push(dynamic_array_t*,const void*,size_t);
void dynamic_array_popback(dynamic_array_t*);
bool dynamic_array_pop(dynamic_array_t*,size_t);
void dynamic_array_free(dynamic_array_t*);

typedef DYNAMIC_ARRAY(uint8_t* pairs) hashset_t;
typedef struct {
	hashset_t* sets;
	size_t set_count;
	size_t max_size;	// Max amount of pairs in a single set

	// Hashing function
	// const void* : pointer to key / value pair
	// returns the index of the set the pair belongs to
	// IF NULL, will default to using first byte as the index
	size_t (*hashing_func)(const void*);
} hashtable_t;

#define NEW_HASHTABLE(_sz,_pair_sz) {NULL,0,(_sz),(_pair_sz),NULL}
#define HASHTABLE_GROW(x) (x) * 2
#define HASHTABLE_START 8

bool hashtable_setup(hashtable_t*,size_t);
bool hashtable_grow(hashtable_t*,size_t);
bool hashtable_set(hashtable_t*,const void*);
hashset_t* hashtable_get(hashtable_t*,const void*);
void hashtable_parse(hashtable_t*,void (*)(void*,void*),void*);
void hashtable_free(hashtable_t*);

typedef struct{
	size_t size;
	const char* ptr;
	const char* memory;
} arena_t;
#define NEW_ARENA() {0,NULL,NULL}

#define KB 1024
#define MB 1024*KB
#define GB 1024*MB

bool arena_setup(arena_t*,size_t);
void* arena_alloc(arena_t*,size_t);
bool arena_free(arena_t*,size_t);
void arena_destroy(arena_t*);

#endif