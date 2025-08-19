#include "FL/filemanager.h"
#include "FL/tokenizer.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	file_t main_file = new_file("examples/test.fs");
	load_file(&main_file);
	append_file_list(main_file);

	if(!tokenize(&main_file))
		return EXIT_FAILURE;

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