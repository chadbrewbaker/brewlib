#ifndef _BLIB_GRAPH_DEF_
#define _BLIB_GRAPH_DEF_
#include "blib_error.h"
typedef struct blib_graph_t{
	int size;
	/*want to make storage generic, but for now hardcode and make general accesors*/
	int **adj;
}blib_graph;


blib_graph* blib_graph_allocate(int size){
	int i;
	blib_graph* g;
	g=(blib_graph*)BLIB_MALLOC(sizeof(blib_graph));
	g->size=size;
	fprintf(stderr,"THe size is %d\n",g->size);
	g->adj=(int**)BLIB_MALLOC(sizeof(int*)*size);
	for(i=0;i<size;i++)
		g->adj[i]=(int*)BLIB_MALLOC(sizeof(int)*size);
	return g;
}

void blib_graph_free(blib_graph* g){
	int i;
	for(i=0;i<g->size;i++){
		free(g->adj[i]);
	}
	free(g);
}

int blib_graph_edge(blib_graph* g,int a, int b){
	if(a >= g->size || b >= g->size){
		BLIB_ERROR("OUT OF BOUNDS");
	}
	return g->adj[a][b];
}

int blib_graph_size(blib_graph* g){
	return g->size;	
}

void blib_graph_set_dir_edge( blib_graph* g, int a, int b, int val){
	if(a >= g->size || b >= g->size){
		BLIB_ERROR("OUT OF BOUNDS");
	}
	g->adj[a][b]=val;
}

void blib_graph_set_edge(blib_graph* g, int a, int b, int val){
	blib_graph_set_dir_edge(g,a,b,val);
	blib_graph_set_dir_edge(g,b,a,val);
}


int blib_graph_is_edge(blib_graph* g, int a, int b){
	if( a> g->size || b>g->size){
		BLIB_ERROR("Edge out of bounds (%d,%d)",a,b);	
	}
	if(blib_graph_edge(g,a,b)!=0)
		return 1;
	return 0;
}

/*Returns the address of the copy*/
blib_graph* blib_graph_copy(blib_graph* a, blib_graph* b){
	int i,j;
	if(b==NULL)
		b=blib_graph_allocate(a->size);
	if(b->size != a->size){
		blib_graph_free(b);
		b=blib_graph_allocate(a->size);
	}
	for(i=0;i<a->size;i++){
		for(j=0;j<a->size;j++)
			blib_graph_set_edge(b,i,j, blib_graph_edge(a,i,j));
	}
	return b;
}
#endif /*_BLIB_GRAPH_DEF_*/