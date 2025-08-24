#include "parser.h"
#include "textstyle.h"
#include "datastructures.h"
#include "tokenizer.h"

arena_t parser_arena;
file_t* parser_file;

static void parser_arena_error(const char* func){
	printf(RED_FG BOLD "parser arena - %s:" YELLOW_FG " %s" RESET_ATTR "\n",func,DS_ERROR_MSG);
	exit(EXIT_FAILURE);
}

int8_t tk_bin_prec(token* tk){
	switch(tk->type){
	case tk_plus:
	case tk_minus:
		return 0;
	case tk_mul:
	case tk_div:
	case tk_mod:
		return 1;
	default:
		return -1;
	}
}

bool parse_term_expr(node_expr* expr){
	if(!expr || !tk_peek(0))
		return false;
	switch(tk_peek(0)->type){
	case tk_char_lit:
		*expr = (node_expr){.char_lit=*tk_consume(0)};
		break;
	case tk_int_lit:
		*expr = (node_expr){.int_lit=*tk_consume(0)};
		break;
	case tk_float_lit:
		*expr = (node_expr){.float_lit=*tk_consume(0)};
		break;
	case tk_str_lit:
		*expr = (node_expr){.str_lit=*tk_consume(0)};
		break;
	case tk_symbol:
		if(tk_peek(1) && tk_peek(1)->type == tk_oparent){
			*expr = (node_expr){.func_call={tk_func_call, tk_consume(0), (node_expr*)parser_arena.ptr, 0}};
			(void) tk_consume(0);
			while(true){
				node_expr* arg = (node_expr*) arena_alloc(&parser_arena,sizeof(node_expr));
				if(!arg)
					parser_arena_error("parse_term_expr");
				if(!parse_expr(arg,0))
					return tk_error("expected valid expression as argument",tk_peek(-1),parser_file);
				expr->func_call.expr_count++;
				if(tk_peek(0)){
					if(tk_peek(0)->type == tk_comma){
						(void) tk_consume(0);
						continue;
					}else if(tk_peek(0)->type == tk_cparent)
						(void) tk_consume(0);
					else
						return tk_error("unexpected token",tk_peek(0),parser_file);
				} break;
			}
			if(tk_peek(-1)->type != tk_cparent)
				tk_error("expected ')'",tk_peek(-1),parser_file);
		}else
			*expr = (node_expr){.symbol=*tk_consume(0)};
		break;
	case tk_minus:{
		tk_consume(0);
		*expr = (node_expr){.binexpr = {tk_binexpr, tk_negation, NULL, NULL}};
		expr->binexpr.lhs = (node_expr*) arena_alloc(&parser_arena, sizeof(node_expr));
		if(!expr->binexpr.lhs)
			parser_arena_error("parse_term_expr");
		if(!parse_expr(expr->binexpr.lhs, 0))
			return tk_error("expected valid expression",tk_peek(-1),parser_file);
		break;
	}case tk_oparent:{
		tk_consume(0);
		*expr = (node_expr){.binexpr = {tk_binexpr, tk_oparent, NULL, NULL}};
		expr->binexpr.lhs = (node_expr*) arena_alloc(&parser_arena, sizeof(node_expr));
		if(!expr->binexpr.lhs)
			parser_arena_error("parse_term_expr");
		if(!parse_expr(expr->binexpr.lhs, 0))
			return tk_error("expected valid expression",tk_peek(-1),parser_file);
		if(tk_peek(0) && tk_peek(0)->type != tk_cparent)
			return tk_error("expected ')'",tk_peek(-1),parser_file);
		(void) tk_consume(0);
		break;
	}default:
		return false;
	}
	return true;
}

bool parse_expr(node_expr* expr, int8_t min_prec){
	if(!expr)
		parser_arena_error("parse_expr (arg)");
	if(parse_term_expr(expr)){
		while(true){
			token* op_token = tk_peek(0);
			if(!op_token)
				break;
			int8_t op_prec = tk_bin_prec(op_token);
			if(op_prec == -1 || op_prec < min_prec)
				break;
			(void) tk_consume(0);

			// Set the left hand side as expr
			node_expr* expr_lhs = (node_expr*) arena_alloc(&parser_arena, sizeof(node_expr));
			if(!expr_lhs)
				parser_arena_error("parse_expr");
			*expr_lhs = *expr;

			// Setup the binary expression
			*expr = (node_expr){.binexpr = {tk_binexpr, op_token->type, expr_lhs, NULL}};

			// Parse the right hand size
			expr->binexpr.rhs = (node_expr*) arena_alloc(&parser_arena, sizeof(node_expr));
			if(!expr->binexpr.rhs)
				parser_arena_error("parse_expr");
			if(!parse_expr(expr->binexpr.rhs, op_prec + 1))
				return tk_error("expected expression",tk_peek(-1),parser_file);
		}
		return true;
	}
	return false;
}

bool parse_stmt(node_stmt* stmt){
	if(!stmt)
		return false;
	if(!tk_peek(0))
		return tk_error("expected token",tk_peek(-1),parser_file);
	switch(tk_peek(0)->type){
	case tk_const:
	case tk_char:
	case tk_i8:
	case tk_u8:
	case tk_i16:
	case tk_u16:
	case tk_i32:
	case tk_u32:
	case tk_i64:
	case tk_u64:
	case tk_str:
	case tk_bool:{
		bool var_const = (tk_peek(0)->type == tk_const);
		if(var_const) tk_consume(0);
		if(!tk_peek(0))
			return tk_error("expected token",tk_peek(-1),parser_file);
		token_t type = tk_consume(0)->type;
		if(tk_peek(0) && tk_peek(0)->type != tk_symbol)
			return tk_error("expected symbol after type",tk_peek(-1),parser_file);
		token* symbol = tk_consume(0);
		node_expr* expr = NULL;
		if(tk_peek(0) && tk_peek(0)->type == tk_assign){
			tk_consume(0);
			expr = (node_expr*) arena_alloc(&parser_arena,sizeof(node_expr));
			if(!expr)
				parser_arena_error("parse_stmt");
			if(!parse_expr(expr,0))
				return tk_error("expected valid expression",tk_peek(-1),parser_file);
			if(!tk_peek(0) || tk_peek(0)->type != tk_semicolon)
				return tk_error("expected semicolon",tk_peek(-1),parser_file);
			(void) tk_consume(0);
		}else if(!tk_peek(0) || tk_peek(0)->type != tk_semicolon)
			return tk_error("expected semicolon",tk_peek(-1),parser_file);
		*stmt = (node_stmt){.var_decl=(node_var_decl){tk_var_decl,type,symbol,expr}};
		break;
	}default:
		return tk_error("not implemented yet",tk_peek(0),parser_file);
	}
	return true;
}

void parser_free(node_prog* prog){
	if(prog)
		dynamic_array_free((dynamic_array_t*)prog);
	arena_destroy(&parser_arena);
}

bool parse(node_prog* prog, file_t* file){
	parser_file = file;
	tk_index = 0;
	if(!arena_setup(&parser_arena, 256*KB))
		parser_arena_error("parse");
	*prog = (node_prog) NEW_DYNAMIC_ARRAY(sizeof(node_stmt));
	while(tk_peek(0)){
		node_stmt stmt;
		if(!parse_stmt(&stmt))
			return false;
		dynamic_array_pushback((dynamic_array_t*)prog, (const void*)&stmt);
	}
	return true;
}