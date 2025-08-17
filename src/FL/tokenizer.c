#include "tokenizer.h"
#include "filemanager.h"
#include <stdlib.h>
#include <string.h>

// All basic keywords
static const struct tk_keyword tk_keywords[] = {
	TK_KW(const),
	TK_KW(constexpr),
	TK_KW(char),
	TK_KW(i8),
	TK_KW(i16),
	TK_KW(i32),
	TK_KW(i64),
	TK_KW(bool),
	TK_KW(str),
	TK_KW(arr),
	TK_KW(ptr),
	{"true",tk_bool_lit},
	{"false",tk_bool_lit},
	TK_KW(if),
	TK_KW(elif),
	TK_KW(else),
	TK_KW(while),
	TK_KW(for),
	TK_KW(func),
	TK_KW(ret),
	TK_KW(or),
	TK_KW(and),
	TK_KW(xor),
	{NULL, tk_invalid}
};

// Token dynamic array
struct tk_array tk_array = {0,0,NULL};
size_t tk_index = 0;

// Push token to the back of tk_array
// Grows tk_array if needed
void tk_pushback(token tk){
	if(tk_array.memsize - tk_array.size < 1){
		tk_array.memsize = (tk_array.memsize) ? tk_array.memsize+32 : 32;
		tk_array.tks = realloc(tk_array.tks, tk_array.memsize*sizeof(token));
		if(!tk_array.tks){
			printf("failed to allocate %lu bytes for tokens!\n",tk_array.memsize*sizeof(token));
			exit(EXIT_FAILURE);
		}
	}
	tk_array.tks[tk_array.size++] = tk;
}

// Frees tk_array
void tk_free(void){
	if(tk_array.tks)
		free(tk_array.tks);
	tk_array = (struct tk_array){0,0,NULL};
}

// Get the nth token after current index,
// without changing the index
token* tk_peek(int n){
	if(tk_index+n < 0 || tk_index+n >= tk_array.size)
		return NULL;
	return &tk_array.tks[tk_index+n];
}

// Consume the nth token after current index,
// incrementing the index by one
token* tk_consume(int n){
	if(tk_index+n < 0 || tk_index+n >= tk_array.size)
		return NULL;
	return &tk_array.tks[n+(tk_index++)];
}

// Prints the full line of code where the token comes from
void tk_print_context(const char* tk, const char* src){
	int line_start = 1;
	for(; tk-line_start >= src && *(tk-line_start) != '\n'; line_start++);
	line_start--;
	int line_end = 1;
	for(; *(tk+line_end) &&  *(tk+line_end) != '\n'; line_end++);
	printf("%.*s\n",line_start+line_end,tk-line_start);
}

// Prints the token in a debugging manner
void tk_print(token* tk){
	printf("\"%.*s\" (%d)",tk->strlen,tk->str,tk->type);
}

// Compares string with token's string data
bool tk_cmp_str(token* tk, const char* str){
	for(uint16_t i = 0; i < tk->strlen; i++, str++)
		if(!(*str) || tk->str[i] != *str)
			return false;
	return true;
}

static void tk_error(const char* msg, token* tk, file_t* file){
	printf("%s:",file->path);
	puts(msg);
	tk_print_context(tk->str, file->contents);
}

#define TOKENIZE_ERR() do{tk_free(); return false;}while(0)

// Tokenizes (classifies words as tokens)
// the contents of the file passed as arg
bool tokenize(file_t* file){
	if(!file->contents)
		return false;
	const char* str = file->contents;
	while(*str){
		if(isspace(*str) || isblank(*str) || !isprint(*str))
			str++;
		else if(isalpha(*str) || *str == '_'){
			token tk = {tk_symbol, 0, str++};
			while(*str && (isalnum(*str) || *str == '_')) str++;
			tk.strlen = str - tk.str;
			for(
				const struct tk_keyword* kw = tk_keywords;
				kw->keyword && kw->type != tk_invalid;
				kw++
			) if(tk_cmp_str(&tk, kw->keyword))
				tk.type = kw->type;
			tk_pushback(tk);
		}else if(isdigit(*str)){
			token tk = {tk_int_lit,0,str++};
			while(*str && isdigit(*str))
				str++;
			if(*str == '.'){
				str++;
				tk.type = tk_float_lit;
				while(*str && isdigit(*str)) str++;
			}
			tk.strlen = str - tk.str;
			tk_pushback(tk);
		}else{
			token tk = {tk_invalid,1,str};
			switch(*str){
				case '\'':
					if(*(str+1) && *(str+2) == '\''){
						tk = (token){tk_char_lit,1,str+1};
						str += 2;
					}else if(*(str+1) == '\\' && *(str+2) && *(str+3) == '\''){
						tk = (token){tk_char_lit,2,str+1};
						str += 3;
					}else{
						tk_error("invalid char literal:",&tk,file);
						TOKENIZE_ERR();
					}
					break;
				case '"':
					str++;
					while(*str != '"'){
						if(!(*str)){
							tk_error("invalid string literal:",&tk,file);
							TOKENIZE_ERR();
						}
						str++;
					}
					tk = (token){tk_str_lit, str - tk.str - 1, tk.str+1};
					break;
				case '+':
					tk.type = tk_plus;
					break;
				case '-':
					tk.type = tk_minus;
					break;
				case '*':
					tk.type = tk_mul;
					break;
				case '/':
					tk.type = tk_div;
					break;
				case '%':
					tk.type = tk_mod;
					break;
				case '=':
					if(*(str+1) == '=' && *(str+2) == '='){
						tk = (token){tk_cmp_strict, 3, str};
						str += 2;
					}else if(*(str+1) == '=')
						tk = (token){tk_cmp_eq, 2, str++};
					else
						tk.type = tk_assign;
					break;
				case '?':
					tk.type = tk_question;
					break;
				case '!':
					tk.type = tk_exclam;
					break;
				case ':':
					tk.type = tk_colon;
					break;
				case ';':
					tk.type = tk_semicolon;
					break;
				case ',':
					tk.type = tk_comma;
					break;
				case '.':
					tk.type = tk_dot;
					break;
				case '(':
					tk.type = tk_oparent;
					break;
				case ')':
					tk.type = tk_cparent;
					break;
				case '{':
					tk.type = tk_obrace;
					break;
				case '}':
					tk.type = tk_cbrace;
					break;
				case '[':
					tk.type = tk_obracket;
					break;
				case ']':
					tk.type = tk_cbracket;
					break;
				case '#':
					tk.str++;
					if(tk_cmp_str(&tk, "include")){
						str += 8;
						while(isspace(*str)){
							if(*str == '\n'){
								tk_error("expected include path:",&tk,file);
								TOKENIZE_ERR();
							}
							str++;
						}
						if(*str != '"'){
							tk_error("expected header file path after include:",&tk,file);
							TOKENIZE_ERR();
						}
						tk.str = ++str;
						while(*str != '"'){
							if(!(*str) || *str == '\n'){
								tk_error("expected valid header file path after include:",&tk,file);
								TOKENIZE_ERR();
							}
							str++;
						}
						tk.type = tk_include;
						tk.strlen = str - tk.str;
						if(str - tk.str > 255){
							tk_error("header file path is too long (255 character max limit!):",&tk,file);
							TOKENIZE_ERR();
						}
						tk_pushback(tk);
						char file_path[255];
						memcpy((void*)file_path,(void*)tk.str,tk.strlen);
						file_path[tk.strlen] = '\0';
						file_t include_file = new_file(file_path);
						if(!load_file(&include_file))
							TOKENIZE_ERR();
						append_file_list(include_file);
						if(!tokenize(&include_file))
							return false;
						tk.type = tk_end_include;
						tk.str = file->path;
						tk.strlen = strlen(file->path);
					}else if(tk_cmp_str(&tk, "define")){
						tk_error("not implemented yet.",&tk,file);
						TOKENIZE_ERR();
					}else{
						tk_error("unknown preprocessor directive:",&tk,file);
						TOKENIZE_ERR();
					}
					break;
				default:
					printf("%s: unexpected character: \'%c\'\n",file->path,*str);
					tk_print_context(tk.str,file->contents);
					TOKENIZE_ERR();
			}
			tk_pushback(tk);
			str++;
		}
	}
	return true;
}
