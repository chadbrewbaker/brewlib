#ifndef _BLIB_GRAPH_DEF_
#define _BLIB_GRAPH_DEF_
#include "blib_error.h"

/* Need to put prototypes here. List of base types I can think of:
   -main attributes: Dynamic/Static, Sparse/Dense
        -auxilary attributes: cyclic/interval[lonesum]/disjoint
   -(Dynamic/Dense)Boolean adjacency matrix (preferably bit packed)
   -(Dynamic/Dense)adjacency matrix
   -(Static/very spare)list of edge pairs
   -(Dynamic/Sparse)adjacency list
   -(Hybrid/Sparse)unordered adjacency arrays
   -(Static/Sparse)saad: Compressed row storage. Very space efficent, but not good for updating. struct {val,dest}*, int* row_ptrs
 */


#define BLIB_GRAPH_NO_EDGE_VAL 0

typedef struct blib_graph_t{
	int size;
	/*want to make storage generic, but for now hardcode and make general accesors*/
	int **adj;
	/*For any edge iterators. We kind of need these to get linear time traversal of sparse graphs*/
	int itr_src,itr_dest;
	
}blib_graph;


inline 
int blib_graph_size(blib_graph* g){
#ifdef BLIB_DEBUG
	if(g==NULL){
		BLIB_ERROR("Accessing a null pointer");
	}
#endif
	return g->size;	
}


inline
void blib_graph_set_dir_edge( blib_graph* g, int a, int b, int val){
#ifdef BLIB_DEBUG
	if(a >= blib_graph_size(g) || b >= blib_graph_size(g) || a<0 || b<0){
		BLIB_ERROR("OUT OF BOUNDS");
	}
#endif
	g->adj[a][b]=val;
}


void blib_graph_reset(blib_graph* g){
	int i,j,size;
	size=blib_graph_size(g);
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			blib_graph_set_dir_edge(g,i,j,BLIB_GRAPH_NO_EDGE_VAL);
		}
	}
}


blib_graph* blib_graph_allocate(int size){
	int i;
	blib_graph* g;
	g=(blib_graph*)BLIB_MALLOC(sizeof(blib_graph));
	g->size=size;
	g->adj=(int**)BLIB_MALLOC(sizeof(int*)*size);
	for(i=0;i<size;i++)
		g->adj[i]=(int*)BLIB_MALLOC(sizeof(int)*size);
	blib_graph_reset(g);
	return g;
}


void blib_graph_free(blib_graph* g){
	int i;
	for(i=0;i<blib_graph_size(g);i++){
		free(g->adj[i]);
	}
	free(g);
}

inline
int blib_graph_edge(blib_graph* g,int a, int b){
#ifdef BLIB_DEBUG
	if(a >= blib_graph_size(g) || b >= blib_graph_size(g) || a<0 || b<0 ){
		BLIB_ERROR("OUT OF BOUNDS graph access");
	}
#endif
	return g->adj[a][b];
}


inline
void blib_graph_set_edge(blib_graph* g, int a, int b, int val){
	blib_graph_set_dir_edge(g,a,b,val);
	blib_graph_set_dir_edge(g,b,a,val);
}

inline
int blib_graph_is_edge(blib_graph* g, int a, int b){
#ifdef BLIB_DEBUG
	if( a>= blib_graph_size(g) || b>=blib_graph_size(g)|| a<0 || b<0){
		BLIB_ERROR("Edge out of bounds (%d,%d)",a,b);	
	}
#endif
	if(blib_graph_edge(g,a,b)!=BLIB_GRAPH_NO_EDGE_VAL)
		return 1;
	return 0;
}

/*Returns the address of the copy*/
blib_graph* blib_graph_copy(blib_graph* a, blib_graph* b){
	int i,j;
#ifdef BLIB_DEBUG
	if(a==NULL){
		BLIB_ERROR(" Copying a null pointer");	
	}
#endif
	if(b==NULL)
		b=blib_graph_allocate(a->size);
	if(blib_graph_size(b) != blib_graph_size(a)){
		blib_graph_free(b);
		b=blib_graph_allocate(blib_graph_size(a));
	}
	for(i=0;i<blib_graph_size(a);i++){
		for(j=0;j<blib_graph_size(a);j++)
			blib_graph_set_dir_edge(b,i,j, blib_graph_edge(a,i,j));
	}
	return b;
}


int blib_graph_edge_count(blib_graph* a){
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
/*Prints in #verts a b\n a b\n.... -1 format*/
void blib_graph_print(blib_graph* g,FILE* stream){
	int i,j;
	fprintf(stream,"%d\n",blib_graph_size(g));
	for(i=0;i<blib_graph_size(g);i++){
		for(j=i+1;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				fprintf(stream,"%d %d\n",i,j);
			}
		}
	}
	fprintf(stream,"-1\n");
}


void blib_graph_print_dot(blib_graph* g,FILE* stream,char** names){
	int i,j;
	fprintf(stream,"digraph g {\n");
	if(names!=NULL){
		for(i=0;i<blib_graph_size(g);i++){
			if(names[i]!=NULL)
				fprintf(stream,"v%d [label=\"%s\"];\n",i,names[i]);
		}
	}
	for(i=0;i<blib_graph_size(g);i++){
		for(j=0;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				fprintf(stream,"v%d -> v%d;\n",i,j);
			}
		}
	}
	fprintf(stream,"\n}\n");
}



/*Prints in a zero based %edges %verts a b\n a b\n... format*/
void blib_graph_print_cages(blib_graph* g,FILE* stream){
	int i,j;
	fprintf(stream,"%d %d\n",blib_graph_edge_count(g),blib_graph_size(g));
	for(i=0;i<blib_graph_size(g);i++){
		for(j=i+1;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				fprintf(stream,"%d %d\n",i,j);
			}
		}
	}
}




void blib_graph_print_dreadnaut(blib_graph* g,FILE* stream){
	int i,j;
	fprintf(stream,"n=%d g\n",blib_graph_size(g));
	for(i=0;i<blib_graph_size(g);i++){
		for(j=i+1;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				fprintf(stream,"%d ",j);
			}
		}
		fprintf(stream,";\n");
	}
	/*To get orbits fprintf(stream,"\nx\no\n");*/
}

#ifdef BLIB_UNIT_TEST

int blib_graph_unit(void){
	blib_graph* g;
	int i,j,fail;
	fail=0;
	g=blib_graph_allocate(5);
	if(g==NULL){
		BLIB_ERROR("Graph didn't allocate");	
		fail=1;
	}
	for(i=0;i<5;i++){
		for(j=0;j<5;j++){
			if(blib_graph_is_edge(g,i,j)){
				BLIB_ERROR("Graph not set to empty during init");	
				fail=1;
			}
		}
	}
	blib_graph_set_dir_edge(g,2,1,1);
	if(blib_graph_is_edge(g,1,2) || !blib_graph_is_edge(g,2,1)){
		BLIB_ERROR("Hey it didn't set just the directed edge!");
		fail=1;
	}
	blib_graph_free(g);
	/* Question how do we test to see if something is free?
	if(g!=NULL){
		BLIB_ERROR("Hey it didn't free the graph");	
	}*/
	return fail;
}
#endif



#endif /*_BLIB_GRAPH_DEF_*/