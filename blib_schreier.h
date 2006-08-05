#ifndef _BLIB_SCHREIER_DEF_
#define _BLIB_SCHREIER_DEF_
#include "blib_error.h"
typedef struct blib_schreier_t{
	int size;
	int perm_count;
	int forb_count;
	int *** perms;
}blib_schreier;


blib_schreier*  blib_schreier_alloc( int size){
	int i,j,k;
	blib_schreier* g;
	g=BLIB_MALLOC(sizeof(blib_schreier));
	g->size=size;
	g->perms=(int***)BLIB_MALLOC(sizeof(int**)*size);
	for(i=0;i<size;i++){
		g->perms[i]=(int**)BLIB_MALLOC(sizeof(int*)*size);
		for(j=0;j<size;j++){
			g->perms[i][j]=(int*)BLIB_MALLOC(sizeof(int)*size);
			for(k=0;k<size;k++)
					g->perms[i][j][k]=-1;
			/*Start out with no permutations*/
		}
	}
	g->perm_count=g->forb_count=0;
	return g;
}

void blib_schreier_free(blib_schreier* g){
	int i,j;
	for(i=0;i<g->size;i++){
		for(j=0;j<g->size;j++){
			free(g->perms[i][j]);
		}
		free(g->perms[i]);
	}
	free(g->perms);
	free(g);
}

int blib_schreier_is_full(blib_schreier* g){
	if(g->forb_count+g->perm_count == g->size*g->size)
		return 1;
	return 0;
}

int blib_schreier_size(blib_schreier* g){
	return g->size;
}

int blib_schreier_is_perm(blib_schreier* g, int src, int dest)
{
	if(g->perms[src][dest][0] >=0)
		return 1;
	return 0;
}

void blib_schreier_get_perm(blib_schreier* g, int src, int dest, int* ret_perm)
{
	int i;
	for(i=0;i<g->size;i++)
		ret_perm[i]=g->perms[src][dest][i];
}

void blib_shrier_forbid_perm(blib_schreier* g, int src, int dest)
{
	
	int i;
	if(g->perms[src][dest][0]== -2) 
		return;
	for(i=0;i<g->size;i++)
		g->perms[src][dest][i]=-2;
	g->forb_count++;
}

int blib_schreier_add_perm(blib_schreier* g, int* new_perm)
{
	int i,j,is_new;
	is_new=0;
	for(i=0;i<g->size;i++){
		if(g->perms[i][new_perm[i]][0] <0){
			if(g->perms[i][new_perm[i]][0]< -1)
				g->forb_count--;
			for(j=0;j<g->size;j++)
				g->perms[i][new_perm[i]][j]= new_perm[j];
			is_new=1;
			g->perm_count++;
		}
	}
	return is_new;
}

#endif /*_BLIB_SCHREIER_DEF_*/