/*
 Chad Brewbaker
 9-27-2006
 minCyclicDesign.c

What is the minimum number of permutations that need to be tested in a black-box group to either identify a cyclic subgroup
 or prove that none exists.
*/
#include <stdio.h>
#inclde <stdlib.h>


#define SET_SIZE 4


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
	for(j=1 j<=n;j++){
		r=r+(scrap[j]-1)*factorial(n-j);
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



int is_vert_cover(blib_graph* g, int* in_cover, int* scratch){
	int i,size;
	size=blib_graph_size(g);
	for(i=0;i<size;i++)
		scratch[i]=0;
	for(i=0;i<size;i++){
		if(in_cover[i]){
			scratch[i]=in_cover[i];
			for(j=0;j<size;j++){
				if(blib_graph_is_dir_edge(g,i,j))
					scratch[j]=1;
			}
		}
	}
	for(i=0;i<size;i++){
		if(scratch[i]=0)
			return 0;
	}
	return 1;
}

dom_set_sub(blib_graph* g, int** dfs_stack,int* result, int* best_size,int* scratch,int depth){
	int i,size;
	size=blib_graph_size(g);
	if(!is_vert_cover(g,dfs_stack[depth],scratch))
		return;
	if((size-depth)< (*best_size)){
		(*best_size)=size-depth;
		for(i=0;i<size;i++)
			result[i]=dfs_stack[depth];
	}
	if(dfs_stack[depth+1]==NULL)
		dfs_stack[depth+1]=(int*)malloc(sizeof(int)blib_graph_size(g));
	for(i=0;i<size;i++){
		if(dfs_stack[depth][i]){
			for(j=0;j<size;j++){
				dfs_stack[depth+1][j]=dfs_stack[depth][j];
				dfs_stack[depth+1][i]=0;
				dom_set_sub(g,dfs_stack,result,best_size,scratch,depth+1);
			}
		}
	}
}

void dom_set(blib_graph* g, int* result){
	int i,j,k
	int** dfs_stack=(int*)malloc(sizeof(int)*blib_graph_size(g));
	
	dom_set_sub();
	
	/*Check the bounds on this*/
	for(i=0;i<best_size;i++){
		free(dfs_stack[i]);
	}
	free(dfs_stack);
}



int*
int main(){
	int** perm_table;
	int* in_dom_set;
	int* perm1;
	int* perm2;
	int* perm3;
	blib_graph* g;
	int group_size= factorial(SET_SIZE);
	int i,j,k,rank;
	g = blib_graph_alloc(group_size);
	perm_table=(int**)malloc(sizeof(int*)*group_size);
	in_cover=(int*)malloc(sizeof(int)*group_size);
	for(i=0;i<group_size;i++){
		perm_table[i]=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	}
	perm1=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	perm2=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	perm3=(int*)malloc(sizeof(int)*(SET_SIZE+1));
	/*Now fill in the perm_table with the permutation that goes there*/
	for(i=0;i<SET_SIZE){
		perm1[i]=i;
	}
	perm_copy(SET_SIZE,perm1,perm_table[0]);
	for(i=0;i<blib_graph_size(g);i++){
		perm_copy(SET_SIZE,perm_table[i],perm1);
		blib_graph_set_edge(g,i,i,1);
		while(1){
			
			perm_mult(SET_SIZE,perm_table[i],perm1,perm1);
			if(equiv_perm(SET_SIZE,perm_table[i],perm1)){
				break;
			}
			blib_graph_set_dir_edge(g,perm_lex_rank(SET_SIZE(perm1)),i,1);
		}
	}
	/*Now find the min vertex cover.  */
	
	dom_set(g,in_cover);
	j=0;
	for(i=0;i<group_size;i++){
		if(in_dom_set[i])
			j++;
	}
	printf("There min vert cover is size %d\n");
	
	blib_graph_free(g);
	for(i=0;i<group_size;i++)
		free(perm_table[i]);
	free(perm_table);
	free(in_dom_set);
	free(perm1);free(perm2);free(perm3);
	return 0;
}