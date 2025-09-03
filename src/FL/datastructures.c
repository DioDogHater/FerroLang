#include "datastructures.h"

uint16_t ds_error = DS_INVALID;
const char* const ds_error_messages[] = {
	"NULL pointer exception",
	"failed to allocate memory",
	"index out of bounds",
	"memory out of bounds",
	"no error"
};
#define DS_ERROR(_e) ds_error = (_e);

// Allocates the necessary memory if necessary for 1 more element
bool dynamic_array_alloc(dynamic_array_t* array){
	if(!array){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(array->memsize - array->size < 1){
		array->memsize = (array->memsize) ? DYNAMIC_ARRAY_GROW(array->memsize) : DYNAMIC_ARRAY_START;
		array->data = (const char*) realloc((void*)array->data, array->memsize * array->data_size);
		if(!array->data){
			DS_ERROR(DS_MEM_ERR);
			return false;
		}
	}
	return true;
}

// Makes sure the dynamic array can hold (size) more elements
bool dynamic_array_grow(dynamic_array_t* array, size_t size){
	if(!array){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(array->memsize - array->size < size){
		array->memsize = size - (array->memsize - array->size);
		array->data = (const char*) realloc((void*)array->data, array->memsize * array->data_size);
		if(!array->data){
			DS_ERROR(DS_MEM_ERR);
			return false;
		}
	}
	return true;
}

bool dynamic_array_pushback(dynamic_array_t* array, const void* data){
	if(!array || !data){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(!dynamic_array_alloc(array))
		return false;
	memcpy((void*)(array->data+array->size*array->data_size),data,array->data_size);
	array->size++;
	return true;
}

bool dynamic_array_push(dynamic_array_t* array, const void* data, size_t at){
	if(!array || !data){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(!dynamic_array_alloc(array))
		return false;
	if(at > array->size){
		DS_ERROR(DS_INDEX_ERR);
		return false;
	}if(at == array->size)
		return dynamic_array_pushback(array, data);
	memcpy(
		(void*)(array->data+(at+1)*array->data_size),
		(void*)(array->data+at*array->data_size),
		(array->size-at)*array->data_size
	);
	memcpy(
		(void*)(array->data+at*array->data_size),
		data,
		array->data_size
	);
	return true;
}

void dynamic_array_popback(dynamic_array_t* array){
	array->size--;
}

bool dynamic_array_pop(dynamic_array_t* array, size_t at){
	if(!array){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(at >= array->size){
		DS_ERROR(DS_INDEX_ERR);
		return false;
	}
	memcpy(
		(void*)(array->data+(at)*array->data_size),
		(void*)(array->data+(at+1)*array->data_size),
		(array->size-at-1)*array->data_size
	);
	array->size--;
	return true;
}

void dynamic_array_free(dynamic_array_t* array){
	if(!array)
		return;
	if(array->data)
		free((void*)array->data);
	*array = (dynamic_array_t) NEW_DYNAMIC_ARRAY(array->data_size);
}

bool hashtable_setup(hashtable_t* ht, size_t pair_size){
	ht->sets = (hashset_t*) realloc((void*)ht->sets, sizeof(hashset_t) * (ht->set_count ? ht->set_count : HASHTABLE_START));
	if(!ht->sets){
		DS_ERROR(DS_MEM_ERR);
		return false;
	}
	for(size_t i = 0; i < ht->set_count; i++)
		ht->sets[i] = (hashset_t) NEW_DYNAMIC_ARRAY(pair_size);
	return true;
}

bool hashtable_grow(hashtable_t* ht, size_t pair_size){
	size_t start_size = ht->set_count;
	ht->set_count = ht->set_count ? HASHTABLE_GROW(ht->set_count) : HASHTABLE_START;
	ht->sets = (hashset_t*) realloc((void*)ht->sets, sizeof(hashset_t) * ht->set_count);
	if(!ht->sets){
		DS_ERROR(DS_MEM_ERR);
		return false;
	}
	if(ht->set_count - start_size > 0){
		for(size_t i = start_size; i < ht->set_count; i++)
			ht->sets[i] = (hashset_t) NEW_DYNAMIC_ARRAY(pair_size);
		for(size_t i = 0; i < start_size; i++){
			for(size_t j = ht->sets[i].size; j < ~0; j--){
				void* pair = hashset_get(&ht->sets[i], j);
				ht->sets[i].size--;
				if(!hashtable_set(ht, pair))
					return false;
			}
		}
	}
	return true;
}

bool hashtable_set(hashtable_t* ht, const void* data){
	if(!ht || !data){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	size_t hash = ((ht->hashing_func) ? ht->hashing_func(data) : (size_t) *((uint8_t*)data));
	hash %= ht->set_count;
	if(hash >= ht->set_count){
		DS_ERROR(DS_INDEX_ERR);
		return false;
	}
	if(!dynamic_array_pushback((dynamic_array_t*)&ht->sets[hash],data))
		return false;
	if(ht->sets[hash].size > ht->max_size)
		return hashtable_grow(ht,ht->sets[hash].data_size);
	return true;
}

hashset_t* hashtable_get(hashtable_t* ht, const void* key){
	if(!ht || !key){
		DS_ERROR(DS_NULL_ERR);
		return NULL;
	}
	size_t hash = ((ht->hashing_func) ? ht->hashing_func(key) : (size_t) *((uint8_t*)key));
	if(ht->sets == NULL)
		return NULL;
	hash %= ht->set_count;
	return &ht->sets[hash];
}

void* hashset_get(hashset_t* hs, size_t at){
	if(!hs){
		DS_ERROR(DS_NULL_ERR);
		return NULL;
	}
	return (void*) (hs->pairs + hs->data_size * at);
}

void* hashtable_find(hashtable_t* ht, const void* key){
	if(!ht){
		DS_ERROR(DS_NULL_ERR);
		return NULL;
	}
	hashset_t* hs = hashtable_get(ht,key);
	if(!hs){
		DS_ERROR(DS_NULL_ERR);
		return NULL;
	}
	void* result = NULL;
	for(size_t i = 0; i < hs->size; i++){
		if(ht->cmp_func ? ht->cmp_func(key, (const void*)hashset_get(hs, i)) : (*(uint8_t*)key == *(uint8_t*)hashset_get(hs, i)))
			result = hashset_get(hs, i);
	}
	return result;
}

void hashtable_parse(hashtable_t* ht, void (*parse_func)(void*, void*), void* args){
	if(!ht || !parse_func)
		return;
	for(size_t i = 0; i < ht->set_count; i++){
		hashset_t* hs = &ht->sets[i];
		for(size_t j = 0; j < hs->size; j++)
			parse_func(hashset_get(hs, j), args);
	}
}

void hashtable_free(hashtable_t* ht){
	if(!ht)
		return;
	for(size_t i = 0; i < ht->set_count; i++){
		dynamic_array_free((dynamic_array_t*) &ht->sets[i]);
	}
}

bool arena_setup(arena_t* arena, size_t size){
	if(!arena){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	arena->size = size;
	arena->memory = (const char*) realloc((void*)arena->memory, arena->size);
	if(!arena->memory){
		DS_ERROR(DS_MEM_ERR);
		return false;
	}
	arena->ptr = arena->memory;
	return true;
}

void* arena_alloc(arena_t* arena, size_t size){
	if(!arena){
		DS_ERROR(DS_NULL_ERR);
		return NULL;
	}
	if(arena->ptr + size > arena->memory + arena->size){
		DS_ERROR(DS_BOUNDS_ERR);
		return NULL;
	}
	arena->ptr += size;
	return (void*)(arena->ptr - size);
}

bool arena_free(arena_t* arena, size_t size){
	if(!arena){
		DS_ERROR(DS_NULL_ERR);
		return false;
	}
	if(arena->ptr - size < arena->memory){
		DS_ERROR(DS_BOUNDS_ERR);
		return false;
	}
	arena->ptr -= size;
	return true;
}

void arena_destroy(arena_t* arena){
	if(!arena)
		return;
	free((void*)arena->memory);
	*arena = (arena_t) NEW_ARENA();
}