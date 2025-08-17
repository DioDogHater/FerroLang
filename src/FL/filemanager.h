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

struct file_list_t;
typedef struct file_list_t{
	file_t f;
	struct file_list_t* next;
} file_list_t;

extern file_list_t file_list;

bool load_file(file_t*);
void close_file(file_t*);

void append_file_list(file_t);
void free_file_list(void);

#endif