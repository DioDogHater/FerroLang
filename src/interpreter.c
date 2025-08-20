#include "FL/filemanager.h"
#include "FL/textstyle.h"
#include "FL/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	char* input_file = NULL;
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-')
		switch(argv[i][1]){
		case 'h':
			show_usage(NULL);
		case '-':

		default:{
			char tmp[32];
			sprintf(tmp,"Invalid argument %.*s",2,argv[i]);
			show_usage(tmp);
		}}
		else if(input_file)
			show_usage("Too many input files (only one required).");
		else
			input_file = argv[i];
	}

	file_t main_file = {input_file,NULL,0};
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