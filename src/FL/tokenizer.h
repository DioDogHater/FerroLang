#ifndef FERRO_TOKENIZER_H
#define FERRO_TOKENIZER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

#include "filemanager.h"

enum{
	tk_invalid = 0xFFFF,

	// Pimitive data types and literals
	tk_char = 0,
	tk_char_lit,
	tk_i8,
	tk_u8,
	tk_i16,
	tk_u16,
	tk_i32,
	tk_u32,
	tk_i64,
	tk_u64,
	tk_int_lit,
	tk_f32,
	tk_f64,
	tk_float_lit,
	tk_str,
	tk_str_lit,
	tk_bool,
	tk_bool_lit,

	// Variable / data keywords
	tk_arr,
	tk_ptr,
	tk_dict,
	tk_list,
	tk_const,
	tk_constexpr,

	// Loops / functions
	tk_if,
	tk_elif,
	tk_else,
	tk_while,
	tk_for,
	tk_break,
	tk_func,
	tk_ret,

	// Operators
	tk_plus,
	tk_minus,
	tk_mul,
	tk_div,
	tk_mod,
	tk_assign,
	tk_question,
	tk_exclam,
	tk_colon,
	tk_semicolon,
	tk_ampersand,
	tk_comma,
	tk_dot,
	tk_oparent,		// ()
	tk_cparent,
	tk_obrace,		// {}
	tk_cbrace,
	tk_obracket,	// []
	tk_cbracket,

	// Comparison operators
	tk_cmp_eq,
	tk_cmp_neq,
	tk_cmp_l,
	tk_cmp_leq,
	tk_cmp_g,
	tk_cmp_geq,
	tk_cmp_strict,
	tk_cmp_type,

	// Logical operators
	tk_and,
	tk_or,
	tk_xor,

	// Preprocessor commands
	tk_include,		// Determines start / end of included file tokens
	tk_end_include,

	// Unknown symbol, handled during parsing
	tk_symbol,
};
typedef uint16_t token_t;

typedef struct{
	token_t type;
	uint16_t strlen;
	const char* str;
} token;

struct tk_keyword{
	const char* keyword;
	token_t type;
};
#define TK_KW(k) {#k, tk_##k}

struct tk_array{
	size_t size;
	size_t memsize;
	token* tks;
};
extern struct tk_array tk_array;
extern size_t tk_index;

void tk_pushback(token);
void tk_free(void);
token* tk_peek(int);
token* tk_consume(int);
void tk_print_context(const char*,const char*);
void tk_print(token*);
bool tk_cmp_str(token*,const char*);

bool tokenize(file_t*);

#endif