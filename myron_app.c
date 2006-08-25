#include <stdlib.h>
#include <stdio.h>
#include "blib_graph.h"
#include "blib_graph_auto.h"
#include "blib_ham_path_stripped.h"


int main(){
	
	blib_graph* graph;
	int i;
	int* orbits;
	int verts,x,y;
	FILE* g_file;
	
	g_file=fopen("/tmp/fccgraph.txt","rb");
	/*g_file=fopen("/tmp/small.txt","rb");*/
	if(g_file==NULL)
	{BLIB_ERROR("FILE NOT FOUND");}
	fscanf(g_file,"%d",&verts);
	graph=blib_graph_allocate(verts);
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
	fprintf(stderr,"size is %d\n",graph->size);
	blib_graph_auto(graph,orbits,NULL,NULL,NULL);
	printf("Orbits with index starting at 1\n");
	for(i=0;i<graph->size;i++)
		printf("%d ",orbits[i]+1);
	printf("\n");
	fprintf(stderr,"---Hamiltonian Paths----\n\n");
	blib_ham_path_stripped(graph,&blib_ham_path_stripped_hello_world);
	blib_graph_free(graph);
	free(orbits);
	return 0;
}
