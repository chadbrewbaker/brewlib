/*
	Graph iso/auto code. Rerwrite of my ruby version in C.
*/

#include "blib_partition.h"
#include "blib_cell_stack.h"
#include "blib_schreier.h"
#include "blib_error.h"
/*
	Using the compressed partiton representation
	Each partition is denoted by a increasing sequence of numbers
*/
#ifndef _BLIB_GRAPH_AUTO_DEF_
#define _BLIB_GRAPH_AUTO_DEF_

typedef struct blib_graph_auto_storage_t{
	blib_partition* best_part;
	blib_partition** part_stack;
	blib_partition* scratch_part;
	blib_partition* old_part;
	int** set_stack;
	int* split_cell;
	int* dirty_cell_arr;
	int dirty_cells;
	int* scratch_arr;
	int* adj_arr;
	int** scratch_stack;
	int** child_cells;
	blib_cell_stack* split_stack;
	blib_graph* graph;
	blib_schreier* schreier;
	int* orbits;
}blib_graph_auto_storage;


blib_graph_auto_storage* blib_graph_auto_init( blib_graph* g, blib_schreier* schreier,int* orbits){
	int i,size;
	blib_graph_auto_storage* stuff;
	size=blib_graph_size(g);
	stuff=BLIB_MALLOC(sizeof(blib_graph_auto_storage));
	stuff->best_part=NULL; /*Keep it null and allocate it when we get one*/
	stuff->old_part=BLIB_MALLOC(sizeof(blib_partition));
	stuff->dirty_cell_arr=BLIB_MALLOC(sizeof(int)*size);
	stuff->scratch_part=blib_partition_allocate(size);
	stuff->part_stack=BLIB_MALLOC(sizeof(blib_partition*)*size);
	for(i=0;i<size;i++)
		stuff->part_stack[i]= blib_partition_allocate(size);
	stuff->scratch_arr=BLIB_MALLOC(sizeof(int)*size);
	stuff->split_cell=BLIB_MALLOC(sizeof(int)*size);
	stuff->set_stack=BLIB_MALLOC(sizeof(int*)*size);
	for(i=0;i<size;i++)
		stuff->set_stack[i]=BLIB_MALLOC(sizeof(int)*size);
	stuff->scratch_stack=BLIB_MALLOC(sizeof(int*)*size);
	for(i=0;i<size;i++)
		stuff->scratch_stack[i]=BLIB_MALLOC(sizeof(int)*size);	
	stuff->child_cells=BLIB_MALLOC(sizeof(int*)*size);
	for(i=0;i<size;i++)
		stuff->child_cells[i]=(int*)BLIB_MALLOC(sizeof(int)*size);
	stuff->split_stack=blib_cell_stack_allocate(size*10);
	stuff->adj_arr =(int*)BLIB_MALLOC(sizeof(int)*size);
	stuff->graph=g;
	stuff->schreier=schreier;
	stuff->orbits=orbits;
 	/*Start out with every in a different orbit then discover automorphisms*/
	if(orbits==NULL)
		stuff->orbits=BLIB_MALLOC(sizeof(int)*size);
	for(i=0;i<size;i++)
		stuff->orbits[i]=i;
	BLIB_ERROR("debug");
	return stuff;
}


void blib_graph_auto_finalize(blib_graph_auto_storage* stuff,blib_graph* graph,blib_schreier* schreier,int* orbits,int* certificate)
{
	int i,size;
	size=blib_graph_size(graph);
	
	if(certificate != NULL){
		for(i=0;i<size;i++)
			certificate[i]=blib_partition_nth_item(stuff->best_part,i);
	}
	blib_partition_free(stuff->best_part);
	blib_partition_free(stuff->old_part);
	for(i=0;i<size;i++)
		blib_partition_free(stuff->part_stack[i]);
	free(stuff->part_stack);
	free(stuff->scratch_arr);
	free(stuff->split_cell);
	free(stuff->dirty_cell_arr);
	for(i=0;i<size;i++)
		free(stuff->set_stack[i]);
	free(stuff->set_stack);
	for(i=0;i<size;i++)
		free(stuff->scratch_stack[i]);
	free(stuff->scratch_stack);
	
	for(i=0;i<size;i++)
		free(stuff->child_cells[i]);
	free(stuff->child_cells);
	free(stuff->adj_arr);
	
	if(orbits==NULL)
		free(stuff->orbits);
	blib_cell_stack_free(stuff->split_stack);
	
	/* Do we need to do any final orbit calculations?*/
	free(stuff);
}




void blib_graph_auto_split_cells(blib_graph_auto_storage* stuff,int depth, int split_cell_size)
{
	int i,j,k,size,used,cells;
	/*split each cell based on its counts*/
	used=0;
	size=blib_partition_size(stuff->part_stack[depth]);
	for(i=0;i<size;i++){
			stuff->adj_arr[i]=0;
			for(j=0;j<split_cell_size;j++){
				/* between stuff->split_cell[k] and nth item of part_stack[depth]*/
				if(blib_graph_is_edge(stuff->graph,
									  blib_partition_nth_item(stuff->part_stack[depth],i),
									  stuff->split_cell[j])){
					stuff->adj_arr[i]++;
				}
			}	
	}
	blib_partition_split_by_key(stuff->part_stack[depth],stuff->adj_arr,stuff->dirty_cell_arr,&stuff->dirty_cells);
} 

void blib_graph_auto_part_refine(blib_graph_auto_storage* stuff, int depth)
{
	int i,split_cell_size;
	/*Push cells onto stack*/
	blib_error_tabs(depth);fprintf(stderr,"auto_part_refine(%d)BEFORE",depth);
	blib_error_tabs(depth);blib_partition_print(stuff->part_stack[depth],stderr);
	for(i=blib_partition_cell_count(stuff->part_stack[depth]);i > 0;i--){
		blib_partition_get_cell(stuff->part_stack[depth], i-1, stuff->scratch_arr, &split_cell_size);
		stuff->split_stack=blib_cell_stack_push(stuff->split_stack,stuff->scratch_arr, split_cell_size);
	}
	/*Pop top element, split cells on it, till stack is empty  */
	while(blib_cell_stack_depth(stuff->split_stack)>0){
		blib_cell_stack_pop(stuff->split_stack, stuff->split_cell, &split_cell_size);
		/*Put the cell to split on in stuff->split_cell*/
		blib_graph_auto_split_cells(stuff,depth,split_cell_size);
		for(i=stuff->dirty_cells-1;i>=0;i--){
			blib_partition_get_cell_at(stuff->part_stack[depth],stuff->dirty_cell_arr[i],
									   stuff->scratch_arr,&split_cell_size);
			stuff->split_stack=blib_cell_stack_push(stuff->split_stack, stuff->scratch_arr,split_cell_size);
		}
	}
	blib_error_tabs(depth);fprintf(stderr,"auto_part_refine(%d)AFTER",depth);
	blib_error_tabs(depth);blib_partition_print(stuff->part_stack[depth],stderr);

}



/* Returns  1 better found, 0 automorphism, -1 worse  */
int blib_graph_auto_part_test(blib_graph_auto_storage* stuff, int depth)
{
	int test_fixed,i,j,a,b,src,dest;
	if(stuff->best_part == NULL)
		return 1;
	test_fixed=blib_partition_front_unit_count(stuff->part_stack[depth]);	
	/*Go down the columns of both testing to see who is smallest*/
	for(j=0;j<test_fixed;j++){
		for(i=0;i<=j;i++){
			src=blib_partition_nth_item(stuff->best_part,i);
			dest=blib_partition_nth_item(stuff->best_part,j);
			a=blib_graph_is_edge(stuff->graph,src,dest);
			src=blib_partition_nth_item(stuff->part_stack[depth],i);
			dest=blib_partition_nth_item(stuff->part_stack[depth],j);
			b=blib_graph_is_edge(stuff->graph,src,dest);
			if(a && !b)
				return 1;
			if(!a && b)
				return -1;
		}
	}
	return 0;
}

/* If this returns 1 it means that we can quit if we don't care about getting the certificate, just the group/orbit info*/
int blib_graph_auto_record(blib_graph_auto_storage* stuff, int depth, int result)
{
	int i,j,a,b,min,size;
	blib_partition_print(stuff->part_stack[depth],stderr);
	blib_error_tabs(depth);BLIB_ERROR(" <-recording%d,%d",depth,result);
	/*Found an automorph so record it*/
	if(result==0){
		if(stuff->schreier!=NULL){
			blib_schreier_print(stuff->schreier,stderr);
			blib_partition_get_perm(stuff->part_stack[depth],stuff->scratch_arr);
			blib_schreier_add_perm(stuff->schreier,stuff->scratch_arr);
			if(blib_schreier_is_full(stuff->schreier))
				return 1;
		}
		size=blib_graph_size(stuff->graph);
		fprintf(stderr,"orbits before<");
		for(i=0;i<size;i++)
			fprintf(stderr,"%d ",stuff->orbits[i]);
		fprintf(stderr,">\n");
			
		for(i=0;i<size;i++){
			a=stuff->orbits[stuff->scratch_arr[i]];
			b=stuff->orbits[i];
			/*We found a new symmetry so combine these classes*/
			if(a!=b){
				/*Give them the smallest of the two keys*/
				if(a<b)
					min=a;
				else
					min=b;
				for(j=0;j<size;j++){
					if(stuff->orbits[j]== a || stuff->orbits[j]==b)
						stuff->orbits[j]=min;
				}
			}
		}
		if(stuff->schreier!=NULL)
			blib_schreier_print(stuff->schreier,stderr);
		fprintf(stderr,"orbits after<");
		for(i=0;i<size;i++)
			fprintf(stderr,"%d ",stuff->orbits[i]);
		fprintf(stderr,">\n");
	
	}
	/*Record it as the new best permutation*/
	else
		stuff->best_part=blib_partition_copy(stuff->part_stack[depth],stuff->best_part);
	return 0;
}


/* Returns  1 if better found, 0 automorphism, -1 worse  */
int blib_graph_auto_sub(blib_graph_auto_storage* stuff, int depth)
{
	int i,cells,result,split_size,split_cell,min_cells,best_flag;
	int d_size,d_j;
	/*Assume it is already part_refined  at [depth], so we can write new copies to [depth+1] */
	blib_error_tabs(depth);fprintf(stderr,"auto_sub(%d)",depth);
	blib_error_tabs(depth);blib_partition_print(stuff->part_stack[depth],stderr);
	if(stuff->best_part == NULL)
		best_flag=1;
	else
		best_flag=0;
	cells=blib_partition_cell_count(stuff->part_stack[depth]);
	result=blib_graph_auto_part_test(stuff,depth);
	blib_error_tabs(depth);BLIB_ERROR("result was %d",result);
	/*Worse off so backtrack*/
	if(result < 0){
		blib_error_tabs(depth);BLIB_ERROR("Returning %d",result);
		return result;
	}
	/*If we have unit partitions record it*/
	blib_error_tabs(depth);BLIB_ERROR("%d ==? %d",cells, blib_graph_size(stuff->graph));
	if(cells == blib_graph_size(stuff->graph)){
		blib_graph_auto_record(stuff,depth,result);
		blib_error_tabs(depth);BLIB_ERROR("recording and returning %d",result);
		return result;	
	}
	for(i=0;i<cells;i++){
		split_size=blib_partition_cell_size(stuff->part_stack[depth],i);
		if( split_size > 1){
			split_cell=i;
			break;
		}
	}
	blib_error_tabs(depth);BLIB_ERROR("split_cell_size %d",split_size);
	min_cells=blib_partition_size(stuff->part_stack[depth]);
	for(i=0;i<split_size;i++){
		/*Get the size of the partition after fixing each element.*/
		stuff->part_stack[depth+1]=blib_partition_copy(stuff->part_stack[depth],stuff->part_stack[depth+1]);
		blib_partition_fix_element(stuff->part_stack[depth+1], split_cell, i); 
		d_size=blib_partition_size(stuff->part_stack[depth+1]);
		blib_partition_print(stuff->part_stack[depth+1],stderr);
		blib_graph_auto_part_refine(stuff,depth+1);
		stuff->child_cells[depth][i]=blib_partition_cell_count(stuff->part_stack[depth+1]);
		if(stuff->child_cells[depth][i]<min_cells)
			min_cells=stuff->child_cells[depth][i];
	}
	blib_error_tabs(depth);BLIB_ERROR("split_cell_size %d",split_size);
	for(i=0;i<split_size;i++){
		if(stuff->child_cells[depth][i]>min_cells)
			continue;
		stuff->part_stack[depth+1]=blib_partition_copy(stuff->part_stack[depth],stuff->part_stack[depth+1]);
		blib_partition_fix_element(stuff->part_stack[depth+1], split_cell, i); 
		blib_graph_auto_part_refine(stuff,depth+1);
		result=blib_graph_auto_sub(stuff,depth+1);
		if(result>0)
			best_flag=1;
		if(result == 0){ /*We got an automorph, so backtrack up to the parent of the best found partition*/
			if(best_flag==0){
				blib_error_tabs(depth);BLIB_ERROR("returning 0");
				return 0;
			}
		}
	}
if(best_flag){
	blib_error_tabs(depth);BLIB_ERROR("blib_graph_auto_sub(%d) returning 1",depth);
	return 1;
}
blib_error_tabs(depth);BLIB_ERROR("blib_graph_auto_sub(%d) returning -1",depth);
	return -1;
}

/*
 
  -Want to partition verticies into "color classes"?Then pass a pre-partition, and possibly pass a forbiden schrier representation
  -Want to say "vertex x and vertex y are pre-computed to be automorphic" then pass a schrier rep marking it as so
 
 
 
 
 
	If you don't want orbits, certificate, or shreier pass a NULL.
*/
void blib_graph_auto(blib_graph* graph, int* orbits, int* certificate, blib_schreier* schreier){
	blib_graph_auto_storage* stuff;
	stuff=blib_graph_auto_init(graph, schreier,orbits);
	/*Should probably modify so orbits can take in pre-defined color classes*/
	BLIB_ERROR(" cells = %d?",blib_partition_cell_count(stuff->part_stack[0]));

	blib_graph_auto_part_refine(stuff,0);
	BLIB_ERROR(" cells = %d?",blib_partition_cell_count(stuff->part_stack[0]));
	BLIB_ERROR("About to sub");
	blib_graph_auto_sub(stuff,0);
	BLIB_ERROR("Finalizing and %d",1);
	
	BLIB_ERROR("Finalizing");
	blib_graph_auto_finalize(stuff,graph, schreier,orbits,certificate);
}
#endif /*_BLIB_GRAPH_AUTO_DEF*/