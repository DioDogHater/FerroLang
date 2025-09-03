#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../FL/datastructures.h"
#include "../FL/tokenizer.h"

enum{
	var_char = 0,
	var_u8,
	var_i8,
	var_u16,
	var_i16,
	var_u32,
	var_i32,
	var_u64,
	var_i64,
	var_str,
	var_bool,
	var_const = 0x80
};
#define VAR_TYPE(x) ((x) & 0x7F)
#define VAR_CONST(x) ((x) & 0x80)

typedef uint8_t var_type;

typedef struct{
	const char* str;
	size_t size;
} var_symbol;

typedef struct{
	void* value;
	var_type type;
} variable_t;

typedef struct{
	var_symbol name;
	variable_t var;
} var_pair;

hashtable_t variables = (hashtable_t) NEW_HASHTABLE(10);

static bool cmp_strlen(const char* str1, size_t strlen1, const char* str2, size_t strlen2){
	if(strlen1 != strlen2)
		return false;
	for(size_t i = 0; i < strlen1; i++, str1++, str2++)
		if(*str1 != *str2)
			return false;
	return true;
}

static bool cmp_var_pair(const void* a, const void* b){
	return cmp_strlen(
		((var_symbol*)a)->str, ((var_symbol*)a)->size,
		((var_symbol*)b)->str, ((var_symbol*)b)->size
	);
}

static size_t hash_var_pair(const void* p){
	size_t result = ((var_symbol*)p)->size;
	const char* str = ((var_symbol*)p)->str;
	for(size_t i = 0; i < ((var_symbol*)p)->size; i++, str++)
		result += *str;
	return result;
}

bool setup_variables(){
	if(!hashtable_setup(&variables, sizeof(var_pair))){
		printf("variable table: %s\n", DS_ERROR_MSG);
		return false;
	}
	variables.cmp_func = cmp_var_pair;
	variables.hashing_func = hash_var_pair;
	return true;
}

variable_t* get_variable(var_symbol name){

}

#endif