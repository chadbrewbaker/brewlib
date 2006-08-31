#ifndef _BLIB_GARBAGE_DEF_
#define _BLIB_GARBAGE_DEF_
#include "blib_error.h"

/*Frees unused temp storage. ALWAYS include this LAST!!!*/
/*Register all headers that use temp storage*/
void blib_garbage()
{
#ifdef _BLIB_PARTITION_DEF_
	blib_partition_garbage();
#endif
	return;	
}
#endif /*_BLIB_GARBAGE_*/