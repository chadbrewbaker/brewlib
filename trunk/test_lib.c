#include <stdlib.h>
#include <stdio.h>
#include "blib_graph.h"
#include "blib_graph_auto.h"
#include "blib_ham_path.h"


int main(){
	
	blib_graph* graph;
	int i;
	int* orbits;
	int verts,x,y;
	FILE* g_file;
	
	/*g_file=fopen("/tmp/fccgraph.txt","rb");*/
	g_file=fopen("/tmp/small.txt","rb");
	if(g_file==NULL)
	{BLIB_ERROR(" ");}
	fscanf(g_file,"%d",&verts);
	
	graph=blib_graph_allocate(verts);
	/* for zero indexed
		while(1){
			fscanf(g_file,"%d",&x);
			if(x<0)
				break;
			fscanf(g_file,"%d",&y);*/
	/*add edge*/
	/*	blib_graph_set_edge(graph,x,y,1);
		}*/
	
	
	/* for 1 indexed*/
	while(1){
		fscanf(g_file,"%d",&x);
		if(x<0)
			break;
		fscanf(g_file,"%d",&y);
		/*add edge*/
		blib_graph_set_edge(graph,x-1,y-1,1);
	}
	


	orbits = (int*)malloc(sizeof(int)*verts);
	
/*	for(i=0;i<verts;i++)
		orbits[i]=0;*/
	fprintf(stderr,"size is %d\n",graph->size);
	
	/*A tringle with a spoke of one point*/
	/*graph=blib_graph_allocate(4);
	blib_graph_set_edge(graph,0,1,1);
	blib_graph_set_edge(graph,1,2,1);
	blib_graph_set_edge(graph,2,0,1);
	blib_graph_set_edge(graph,2,3,1);*/
	blib_graph_auto(graph,orbits,NULL,NULL,NULL);
	fprintf(stderr,"---COOL IT WORKS----\n\n");
	for(i=0;i<graph->size;i++)
		printf("%d ",orbits[i]);
	printf("\n");
	fprintf(stderr,"---COOL IT WORKS----\n\n");
	blib_ham_path(graph,&blib_ham_path_hello_world);
	
	blib_graph_free(graph);
	free(orbits);

	return 0;
}