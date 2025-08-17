#include "filemanager.h"
#include <stdlib.h>

file_list_t file_list = {new_file(NULL),NULL};

bool load_file(file_t* file){
	FILE* fptr = fopen(file->path, "rb");
	if(!fptr){
		printf("failed to open %s\n", file->path);
		perror(NULL);
		return false;
	}
	fseek(fptr,0L,SEEK_END);
	file->size = ftell(fptr);
	rewind(fptr);
	if(file->size <= 0){
		fclose(fptr);
		printf("File %s has invalid size!\n",file->path);
		return false;
	}
	file->contents = (const char*)realloc((void*)file->contents,file->size);
	if(!file->contents){
		fclose(fptr);
		printf("Failed to allocate memory for file %s\n",file->path);
		return false;
	}
	if(fread((void*)file->contents,1,file->size,fptr) != file->size){
		fclose(fptr);
		printf("Failed to read file %s\n",file->path);
		return false;
	}
	fclose(fptr);
	return true;
}

void close_file(file_t* file){
	if(file->contents)
		free((void*)file->contents);
	file->contents = NULL;
}

void append_file_list(file_t file){
	file_list_t* node = (file_list_t*) malloc(sizeof(file_list_t));
	if(!node){
		printf("failed to allocate %lu bytes for file list\n",sizeof(file_list_t));
		exit(EXIT_FAILURE);
	}
	*node = (file_list_t){file,NULL};
	file_list_t* ptr = &file_list;
	while(ptr->next) ptr = ptr->next;
	ptr->next = node;
}

void free_file_list(void){
	file_list_t* ptr = file_list.next;
	if(!ptr) return;
	while(ptr->next){
		void* node = ptr;
		ptr = ptr->next;
		free(node);
	}
}