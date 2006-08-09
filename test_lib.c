#include <stdlib.h>
#include <stdio.h>
#include "blib_graph.h"
#include "blib_graph_auto.h"
int main(){
	blib_graph* graph;
	int i;
	int orbits[4];
	for(i=0;i<4;i++)
		orbits[i]=0;
	graph=blib_graph_allocate(4);
	fprintf(stderr,"size is %d\n",graph->size);
	/*A tringle with a spoke of one point*/
	blib_graph_set_edge(graph,0,1,1);
	blib_graph_set_edge(graph,1,2,1);
	blib_graph_set_edge(graph,2,0,1);
	blib_graph_set_edge(graph,2,3,1);
	blib_graph_auto(graph,orbits,NULL,NULL);

	for(i=0;i<graph->size;i++)
		printf("%d ",orbits[i]);
	printf("\n");
	blib_graph_free(graph);
	return 0;
}