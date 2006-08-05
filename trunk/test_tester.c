#include "blib_error.h"
#include <stdio.h>


int main(){

	BLIB_ERROR("");
	BLIB_ERROR("frog");
	BLIB_ERROR("%d frogs",2);
	return 0;
}
