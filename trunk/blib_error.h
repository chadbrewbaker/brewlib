#ifndef _BLIB_ERROR_DEF_
#define _BLIB_ERROR_DEF_
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void blib_error_location(char* file_name,int line){
	fprintf(stderr,":%s (%d)\n",file_name,line);
}


#define BLIB_ERROR(...) fprintf(stderr,__VA_ARGS__);blib_error_location(__FILE__,__LINE__);fflush(stderr);

void* blib_error_malloc(size_t size, char* file,int line){
	void* ptr=malloc(size);
	if(ptr==NULL){
		fprintf(stderr,"%s(%d):MALLOC ERROR\n",file,line);
		exit(0);
	}
	return ptr;
}
#define BLIB_MALLOC(size) blib_error_malloc(size,__FILE__,__LINE__);

void blib_error_tabs(int tabs){
	int i;
	for(i=0;i<tabs;i++)
		fprintf(stderr,"\t");
}


#endif /*_BLIB_ERROR_DEF_*/