#ifndef _BLIB_GRAPH_TRANSFORM_DEF_
#define _BLIB_GRAPH_TRANSFORM_DEF_
#include "blib_error.h"
#include "blib_graph.h"


/*Insert a vertex on each of the edges. Verticies of the original graph will have the same index*/
blib_graph* blib_graph_transform_edge_adorn(blib_graph* g){
	int i,j,k,size,edges,edge_index;
	blib_graph* eg;
	size=blib_graph_size(g);
	edges=blib_graph_edge_count(g);
	eg=blib_graph_allocate(size+edges);
	/*Replace this with an iterator when we get around to it*/
	edge_index=0;
	for(i=0;i<size;i++){
		for(j=i+1;j<size;j++){
			if(blib_graph_is_edge(g,i,j)){
				blib_graph_set_edge(eg,i,size+edge_index,1);
				blib_graph_set_edge(eg,size+edge_index,j,1);
				edge_index++;
			}
		}
	}
	return eg;
}


/* NOT PROPERLY CODED YET!!!*/
/*Every edge gets turned into a vert and are adjacent if they shared a vertex in the orignal graph.*/
blib_graph* blib_graph_transform_line_graph(blib_graph* g){
	int i,j,k,size,edges,edge_index;
	blib_graph* lg;
	blib_graph* eg;
	size=blib_graph_size(g);
	edges=blib_graph_edge_count(g);
	eg=blib_graph_transform_edge_adorn(g);
	lg=blib_graph_allocate(edges);
	/*Replace this with an iterator when we get around to it*/
	edge_index=0;
	for(i=0;i<size;i++){
		for(j=1;j<size;j++){
			if(blib_graph_is_edge(g,i,j)){
				blib_graph_set_edge(eg,i,size+edge_index,1);
				blib_graph_set_edge(eg,j,size+edge_index,1);
				edge_index++;
			}
		}
	}
	return lg;
}

#ifdef BLIB_UNIT_TEST

void blib_graph_transform_unit(void){
	int fcc_graph[]={1, 2,1, 3,1, 5,1, 6,1, 7,1, 8,2, 1,2, 4,2, 6,2, 8,3, 1,3, 4,3, 7,
		3, 8,4, 2,4, 3,4, 8,5, 1,5, 6,5, 7,5, 9,5, 10,5, 11,5, 12,
		6, 1,6, 2,6, 5,6, 8,6, 10,6, 12,7, 1,7, 3,7, 5,7, 8,7, 11,
		7, 12,8, 1,8, 2,8, 3,8, 4,8, 6,8, 7,8, 12,
		9, 5,9, 10,9, 11,9, 13,9, 14,9, 15,9, 16,10, 5,
		10, 6,10, 9,10, 12,10, 14,10, 16,11, 5,11, 7,11, 9,
		11, 12,11, 15,11, 16,12, 5,12, 6,12, 7,12, 8,12, 10,
		12, 11,12, 16,13, 9,13, 14,13, 15,13, 17,13, 18,13, 19,
		13, 20,14, 9,14,10,14, 13,14, 16,14, 18,14, 20,15, 9,
		15, 11,15, 13,15, 16,15, 19,15, 20,16, 9,16, 10,16, 11,16, 12,
		16, 14,16, 15,16, 20,17, 13,17, 18,17, 19,18, 13,18, 14,
		18, 17,18, 20,19, 13,19, 15,19, 17,19, 20,20, 13,20, 14,
		20, 15,20, 16,20, 18,20, 19,-1};
	int i,j,k,index,size;
	blib_graph* g;
	blib_graph* eg;
	g=blib_graph_allocate(20);
	index=0;
	while(1){
		i=fcc_graph[index];
		if(i<0) break;
		j=fcc_graph[index+1];
		blib_graph_set_edge(g,i-1,j-1,1);
		index+=2;
	}
	
	eg=blib_graph_transform_edge_adorn(g);
	if(eg==NULL){
		BLIB_ERROR("Failed to allocate");
	}
	if(blib_graph_size(eg)!= blib_graph_size(g)+blib_graph_edge_count(g)  ){
		BLIB_ERROR("The size is wrong!");
	}
	for(i=0;i<blib_graph_size(g);i++){
		for(j=0; j<blib_graph_size(g);j++){
			if(blib_graph_edge(eg,i,j)||blib_graph_edge(eg,i,j)){
				BLIB_ERROR("Silly rabbit the adorned graph is bipartite (%d,%d)",i,j)
			}
		}
	}
	index=0;
	size=blib_graph_size(g);
	for(i=0;i<size;i++){
		for(j=i+1; j<size;j++){
			if(blib_graph_is_edge(g,i,j)){
				if(!blib_graph_is_edge(eg,i,size+index) || !blib_graph_is_edge(eg,size+index,j)){
					BLIB_ERROR("Failed to make edges(%d,[%d],%d)",i,size+index,j)
				}
				index++;
			}
		}
	}
	for(i=size;i<blib_graph_size(eg);i++){
		for(j=i+1;j<blib_graph_size(eg);j++){
			if(blib_graph_is_edge(eg,i,j)){
				BLIB_ERROR("Error edges ([%d],[%d]) don't touch!",i,j);
			}
		}
	}
	
	
	free(g);
	free(eg);
}

#endif





#endif /*_BLIB_GRAPH_TRANSFORM_DEF_*/