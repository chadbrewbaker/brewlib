#ifndef _BLIB_HAM_PATHS_DEF_
#define _BLIB_HAM_PATHS_DEF_
#include "blib_error.h"
#include "blib_graph.h"
#include "blib_partition.h"

void blib_ham_path_hello_world(int* arr,int size,int* orbits){
	printf("Howdy(%d,%d->%d)\n",size,arr[0],arr[size-1]);
}

/*Traverse all paths not using the parents again*/
void blib_ham_path_sub(blib_graph* g, void(*path_func)(int*,int,int*), int depth, int* used, int** scratch,int* orbits,blib_partition* pre_part,blib_graph_auto_storage* stuff)
{
	int i,j;
	/*record the path*/
	if(depth==blib_graph_size(g)){
		/*record the path*/
		(*path_func)(used,depth,orbits);
		/*kill off isomorphic ancestors*/
		for(i=1;i<depth-1;i++){
			/*Color the used verts, then kill automorphs*/
			/*blib_partition_reset(pre_part);*/
			/*blib_partition_print(pre_part,stderr);*/
			blib_partition_bicolor(pre_part,used,i);
			/*blib_partition_print(pre_part,stderr);
			fprintf(stderr,"(");
			for(j=0;j<=i;j++){
				fprintf(stderr,"%d, ", used[j]);	
			}
			fprintf(stderr,")\n");*/
			blib_graph_auto_persistent(g,orbits,NULL,NULL,pre_part,stuff);
			for(j=0;j<blib_graph_size(g);j++){
				if(orbits[i]<i){
				scratch[i][j]=-1;	
				}
			}
		}
		
		return;
	}
		
	/*should make this into a sparse list*/
	for(i=0;i<g->size;i++)
		scratch[depth][i]=1;
	for(i=0;i<depth;i++)
		scratch[depth][used[i]]=-1;
	/*blib_prune_path_auts(g, used, scratch[depth], depth);*/
	
	for(i=0;i<blib_graph_size(g);i++){
		/*Already used so ignore it*/
		if(scratch[depth][i]<0)
			continue;
		if(!blib_graph_is_edge(g,used[depth-1],i))
			continue;
		used[depth]=i;
		blib_ham_path_sub(g,path_func,depth+1,used, scratch,orbits,pre_part,stuff);
	}
}

blib_ham_path(blib_graph* g, void(*path_func)(int*,int,int*) )
{	
	int* used;
	int* orbits;
	int** scratch;
	int i;	
	blib_graph_auto_storage* stuff;
	blib_partition* pre_part;
	pre_part=blib_partition_allocate(blib_graph_size(g));
	scratch= (int**) BLIB_MALLOC(sizeof(int*)*blib_graph_size(g));
	used= (int*) BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	orbits=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	
	for(i=0;i<blib_graph_size(g);i++)
		scratch[i]=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	
	
	/*prune out iso verts*/
	stuff=blib_graph_auto_init(g,NULL,orbits,NULL);
	blib_graph_auto_persistent(g,orbits,NULL,NULL,NULL,stuff);
	for(i=0;i<blib_graph_size(g);i++){
		if(orbits[i]==i){
			used[0]=i;
			blib_ham_path_sub(g,path_func,1,used,scratch,orbits,pre_part,stuff);
		}
	}
	blib_graph_auto_finalize(stuff,g,NULL,orbits,NULL);
	for(i=0;i<blib_graph_size(g);i++)
		free(scratch[i]);
	free(scratch);
	free(used);
	free(orbits);
}
#endif /*_BLIB_HAM_PATHS_*/