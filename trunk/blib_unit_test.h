#ifndef _BLIB_UNIT_TEST_
#define _BLIB_UNIT_TEST_
#define BLIB_UNIT_TEST 1

/*Register all includes and unit tests*/
#include "blib_error.h"
#include "blib_partition.h"
#include "blib_graph_auto.h"
#include "blib_graph.h"
#include "blib_graph_transform.h"

void blib_unit_test(void){
	blib_partition_unit();
	BLIB_ERROR("Finished partition unit tests");
	blib_graph_unit();
	BLIB_ERROR("Finished graph unit tests");
	blib_graph_auto_unit();
	BLIB_ERROR("Finished graph_auto unit tests");
	blib_graph_transform_unit();
	BLIB_ERROR("Finished graph_transform tests");
	BLIB_ERROR("Finished all unit tests");
}

#endif