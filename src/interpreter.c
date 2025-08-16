#include "FL/tokenizer.h"
#include <stdio.h>

int main(int argc, char* argv[]){
	file_t main_file = new_file("examples/test.fl");
	load_file(&main_file);

	tokenize(&main_file);

	token* tk;
	while((tk = tk_consume(0))){
		tk_print(tk);
		putchar(' ');
	}
	putchar('\n');

	tk_free();
	close_file(&main_file);

	return 0;
}