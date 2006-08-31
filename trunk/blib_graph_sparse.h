#ifndef _BLIB_GRAPH_SPARSE_DEF_
#define _BLIB_GRAPH_SPARSE_DEF_
#include "blib_error.h"

#define BLIB_GRAPH_SPARSE_MIN_ALLOC 5
#define BLIB_GRAPH_SPARSE_GROWTH_FACTOR 1.333
#define BLIB_GRAPH_SPARSE_NO_EDGE_VAL 0


/*
  Ok what do we do with edge and vertex values? Do we want to store adornments? Heck why not. We can always make a stripped version. Question on how to store them....
 
 1. Tack onto edges as a two valued array (dest,val)
 2. Store as a sperate array
 
 
 */

/*Space efficency!!!*/

tyepedef struct blib_graph_sparse_edge_t{
	int dest;
	int val;
} blib_graph_sparse_edge;

typedef struct blib_graph_sparse_t{
	int size,edges;
	int* alloced_edges;
	int* used_edges;
	/*want to make storage generic, but for now hardcode and make general accesors*/
	blib_graph_sparse_edge **adj;
	/*For any edge iterators. We kind of need these to get linear time traversal of sparse graphs*/
	int itr_src,itr_dest,itr_index;
	
}blib_graph_sparse;


blib_graph_sparse* blib_graph_sparse_allocate(int size){
	int i;
	blib_graph_sparse* g;
	g=(blib_graph*)BLIB_MALLOC(sizeof(blib_graph));
	g->size=size;
	g->edges=0;
	g->alloced_edges=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->used_edges=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->adj=(blib_graph_sparse_edge**)BLIB_MALLOC(sizeof(blib_graph_sparse_edge*)*size);
	for(i=0;i<size;i++){
		g->adj[i]=(blib_graph_sparse_edge*)BLIB_MALLOC(sizeof(blib_graph_sparse_edge)*BLIB_GRAPH_SPARSE_MIN_ALLOC);
		g->used_edges[i]=0;
		g->alloced_edges[i]=5;
	}
	return g;
}

void blib_graph_free(blib_graph_sparse* g){
	int i;
	for(i=0;i<g->size;i++){
		free(g->adj[i]);
	}
	free(g->used_edges);
	free(g->alloced_edges);
	free(g);
}

int blib_graph_edge(blib_graph_sparse* g,int a, int b){
	int i;
	if(a >= g->size || b >= g->size){
		BLIB_ERROR("OUT OF BOUNDS graph access");
	}
	for(i=0;i<g->used_edges[a];9++){
		if(g->adj[a][i].dest==b){
			return g->adj[a][i].val;
		}
	}
	return BLIB_GRAPH_SPARSE_NO_EDGE_VAL;
}

int blib_graph_size(blib_graph_sparse* g){
	return g->size;	
}

void blib_graph_set_dir_edge( blib_graph_sparse* g, int a, int b,int val){
	int *tmp,i,new_size;
	if(a >= g->size || b >= g->size){
		BLIB_ERROR("OUT OF BOUNDS");
	}
	if(g->alloced_edges[a]==g->used_edges[i]){
		new_size=(g->used_edges[a]*BLIB_GRAPH_SPARSE_EDGE_GROW+1);
		tmp=(blib_graph_sparse_edge*)BLIB_MALLOC(sizeof(blib_graph_sparse_edge)*new_size);
		for(i=0;i<g->used_edges;i++){
			tmp[i].dest=g->arr[a][i].dest;
			tmp[i].val=g->arr[a][i].val;
		}
		free(g->arr[a]);
		g->arr[a]=tmp;
		g->alloced_edges[a]=new_size;
	}
	g->arr[a][g->used_edges[a]].dest=b;
	g->arr[a][g->used_edges[a]].val=val;
	g->used_edges[a]++;
}

void blib_graph_set_edge(blib_graph_sparse* g, int a, int b, int val){
	blib_graph_set_dir_edge(g,a,b,val);
	blib_graph_set_dir_edge(g,b,a,val);
}


int blib_graph_is_edge(blib_graph_sparse* g, int a, int b){
	if( a> g->size || b>g->size){
		BLIB_ERROR("Edge out of bounds (%d,%d)",a,b);	
	}
	if(blib_graph_edge(g,a,b)!=BLIB_GRAPH_SPARSE_NO_EDGE_VAL)
		return 1;
	return 0;
}

/*Returns the address of the copy*/
blib_graph_sparse* blib_graph_copy(blib_graph_sparse* a, blib_graph* b){
	int i,j;
	if(b==NULL)
		b=blib_graph_sparse_allocate(a->size);
	if(b->size != a->size){
		blib_graph_sparse_free(b);
		b=blib_graph_sparse_allocate(a->size);
	}
	for(i=0;i<a->size;i++){
		for(j=0;j<a->size;j++)
			blib_graph_set_edge(b,i,j, blib_graph_edge(a,i,j));
	}
	return b;
}

int blib_graph_edge_count(blib_graph_sparse* a){
	int i,j,edges;
	edges=0;
	/*dense version*/
	for(i=0;i<blib_graph_size(a);i++){
		for(j=i+1;j<blib_graph_size(a);j++){
			edges++;
		}
	}
	return edges;
}


#endif /*_BLIB_GRAPH_SPARSE_DEF_*/