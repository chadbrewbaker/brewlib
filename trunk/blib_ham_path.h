#ifndef _BLIB_HAM_PATHS_DEF_
#define _BLIB_HAM_PATHS_DEF_
#include "blib_error.h"

/*Traverse all paths not using the parents again*/
blib_ham_paths_sub(blib_graph* g, void(*path_func)(blib_graph*,int*), int depth, int* used, int** scratch)
{
	int i;
	if(depth == g->size)
		{
			(*path_func)(g,used)
			return;
		}
	for(i=0;i<g->size;i++)
		scratch[depth][i]=i;
	for(i=0;i<depth;i++)
		scratch[depth][used[i]]=-1;
	/*blib_prune_path_auts(g, used, scratch[depth], depth);*/
	for(i=0;i<g->size;i++){
		if(scratch[depth][i]<0)
			continue;
		if(!g->edgeq(used[depth],i))
			continue;
		used[depth]=i;
		blib_ham_paths_sub(g,path_func,depth+1,used, scratch);
	}
}

blib_ham_paths(blib_graph* g, void(*path_func)(blib_graph*,int*))
{	
	int* used;
	int** scratch;
	int i;
	used= (int*)BLIB_MALLOC(sizeof(int)*g->size);
	scratch= (int**)BLIB_MALLOC(sizeof(int)*g->size);
	for(i=0;i<g->size;i++)
		scratch[i]=(int*)sizeof(int)*g->size);
	blib_ham_paths_sub(g,path_func,0,used,scratch);
	for(i=0;i<g->size;i++)
		free(scratch[i]);
	free(scratch);
	free(used);
}
#endif /*_BLIB_HAM_PATHS_*/