#include "filemanager.h"

bool load_file(file_t* file){
	FILE* fptr = fopen(file->path, "rb");
	if(!fptr){
		perror("Failed to open file");
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

bool append_start_file(file_t* dest, const char* path){
	file_t src = new_file(path);
	if(!load_file(&src)) return false;;
	dest->contents = (char*) realloc((void*)dest->contents, dest->size + src.size);
	memcpy((void*)dest->contents+src.size,(void*)src.contents, dest->size);
	memcpy((void*)dest->contents,(void*)src.contents,src.size);
	close_file(&src);
	return true;
}

bool append_end_file(file_t* dest, const char* path){
	file_t src = new_file(path);
	if(!load_file(&src)) return false;;
	dest->contents = (char*) realloc((void*)dest->contents, dest->size + src.size);
	memcpy((void*)dest->contents+dest->size,(void*)src.contents,src.size);
	close_file(&src);
	return true;
}