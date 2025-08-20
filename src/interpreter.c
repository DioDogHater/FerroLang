#include "FL/filemanager.h"
#include "FL/textstyle.h"
#include "FL/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

static void show_usage(const char* msg){
	if(msg){
		printf(BOLD YELLOW_FG "! ");
		puts(msg);
	}
	printf(
		RESET_ATTR "Usage:" BOLD DEFAULT_FG " ferro_interpreter [-options] <main.fs>\n"
		RESET_ATTR "Options:\n"
		"	-h : Help\n"
	);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	if(argc < 2)
		show_usage("Missing input file.");
	file_t main_file = {NULL,NULL,0};
	load_file(&main_file);
	append_file_list(main_file);

	if(!tokenize(&main_file))
		return EXIT_FAILURE;

	printf("\x1b[32;1mTokenization complete!\x1b[0m\n");

	token* tk;
	while((tk = tk_consume(0))){
		tk_print_token(tk);
		putchar(' ');
	}
	putchar('\n');

	tk_free();
	free_file_list();

	return 0;
}