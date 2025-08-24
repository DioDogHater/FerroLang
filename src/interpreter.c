#include "FL/filemanager.h"
#include "FL/textstyle.h"
#include "FL/tokenizer.h"
#include "FL/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FERRO_DEBUG

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

void cleanup(node_prog* prog){
	parser_free(prog);
	tk_free();
	free_file_list();
}

int main(int argc, char* argv[]){
	if(argc < 2)
		show_usage("Missing input file.");

	printf("tk = %lu vs tk* = %lu\n",sizeof(token),sizeof(token*));
	char* input_file = NULL;
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-')
		switch(argv[i][1]){
		case 'h':
			show_usage(NULL);
		case '-':
			if(!strcmp(argv[i],"--help"))
				show_usage(NULL);
			else{
				char tmp[512];
				sprintf(tmp,"Invalid argument %.*s",450,argv[i]);
				show_usage(tmp);
			}
			break;
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

	if(!tokenize(&main_file)){
		cleanup(NULL);
		return EXIT_FAILURE;
	}

	printf(GREEN_FG BOLD "Tokenization complete!" RESET_ATTR "\n");

#ifdef FERRO_DEBUG
	{
	printf("TOKENS:\n");
	token* tk;
	while((tk = tk_consume(0))){
		tk_print_token(tk);
		putchar(' ');
	}
	putchar('\n');
	}
#endif

	node_prog prog;
	if(!parse(&prog, &main_file)){
		cleanup(&prog);
		return EXIT_FAILURE;
	}

	printf(GREEN_FG BOLD "Parsing complete!" RESET_ATTR "\n");

	cleanup(&prog);
	return 0;
}