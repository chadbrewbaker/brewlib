/*
 Chad Brewbaker
 9-27-2006
 minCyclicDesign.c

What is the minimum number of permutations that need to be tested in a black-box group to either identify a cyclic subgroup
 or prove that none exists.
*/
#include <stdio.h>
#include <stdlib.h>
#include "blib_graph.h"
#include "blib_error.h"

#define SET_SIZE 6


/*Modification of CAGES page53*/
/*Make sure perm and scratch have are padded with en extra element on the end*/
void perm_lex_successor(int size, int* perm, int* scratch){
	int i,j,t,h;
	/*shift right and re-base to 1*/
	for(i=0;i<size;i++){
		perm[i+1]=perm[i]+1;
	}
	perm[0]=0;
	i=size-1;
	/*Find the first increasing pair*/
	while(perm[i]>perm[i+1])
		i--;
	j=size;
	/*Find the first element from the end of the array that is bigger or same*/
	while(perm[j]<perm[i])
		j--;
	t=perm[j];
	perm[j]=perm[i];
	perm[i]=t;
	for(h=i+1;h<=size;h++){
		scratch[h]=perm[h];
	}
	for(h=i+1;h<=size;h++){
		perm[h]=scratch[size+i+1-h];
	}
	/*shift left and re-base to 0*/
	for(i=0;i<size;i++){
		perm[i]=perm[i+1]-1;
	}
}

void perm_copy(int size,int* source, int* dest){
	int i;
	for(i=0;i<size;i++){
		dest[i]=source[i];
	}	
}


int factorial(int x){
	if(x<=1)
		return 1;
	return x*factorial(x-1);
}

int perm_lex_rank(int size,int* perm,int* scrap){
	int i,j,r;
	/*rebase and shift*/
	for(i=0;i<size;i++){
		perm[i+1]=perm[i]+1;
	}
	r=0;
	perm_copy(size+1,perm,scrap);
	for(j=1; j<size;j++){
		r=r+(scrap[j]-1)*factorial(size-j);
		for(i=j+1;i<=size;i++){
			if(scrap[i]>scrap[j]){
				scrap[i]--;
			}
		}
	}
	/*rebase and shift*/
	for(i=0;i<size;i++){
		perm[i]=perm[i+1]-1;
	}
	return r;
}

/*A la cages modified for zero index subsets*/
int k_subset_lex_successor(int* subset,int* result, int set_size, int subset_size){
	int i,j;
	i=subset_size;
	while(i>=1 && (subset[i-1]+1==set_size-subset_size+i)){
		i--;
	}
	if(i==0)
		return 1;
	for(j=0;j<subset_size;j++)
		result[j]=subset[j];
	for(j=i;j<=subset_size;j++){
		result[j-1]=subset[i-1]+1+j-i;
	}
	for(i=0;i<subset_size;i++)
		subset[i]=result[i];
	return 0;
}

int is_dom_set(blib_graph* g, int* in_dom_set, int set_size,int* scratch){
	int i,j,size,index;
	size=blib_graph_size(g);
	for(i=0;i<size;i++)
		scratch[i]=0;
	for(i=0;i<set_size;i++){
		index=in_dom_set[i];
			scratch[index]=1;
			for(j=0;j<size;j++){
				if(blib_graph_is_edge(g,index,j))
					scratch[j]=1;
			}
	}
	for(i=0;i<size;i++){
		if(scratch[i]==0)
			return 0;
	}
	return 1;
}


void dom_set(blib_graph* g, int* result){
	int i,j,best_size;
	int* subset;
	int* scratch;
	int* best_set;
	subset=(int*)malloc(sizeof(int)*blib_graph_size(g));
	scratch=(int*)malloc(sizeof(int)*blib_graph_size(g));
	best_set=(int*)malloc(sizeof(int)*blib_graph_size(g));
	best_size=0;
	for(i=blib_graph_size(g);i>=1;i--){
		for(j=0;j<i;j++){
			subset[j]=j;
		}
		BLIB_ERROR("Trying size %d subsets",i);
		while(1){
			if(is_dom_set(g,subset,i,scratch)){
				best_size=i;
				printf("The min dom set is size %d\n",best_size);
				for(j=0;j<i;j++)
					best_set[j]=subset[j];
				break;
			}
			if( k_subset_lex_successor(subset,scratch, blib_graph_size(g), i))
				break;
		}
		if(best_size!=i)
			break;
	}
	for(i=0;i<blib_graph_size(g);i++)
		result[i]=0;
	for(i=0;i<best_size;i++)
		result[best_set[i]]=1;
	free(subset);
	free(scratch);
}

void perm_mult(int size, int* a, int* b, int* result){
	int i;
	for(i=0;i<size;i++)
		result[i]=a[b[i]];
}


int equiv_perm(int size, int* a, int* b){
	int i;
	for(i=0;i<size;i++){
		if(a[i]!=b[i])
			return 0;
	}
	return 1;
}

int main(){
	int** perm_table;
	int* in_dom_set;
	int* perm1;
	int* perm2;
	int* perm3;
	blib_graph* g;
	int group_size= factorial(SET_SIZE);
	int i,j,k,rank;
	g =(blib_graph*) blib_graph_allocate(group_size);
	perm_table=(int**)malloc(sizeof(int*)*group_size);
	in_dom_set=(int*)malloc(sizeof(int)*group_size);
	for(i=0;i<group_size;i++){
		perm_table[i]=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	}
	perm1=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	perm2=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	perm3=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	/*Now fill in the perm_table with the permutation that goes there*/
	for(i=0;i<SET_SIZE;i++){
		perm1[i]=i;
	}
	perm_copy(SET_SIZE,perm1,perm_table[0]);
	for(i=0;i<blib_graph_size(g);i++){
		perm_copy(SET_SIZE,perm_table[i],perm1);
		blib_graph_set_edge(g,i,i,1);
		while(1){
			perm_mult(SET_SIZE,perm_table[i],perm1,perm2);
			perm_copy(SET_SIZE,perm2,perm1);
			if(equiv_perm(SET_SIZE,perm_table[i],perm1)){
				break;
			}
			blib_graph_set_dir_edge(g,perm_lex_rank(SET_SIZE,perm1,perm2),i,1);
		}
	}
	/*Now find the min vertex cover.  */
	/*Bork the graph so that everybody goes directly to the identity element, and the identity goes to nobody*/
	for(i=0;i<blib_graph_size(g);i++){
		blib_graph_set_dir_edge(g,0,i,BLIB_GRAPH_NO_EDGE_VAL);
		blib_graph_set_dir_edge(g,i,0,1);
	}
	
	printf("Our graph has %d verts\n",blib_graph_size(g));
	
	dom_set(g,in_dom_set);
	j=0;
	for(i=0;i<group_size;i++){
		if(in_dom_set[i])
			j++;
	}
	printf("There min dom set is size %d\n",j);
	
	blib_graph_free(g);
	for(i=0;i<group_size;i++)
		free(perm_table[i]);
	free(perm_table);
	free(in_dom_set);
	free(perm1);free(perm2);free(perm3);
	return 0;
}