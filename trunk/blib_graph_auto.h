/*
	Graph iso/auto code. Rerwrite of my ruby version in C.
*/

#include "blib_partition.h"
#include "blib_graph.h"
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
	int best_part_defined;
	blib_partition* best_part;
	blib_partition** part_stack;
	blib_partition* scratch_part;
	/*blib_partition* old_part;*/
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


blib_graph_auto_storage* blib_graph_auto_init( blib_graph* g, blib_schreier* schreier,int* orbits, blib_partition* pre_part){
	int i,size;
	blib_graph_auto_storage* stuff;
	size=blib_graph_size(g);
	stuff=BLIB_MALLOC(sizeof(blib_graph_auto_storage));
	stuff->best_part_defined=0;
	stuff->best_part=blib_partition_allocate(size);
	/* stuff->old_part=blib_partition_allocate(size); */
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
	stuff->split_stack=blib_cell_stack_allocate(size*10,size);
	stuff->adj_arr =(int*)BLIB_MALLOC(sizeof(int)*size);
	stuff->graph=g;
	stuff->schreier=schreier;
	stuff->orbits=orbits;
 	/*Start out with every in a different orbit then discover automorphisms*/
	if(orbits==NULL)
		stuff->orbits=BLIB_MALLOC(sizeof(int)*size);
	for(i=0;i<size;i++)
		stuff->orbits[i]=i;
	if(pre_part!=NULL){
		stuff->part_stack[0]=blib_partition_copy(pre_part,stuff->part_stack[0]);
	}
	return stuff;
}





void blib_graph_auto_reset( blib_graph* g, blib_schreier* schreier,int* orbits, blib_partition* pre_part,blib_graph_auto_storage* stuff){
	int i,size;
	
	size=blib_graph_size(g);
	   
	if((stuff==NULL)|| (blib_partition_size(stuff->scratch_part)!=size)){
		BLIB_ERROR("auto storage you passed me was bogus ");
	}

	stuff->best_part_defined=0;
	blib_partition_reset(stuff->scratch_part);	
	for(i=0;i<size;i++)
		blib_partition_reset(stuff->part_stack[i]);
	blib_cell_stack_reset(stuff->split_stack);
	stuff->graph=g;
	stuff->schreier=schreier;
	stuff->orbits=orbits;
 	/*Start out with every in a different orbit then discover automorphisms*/
	if(orbits==NULL)
		stuff->orbits=BLIB_MALLOC(sizeof(int)*size);
	for(i=0;i<size;i++)
		stuff->orbits[i]=i;
	if(pre_part!=NULL){
		stuff->part_stack[0]=blib_partition_copy(pre_part,stuff->part_stack[0]);
	}
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
	/*blib_partition_free(stuff->old_part);*/
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


void blib_graph_auto_finalize_persistent(blib_graph_auto_storage* stuff,blib_graph* graph,blib_schreier* schreier,int* orbits,int* certificate)
{
	int i,size;
	size=blib_graph_size(graph);
	if(certificate != NULL){
		for(i=0;i<size;i++)
			certificate[i]=blib_partition_nth_item(stuff->best_part,i);
	}
}






void blib_graph_auto_split_cells(blib_graph_auto_storage* stuff,int depth, int split_cell_size)
{
	int i,j,size,used;
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
	if(stuff->part_stack[depth]->cell_count > stuff->part_stack[depth]->size){
		
		BLIB_ERROR(" ");	
	}
	
	
} 

void blib_graph_auto_part_refine(blib_graph_auto_storage* stuff, int depth)
{
	int i,j,split_cell_size,db0,db1,db3,db4;
	/*Push cells onto stack*/
	
	db1=stuff->split_stack->cells_allocated;
	/*blib_error_tabs(depth);fprintf(stderr,"auto_part_refine(%d)BEFORE",depth);*/
	/*blib_error_tabs(depth);blib_partition_print(stuff->part_stack[depth],stderr);*/
	for(i=blib_partition_cell_count(stuff->part_stack[depth]);i > 0;i--){
		blib_partition_get_cell(stuff->part_stack[depth], i-1, stuff->scratch_arr, &split_cell_size);
		stuff->split_stack=blib_cell_stack_push(stuff->split_stack,stuff->scratch_arr, split_cell_size);
	}
	db3=stuff->part_stack[depth]->cell_count;
	/*Pop top element, split cells on it, till stack is empty  */
	db0=stuff->split_stack->cells_allocated;
	db4=0;
	while(blib_cell_stack_depth(stuff->split_stack)>0){
		
		blib_cell_stack_pop(stuff->split_stack, stuff->split_cell, &split_cell_size);
		/*Put the cell to split on in stuff->split_cell*/
		blib_graph_auto_split_cells(stuff,depth,split_cell_size);
		
		
		
		if(stuff->part_stack[depth]->cell_count >  stuff->part_stack[depth]->size ){
			BLIB_ERROR(" ");
			blib_partition_print(stuff->part_stack[depth],stderr);
			i=1;
		}
		
		
		for(i=stuff->dirty_cells-1;i>=0;i--){
			j=split_cell_size;
			blib_partition_get_cell_at(stuff->part_stack[depth],stuff->dirty_cell_arr[i],
									   stuff->scratch_arr,&split_cell_size);
			stuff->split_stack=blib_cell_stack_push(stuff->split_stack, stuff->scratch_arr,split_cell_size);
		}
		
		
		db4++;
	}
	

	/*
	blib_error_tabs(depth);fprintf(stderr,"auto_part_refine(%d)AFTER",depth);
	 blib_error_tabs(depth);blib_partition_print(stuff->part_stack[depth],stderr);*/
	
}



/* Returns  1 better found, 0 automorphism, -1 worse  */
int blib_graph_auto_part_test(blib_graph_auto_storage* stuff, int depth)
{
	int test_fixed,i,j,a,b,src,dest;
	if(!stuff->best_part_defined)
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
	/*blib_partition_print(stuff->part_stack[depth],stderr);*/
	/*blib_error_tabs(depth);BLIB_ERROR(" <-recording%d,%d",depth,result);*/
	size=blib_graph_size(stuff->graph);
	
	/*Found an automorph so record it*/
	if(result==0){
		
		if(stuff->schreier!=NULL){
			BLIB_ERROR("Orbit found");
			for(i=0;i<size;i++){
				stuff->scratch_arr[i]=blib_partition_nth_item(stuff->part_stack[depth],i);
			}
			blib_schreier_enter2(stuff->schreier,stuff->scratch_arr);
			/* Implement it so that it automaticaly quits when the symetric group is generated, or we have enough 
				forbiden permutations so that we can add no more:
				if(blib_schreier_is_full(stuff->schreier))
				return 1;*/
		}
		
		/*fprintf(stderr,"orbits before<");
		for(i=0;i<size;i++)
		fprintf(stderr,"%d ",stuff->orbits[i]);
		fprintf(stderr,">\n");*/
		
		for(i=0;i<size;i++){
			a=blib_partition_nth_item(stuff->part_stack[depth],i);
			b=blib_partition_nth_item(stuff->best_part,i);
			/*We found a new symmetry so combine these classes*/
			if(a!=b){
				if(stuff->orbits[a]<=stuff->orbits[b])
					min=stuff->orbits[a];
				else
					min=stuff->orbits[b];
				/*Give them the smallest of the two keys*/
				for(j=0;j<size;j++){
					if(stuff->orbits[j]== a || stuff->orbits[j]==b)
						stuff->orbits[j]=min;
				}
			}
		}
		/*
		 if(stuff->schreier!=NULL)
		 blib_schreier_print(stuff->schreier,stderr);
		 */
		/*fprintf(stderr,"orbits after<");
		for(i=0;i<size;i++)
		fprintf(stderr,"%d ",stuff->orbits[i]);
		fprintf(stderr,">\n");*/
		
	}
	/*Record it as the new best permutation*/
	else{
		BLIB_ERROR("Better found");
		stuff->best_part_defined=1;
		stuff->best_part=blib_partition_copy(stuff->part_stack[depth],stuff->best_part);
	}
	fprintf(stderr,"[");
	for(i=0;i<size;i++){
		fprintf(stderr,",%d ",blib_partition_nth_item(stuff->part_stack[depth],i));
	}
	fprintf(stderr,"]\n");
	BLIB_ERROR(" ");
	return 0;
}


/* Returns  1 if better found, 0 automorphism, -1 worse  */
int blib_graph_auto_sub(blib_graph_auto_storage* stuff, int depth)
{
	int i,j,k,cells,result,split_size,split_cell,min_cells,best_flag;
	int split_vert;
	int d_size;
	if(!stuff->best_part_defined)
		best_flag=1;
	else
		best_flag=0;
	
	cells=blib_partition_cell_count(stuff->part_stack[depth]);
#ifdef BLIB_DEBUG
	if(cells>blib_graph_size(stuff->graph)){
		blib_partition_print(stuff->part_stack[depth],stderr);
		fprintf(stderr,"\n# ");
		for(i=0;i<blib_graph_size(stuff->graph);i++){
			if( BLIB_DEBUG_ARR[i] >=0){
				fprintf(stderr," %d ",BLIB_DEBUG_ARR[i]);
			}
			else{
				break;
			}
		}
		fprintf(stderr,"# (%d) \n", BLIB_DEBUG_X);
		BLIB_ERROR(" ");	
	}
#endif
	result=blib_graph_auto_part_test(stuff,depth);
	if(result < 0){
		/*Worse off so backtrack*/
		blib_error_tabs(depth);BLIB_ERROR("Returning %d",result);
		return result;
	}
	/*If we have unit partitions record it*/
	if(cells == blib_graph_size(stuff->graph)){
		blib_graph_auto_record(stuff,depth,result);
		/*blib_error_tabs(depth);BLIB_ERROR("recording and returning %d",result);*/
		return result;	
	}
	for(i=0;i<cells;i++){
		split_size=blib_partition_cell_size(stuff->part_stack[depth],i);
		if( split_size > 1){
			split_cell=i;
			break;
		}
	}
	/*blib_error_tabs(depth);BLIB_ERROR("split_cell_size %d",split_size);*/
	
	/*This section does a greedy check on siblings to go down paths that have the most fixed elements at the next level*/
	min_cells=blib_partition_size(stuff->part_stack[depth]);
	for(i=0;i<split_size;i++){
		/*Get the size of the partition after fixing each element.*/
		stuff->part_stack[depth+1]=blib_partition_copy(stuff->part_stack[depth],stuff->part_stack[depth+1]);
		blib_partition_fix_element(stuff->part_stack[depth+1], split_cell, i); 
		d_size=blib_partition_size(stuff->part_stack[depth+1]);
		/*blib_partition_print(stuff->part_stack[depth+1],stderr);*/
		blib_graph_auto_part_refine(stuff,depth+1);
		stuff->child_cells[depth][i]=blib_partition_cell_count(stuff->part_stack[depth+1]);
		if(stuff->child_cells[depth][i]<min_cells)
			min_cells=stuff->child_cells[depth][i];
	}
	
	/*blib_error_tabs(depth);BLIB_ERROR("split_cell_size %d",split_size);*/
	for(i=0;i<split_size;i++){
		/*Ignore if it didn't meet the greedy test*/
		if(stuff->child_cells[depth][i]>min_cells  || stuff->child_cells[depth][i]<0)
			continue;
		stuff->part_stack[depth+1]=blib_partition_copy(stuff->part_stack[depth],stuff->part_stack[depth+1]);
		blib_partition_fix_element(stuff->part_stack[depth+1], split_cell, i); 
		blib_graph_auto_part_refine(stuff,depth+1);
		
		result=blib_graph_auto_sub(stuff,depth+1);
		if(result>0)
			best_flag=1;
		/*We got an automorph, so backtrack up to the parent of the best found partition*/
		if(result == 0){
			if(!best_flag){
				return 0;
			}
		}
		/*Kill off isomorphic children */
		/*Re-base base on our perm. Is there a quicker way of doing this??? It would also be nice to skip this step when we have no auts*/
		for(j=0;j<blib_graph_size(stuff->graph);j++){
			stuff->scratch_arr[j]=blib_partition_nth_item(stuff->part_stack[depth+1],j);
		}
		if(stuff->schreier==NULL)
			stuff->schreier=blib_schreier_alloc(blib_graph_size(stuff->graph));
		blib_schreier_change_base(stuff->schreier,stuff->scratch_arr);
		/*The index we are splitting on is split_cell*/
		/*The element we are splitting on is located at ...blib_partition_nth_item(stuff->part_stack[depth],split_cell+i)*/
		
		split_vert=blib_partition_nth_item(stuff->part_stack[depth],split_cell+i);
		for(j=0;j<blib_graph_size(stuff->graph);j++){
			if(stuff->schreier->perms[split_cell][j][0]>=0){
				for(k=i+1;k<split_size;k++){
					if(stuff->schreier->perms[split_cell][j][split_vert]== 
					   blib_partition_nth_item(stuff->part_stack[depth],split_cell+k)      ){						
						stuff->child_cells[depth][k]=-1;
					}
				}
			}
		}
	}
	if(best_flag){
		/*blib_error_tabs(depth);BLIB_ERROR("blib_graph_auto_sub(%d) returning 1",depth);*/
		return 1;
	}
	/*blib_error_tabs(depth);BLIB_ERROR("blib_graph_auto_sub(%d) returning -1",depth);*/
	return -1;
}

/*
 
  -Want to partition verticies into "color classes"?Then pass a pre-partition, and possibly pass a forbiden schrier representation
  -Want to say "vertex x and vertex y are pre-computed to be automorphic" then pass a schrier rep marking it as so
 
 
	If you don't want orbits, certificate, or shreier pass a NULL.
*/
void blib_graph_auto(blib_graph* graph, int* orbits, int* certificate, blib_schreier* schreier,blib_partition* pre_part){
	blib_graph_auto_storage* stuff;
	stuff=blib_graph_auto_init(graph, schreier,orbits,pre_part);
	blib_graph_auto_part_refine(stuff,0);
	blib_graph_auto_sub(stuff,0);
	blib_graph_auto_finalize(stuff,graph, schreier,orbits,certificate);
}


void blib_graph_auto_persistent(blib_graph* graph, int* orbits, int* certificate, blib_schreier* schreier,blib_partition* pre_part,blib_graph_auto_storage* stuff)
{
	int d1,d2;
	blib_graph_auto_reset(graph,schreier,orbits,pre_part,stuff);
	d1=stuff->part_stack[0]->cell_count;
	/*BLIB_ERROR("Printing the stack before it gets farked");
	blib_partition_print(stuff->part_stack[0],stderr);*/
	
	blib_graph_auto_part_refine(stuff,0);
	/*BLIB_ERROR("Printing the stack after it gets farked");
	blib_partition_print(stuff->part_stack[0],stderr);*/
	d2=stuff->part_stack[0]->cell_count;
	blib_graph_auto_sub(stuff,0);
	blib_graph_auto_finalize_persistent(stuff,graph,schreier,orbits,certificate);
}

void blib_graph_auto_edge_colored(blib_graph* g, int* orbs){
	/*blib_graph* g2;*/
	/*int i,j,edge_count;*/
	/*	edge_count=blib_graph_edge_num(g);*/
	
	/*g2=blib_graph_allocate(blib_graph_size(g)+edge_count);*/
	/* add edges to g*/
	/* Partition verts off from edges*/
	/*partition vert color classes*/
	/*partition edge color classes*/
	/*Run graph auto*/
	/*return the orbits of the vertex verticies as normal*/
}

void blib_graph_auto_print_saucy(blib_graph* g,blib_partition* part,FILE* stream){
	int i,j,is_first,size;
	size=blib_graph_size(g);
	fprintf(stream,"%d %d %d ",size,blib_graph_edge_count(g),blib_partition_cell_count(part));
	int* temp_perm;
	
	for(i=1;i<blib_partition_cell_count(part);i++){
		fprintf(stream,"%d ",part->cells[i]);	
	}
	temp_perm=(int*)malloc(sizeof(int)*size);
	for(i=0;i<size;i++){
		temp_perm[i]=part->perm[i];
	}
	is_first=1;
	for(i=0;i<blib_graph_size(g);i++){
		for(j=i+1;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				fprintf(stream,"%d %d \n",temp_perm[i],temp_perm[j]);	
				}
			}
	}
	free(temp_perm);
}

void blib_graph_auto_print_gap(blib_graph* g,blib_partition* part,FILE* stream){
	int i,j,is_first;
	fprintf(stream,"AutGroupGraph(UnderlyingGraph(EdgeOrbitsGraph( Group(()), [");
	is_first=1;
	for(i=0;i<blib_graph_size(g);i++){
		for(j=i+1;j<blib_graph_size(g);j++){
			if(blib_graph_is_edge(g,i,j)){
				if(is_first){
					fprintf(stream,"[%d,%d]",i+1,j+1);
					is_first=0;
				}
				else{
					fprintf(stream,",[%d,%d]",i+1,j+1);	
				}
			}
		}
	}
	fprintf(stream,"], %d)),[",blib_graph_size(g));
	blib_partition_print(part,stream);
	fprintf(stream,"]);\n");
}


#ifdef BLIB_UNIT_TEST


int blib_graph_auto_unit(void){
	int check_orb[]={0, 1, 1, 3, 4, 5, 5, 7, 8, 9, 9, 8, 7, 5, 5, 4, 3, 1, 1, 0};
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
	int i,j,index,fail;
	int orbits[20];
	blib_graph* g;
	blib_graph_auto_storage* stuff;
	fail=0;
	g=blib_graph_allocate(20);
	index=0;
	while(1){
		i=fcc_graph[index];
		if(i<0) break;
		j=fcc_graph[index+1];
		blib_graph_set_edge(g,i-1,j-1,1);
		index+=2;
	}
	blib_graph_auto(g,orbits, NULL, NULL,NULL);
	for(i=0;i<20;i++){
		if(orbits[i]!=check_orb[i]){
			BLIB_ERROR("BAD ORBITS!!");
			fail=1;
		}
	}
	stuff=blib_graph_auto_init(g,NULL,orbits,NULL);
	blib_graph_auto_persistent(g,orbits,NULL,NULL,NULL,stuff);
	for(i=0;i<20;i++){
		if(orbits[i]!=check_orb[i]){
			BLIB_ERROR("BAD ORBITS!!");
			fail=1;
		}
	}	
	blib_graph_auto_persistent(g,orbits,NULL,NULL,NULL,stuff);
	for(i=0;i<20;i++){
		if(orbits[i]!=check_orb[i]){
			BLIB_ERROR("BAD ORBITS!!");
			fail=1;
		}
	}	
	blib_graph_auto_finalize(stuff,g,NULL,orbits,NULL);
	return fail;
}

#endif

#endif /*_BLIB_GRAPH_AUTO_DEF*/