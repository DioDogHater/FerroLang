#include "tokenizer.h"
#include "datastructures.h"
#include "filemanager.h"

// All basic keywords
struct tk_keyword{
	const char* keyword;
	token_t type;
};
#define TK_KW(k) {#k, tk_##k}
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
	TK_KW(ret),
	TK_KW(sizeof),
	TK_KW(typeof),
	TK_KW(print),
	TK_KW(putchar),
	TK_KW(input),
	TK_KW(getchar),
	TK_KW(exit),
	TK_KW(or),
	TK_KW(and),
	TK_KW(xor),
	{NULL, tk_invalid}
};

// Token dynamic array
tk_array_t tk_array = NEW_DYNAMIC_ARRAY(sizeof(token));
size_t tk_index = 0;

// Macro dynamic array
macro_array_t macro_array = NEW_DYNAMIC_ARRAY(sizeof(macro));

// Push token to the back of tk_array
// Grows tk_array if needed
void tk_pushback(token tk){
	if(!dynamic_array_pushback((dynamic_array_t*) &tk_array, &tk)){
		printf("token array error: %s\n",DS_ERROR_MSG);
		exit(EXIT_FAILURE);
	}
}

// Frees tk_array
void tk_free(void){
	dynamic_array_free((dynamic_array_t*) &tk_array);
	dynamic_array_free((dynamic_array_t*) &macro_array);
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
void tk_print_context(const char* tk, uint32_t len, const char* src){
	int line_start = 1;
	for(; tk-line_start >= src && *(tk-line_start) != '\n'; line_start++);
	line_start--;
	int line_end = 1;
	for(; *(tk+line_end+len) &&  *(tk+line_end+len) != '\n'; line_end++);
	printf(
		RESET_ATTR BOLD BLACK_FG ">>>>>------------------<<<<<"
		"\n" WHITE_FG "%.*s" UNDERLINED YELLOW_FG "%.*s" NOT_UNDERLINED WHITE_FG "%.*s\n"
		BLACK_FG">>>>>------------------<<<<<" RESET_ATTR "\n\n",
		line_start, tk-line_start,
		len, tk,
		line_end, tk+len
	);
}

// Prints the token in a debugging manner
void tk_print_token(token* tk){
	printf("\"%.*s\" (%d)",(tk->str) ? tk->strlen : 4,(tk->str) ? tk->str : "null",tk->type);
}

// Compares string with token's string data
bool tk_cmp_str(token* tk, const char* str){
	for(uint32_t i = 0; i < tk->strlen; i++, str++)
		if(!(*str) || tk->str[i] != *str)
			return false;
	return !(*str);
}

// Compares token's string with length determined token
bool tk_cmp_strlen(token* tk1, const char* str, uint32_t strlen){
	if(tk1->strlen != strlen)
		return false;
	for(uint32_t i = 0; i < tk1->strlen && i < strlen; i++, str++){
		if(tk1->str[i] != *str)
			return false;
	}
	return true;
}

bool tk_error(const char* msg, token* tk, file_t* file){
	unsigned int line_number = 1;
	if(!tk)
		tk = &tk_array.tks[0];
	const char* str = tk->str;
	for(;str >= file->contents && *str; str++)
		if(*str == '\n')
			line_number++;
	printf("\n" RESET_ATTR GREEN_FG BOLD ITALIC "%s:%u" RESET_ATTR " - " RED_FG BOLD,file->path,line_number);
	puts(msg);
	tk_print_context(tk->str, tk->strlen, file->contents);
	return false;
}

#define TOKENIZE_ERR(_msg) do{ (void) tk_error((_msg),&tk,file); tk_free(); return false;}while(0)

// Tokenizes (classifies words as tokens)
// the contents of the file passed as arg
bool tokenize(file_t* file){
	bool recording_macro = false;
	if(!file->contents)
		return false;
	const char* str = file->contents;
	while(*str){
		if(recording_macro && *str == '\n' && *(str-1) != '\\'){
			tk_pushback((token){tk_end_macro,0,NULL});
			recording_macro = false;
			str++;
		}else if(isspace(*str) || isblank(*str) || !isprint(*str)){
			str++;
		}else if(isalpha(*str) || *str == '_'){
			token tk = {tk_symbol, 0, str++};
			while(*str && (isalnum(*str) || *str == '_')) str++;
			tk.strlen = str - tk.str;
			for(
				const struct tk_keyword* kw = tk_keywords;
				kw->keyword && kw->type != tk_invalid;
				kw++
			) if(tk_cmp_str(&tk, kw->keyword)){
				tk.type = kw->type;
				break;
			}
			if(tk.type == tk_symbol){
				for(size_t i = 0; i < macro_array.size; i++)
					if(tk_cmp_strlen(&tk, macro_array.macros[i].symbol, macro_array.macros[i].symbol_len)){
						// Fixed
						token* tk_ptr = &tk_array.tks[macro_array.macros[i].macro_start+1];
						size_t macro_size = 0;
						while(tk_ptr->type != tk_end_macro){
							macro_size++;
							tk_ptr++;
						}
						dynamic_array_grow((dynamic_array_t*)&tk_array, macro_size);

						tk_ptr -= macro_size;
						while(tk_ptr->type != tk_end_macro){
							tk_pushback(*tk_ptr);
							tk_ptr++;
						}
						tk.type = tk_invalid;
						break;
					}
			}
			if(tk.type != tk_invalid)
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
					}else
						TOKENIZE_ERR("invalid char literal");
					break;
				case '"':
					str++;
					while(*str != '"'){
						if(!(*str)){
							tk.strlen = str - tk.str;
							TOKENIZE_ERR("invalid string literal");
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
					if(*(str+1) == '/'){
						tk.type = tk_invalid;
						while(*str && *str != '\n') str++;
					}else
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
				case '>':
					if(*(str+1) == '=')
						tk = (token){tk_cmp_geq, 2, str++};
					else
						tk.type = tk_cmp_g;
					break;
				case '<':
					if(*(str+1) == '=')
						tk = (token){tk_cmp_leq, 2, str++};
					else
						tk.type = tk_cmp_l;
					break;
				case '?':
					if(*(str+1) == '=')
						tk = (token){tk_cmp_type, 2, str++};
					else
						tk.type = tk_question;
					break;
				case '!':
					if(*(str+1) == '=')
						tk = (token){tk_cmp_neq, 2, str++};
					else
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
					if(str > file->contents && *(str-1) != '\n')
						TOKENIZE_ERR("preprocessor directive needs to be at start of line");
					str++;
					for(;isalpha(*str);str++);
					tk.strlen = str - tk.str;
					if(tk_cmp_str(&tk, "#include")){
						while(isspace(*str)){
							if(*str == '\n')
								TOKENIZE_ERR("expected include path");
							str++;
						}
						if(*str != '"')
							TOKENIZE_ERR("expected header file path after include");
						tk.str = ++str;
						while(*str != '"'){
							if(!(*str) || *str == '\n')
								TOKENIZE_ERR("expected valid header file path after include");
							str++;
						}
						tk.type = tk_include;
						tk.strlen = str - tk.str;
						tk_pushback(tk);
						char* file_path = (char*) malloc(tk.strlen+1);
						memcpy((void*)file_path,(void*)tk.str,tk.strlen);
						file_path[tk.strlen] = '\0';
						file_t include_file = new_file(file_path);
						if(!load_file(&include_file)){
							tk_free();
							return false;
						}
						append_file_list(include_file);
						if(!tokenize(&include_file))
							return false;
						tk.type = tk_end_include;
						tk.str = file->path;
						tk.strlen = strlen(file->path);
					}else if(tk_cmp_str(&tk, "#define")){
						while(isspace(*str)){
							if(*str == '\n')
								TOKENIZE_ERR("macro has no name");
							str++;
						}
						tk.str = str;
						if(!isalpha(*str))
							TOKENIZE_ERR("macro name should start with a letter (A-Z)");
						str++;
						while(isalnum(*str) || *str == '_') str++;
						tk = (token) {tk_macro, str - tk.str, tk.str};
						for(size_t i = 0; i < macro_array.size; i++)
							if(tk_cmp_strlen(&tk, macro_array.macros[i].symbol, macro_array.macros[i].symbol_len))
								TOKENIZE_ERR("macro is redefined");
						macro new_macro = {tk.str, tk_array.size, tk.strlen};
						dynamic_array_pushback((dynamic_array_t*) &macro_array, &new_macro);
						recording_macro = true;
					}else if(tk_cmp_str(&tk, "#ifdef")){
						while(isspace(*str)){
							if(*str == '\n')
								TOKENIZE_ERR("expected macro name");
							str++;
						}
						tk.str = str;
						if(!isalpha(*str))
							TOKENIZE_ERR("macro name should start with a letter (A-Z)");
						str++;
						while(isalnum(*str) || *str == '_') str++;
						tk = (token) {tk_ifdef, str - tk.str, tk.str};
					}else if(tk_cmp_str(&tk, "#ifndef")){
						while(isspace(*str)){
							if(*str == '\n')
								TOKENIZE_ERR("expected macro name");
							str++;
						}
						tk.str = str;
						if(!isalpha(*str))
							TOKENIZE_ERR("macro name should start with a letter (A-Z)");
						str++;
						while(isalnum(*str) || *str == '_') str++;
						tk = (token) {tk_ifndef, str - tk.str, tk.str};
					}else if(tk_cmp_str(&tk, "#endif")){
						tk.type = tk_endif;
					}else
						TOKENIZE_ERR("unknown preprocessor directive:");
					break;
				case '\\':
					str += 2;
					break;
				default:
					TOKENIZE_ERR("unexpected character:");
			}
			if(tk.type == tk_invalid)
				continue;
			tk_pushback(tk);
			str++;
		}
	}
	if(recording_macro){
		printf("%s: macro was not completed before end of file!\n", file->path);
		tk_free();
		return false;
	}
	return true;
}
