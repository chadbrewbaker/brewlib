#ifndef _BLIB_UNIT_TEST_
#define _BLIB_UNIT_TEST_
#define BLIB_UNIT_TEST 1

/*Register all includes and unit tests*/
#include "blib_error.h"
#include "blib_partition.h"
#include "blib_graph_auto.h"
#include "blib_graph.h"
#include "blib_graph_transform.h"
#include "blib_schreier.h"
int BLIB_UNIT_PASS_COUNT,BLIB_UNIT_FAIL_COUNT;

void blib_unit_report(int i,char* test_name){
	if(!i){
		printf("Passed %s\n",test_name);
		BLIB_UNIT_PASS_COUNT++;
	}
	else{
		printf("Failed %s\n",test_name);
		BLIB_UNIT_FAIL_COUNT++;
	}
}

void blib_unit_test(void){
	BLIB_UNIT_PASS_COUNT=BLIB_UNIT_FAIL_COUNT=0;
	printf("Starting Unit tests\n");
	blib_unit_report(blib_partition_unit(),"partition");
	blib_unit_report(blib_graph_unit(),"graph");
	blib_unit_report(blib_graph_auto_unit(),"graph_auto");
	blib_unit_report(blib_graph_transform_unit(),"graph_transform");
	blib_unit_report(blib_schreier_unit(),"schreier");
	printf("Passed %d, Failed %d\n",BLIB_UNIT_PASS_COUNT,BLIB_UNIT_FAIL_COUNT);
}

#endif