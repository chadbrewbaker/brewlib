#ifndef _BLIB_HAM_PATH_STRIPPED_DEF_
#define _BLIB_HAM_PATH_STRIPPED_DEF_
#include "blib_error.h"
#include "blib_graph.h"
#include "blib_partition.h"

#define BLIB_HAM_PATH_STRIPPED_PRINT_ALL_PATHS 0

long BLIB_HAM_PATH_STRIPPED_COUNTER=0;

void blib_ham_path_stripped_hello_world(int* arr,int size){
	int i;
	BLIB_HAM_PATH_STRIPPED_COUNTER++;

	if(BLIB_HAM_PATH_STRIPPED_PRINT_ALL_PATHS){
		printf("Howdy(");
		for(i=0;i<size;i++){
			printf("%d->",arr[i]);
		}
		printf(")\n");
	}
	else{
		if(BLIB_HAM_PATH_STRIPPED_COUNTER%10000 ==1){
			printf("Howdy(");
			for(i=0;i<size;i++){
				printf("%d->",arr[i]);
			}
			printf(")[%ld]\n",BLIB_HAM_PATH_STRIPPED_COUNTER);
		}
	}
}


/*Traverse all paths not using the parents again*/
void blib_ham_path_stripped_sub(blib_graph* g, void(*path_func)(int*,int), int depth, int* used, int* forb)
{
	int i,j;
	/*record the path*/
	if(depth==blib_graph_size(g)){
		/*record the path*/
		(*path_func)(used,depth);
		return;
	}
	for(i=0;i<blib_graph_size(g);i++){
		if(!blib_graph_is_edge(g,used[depth-1],i) || forb[i])
			continue;
		used[depth]=i;
		forb[i]=1;
		blib_ham_path_stripped_sub(g,path_func,depth+1,used,forb);
	    forb[i]=0;
	}
}

blib_ham_path_stripped(blib_graph* g, void(*path_func)(int*,int) )
{	
	int* used;
	int* forb;
	int i;	
	used= (int*) BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	forb= (int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	for(i=0;i<blib_graph_size(g);i++)
		forb[i]=0;
	for(i=0;i<blib_graph_size(g);i++){
			used[0]=i;
		    forb[i]=1;
			blib_ham_path_stripped_sub(g,path_func,1,used,forb);
			forb[i]=0;
	}
	printf("We had %d hamiltonian paths\n",BLIB_HAM_PATH_STRIPPED_COUNTER);
	free(used);
	free(forb);
}
#endif /*_BLIB_HAM_PATH_STRIPPED_*/
