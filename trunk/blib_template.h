#ifndef _BLIB_TEMPLATE_DEF_
#define _BLIB_TEMPLATE_DEF_
#include "blib_util.h"

typedef struct blib_template_t{
	int size;
	int *data;
}blib_template;

void blib_template_allocate(blib_template* t, int size){
	t=BLIB_MALLOC(sizeof(blib_template)*size);	
}
void blib_template_free(blib_template* t){}
void blib_template_copy(blib_template* a, blib_template* b){}
int blib_template_compare(blib_template* a, blib_template* b){}
#endif /*_BLIB_TEMPLATE_DEF_*/
