#ifndef FERRO_FILE_MANAGER_H
#define FERRO_FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct{
	const char* path;
	const char* contents;
	size_t size;
} file_t;
#define new_file(p) {(p),NULL,0}

bool load_file(file_t*);
void close_file(file_t*);
bool append_start_file(file_t*,const char*);
bool append_end_file(file_t*,const char*);

#endif