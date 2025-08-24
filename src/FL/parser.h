#ifndef FERRO_PARSER_H
#define FERRO_PARSER_H

#include "datastructures.h"
#include "filemanager.h"
#include "tokenizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

extern arena_t parser_arena;
extern file_t* parser_file;

typedef token_t node_t;

union node_expr;
union node_stmt;

typedef struct{
	node_t type;	// tk_binexpr
	token_t op;		// operator (tk_plus, tk_minus, tk_mul, etc.)
	union node_expr *lhs, *rhs;
} node_binexpr;

typedef struct{
	node_t type;	// tk_func_call
	token* symbol;
	union node_expr* exprs;
	size_t expr_count;
} node_func_call;

typedef union node_expr {
	node_t type;
	token char_lit;			// tk_char_lit
	token int_lit;			// tk_int_lit
	token float_lit;		// tk_float_lit
	token str_lit;			// tk_str_lit
	token symbol;			// tk_symbol
	node_func_call func_call;// tk_func_call
	node_binexpr binexpr;	// tk_binexpr
} node_expr;

typedef struct{
	node_t type;	// tk_scope
	union node_stmt* stmts;
	size_t stmt_count;
} node_scope;

typedef struct{
	node_t type;	// tk_var_decl
	token_t var_type;
	token* symbol;
	node_expr* expr;
} node_var_decl;

typedef struct{
	node_t type;	// tk_var_assign
	token* symbol;
	node_expr expr;
} node_var_assign;

typedef struct{
	node_t type;	// tk_exit
	node_expr expr;
} node_exit;

typedef union node_stmt {
	node_t type;
	node_var_decl var_decl;
	node_var_assign var_assign;
	node_func_call func_call;
	node_scope scope;
	node_exit exit;
} node_stmt;

typedef DYNAMIC_ARRAY(node_stmt* stmts) node_prog;

int8_t tk_bin_prec(token*);
bool parse_term_expr(node_expr*);
bool parse_expr(node_expr*,int8_t);
bool parse_cmp(node_expr*);
bool parse_condition(node_expr*);
bool parse_stmt(node_stmt*);
bool parse_scope(node_scope*);
void parser_free_stmt(node_stmt*);
void parser_free(node_prog*);

bool parse(node_prog*,file_t*);

#endif