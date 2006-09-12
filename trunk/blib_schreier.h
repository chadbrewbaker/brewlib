#ifndef _BLIB_SCHREIER_DEF_
#define _BLIB_SCHREIER_DEF_
#include "blib_error.h"
typedef struct blib_schreier_t{
	int size;
	/*In case we want to change the base permutation to something other than the identity*/
	int* base;
	/*The number of perms at each level*/
	int *fixed_sizes;
	/* The schreier representation. */
	/*Actually we can save have the space because the first n cords of the nth permutation are fixed*/
	int *** perms;
	/*No clue if these are needed in the schrier rep. But might be cool if they are*/
	/*For this is there a faster way than backtrack to generate it?*/
	int perm_count;
	/*Hmm. Question if we take Sn and take out all elements of G except the identity do we get a group back, and if so is this anti-group helpfull?*/
	int forb_count;
	int* temp;
	int* temp2;
	int* temp3;
	int** temp_stack;
	int* temp_stack_used;
	int temps_allocated;
	int temps_used;
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
		/*Give each level the identity permutation*/
		for(j=0;j<size;j++){
			g->perms[i][0][j]=j;
		}
		g->fixed_sizes[i]=1;
	}
	g->perm_count=1;
	g->forb_count=0;
	g->temp=BLIB_MALLOC(sizeof(int)*size);
	g->temp2=BLIB_MALLOC(sizeof(int)*size);
	g->temp3=BLIB_MALLOC(sizeof(int)*size);
	g->temps_allocated=0;
	g->temps_used=0;
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
	for(i=0;i<g->temps_allocated;i++)
		free(g->temp_stack[i]);
	free(g->temp_stack);
	free(g->temp_stack_used);
	
	free(g->perms);
	free(g->temp);
	free(g->temp2);
	free(g->temp3);
	
	free(g);
}


int blib_schreier_size(blib_schreier* g){
	return g->size;
}

int blib_schreier_is_full(){return 0;}
void blib_schreier_get_perm(blib_schreier* g, int src, int dest, int* ret_perm){}
void blib_shchreier_forbid_perm(blib_schreier* g, int src, int dest){}


void blib_schreier_inverse(blib_schreier* g, int* perm, int* result){
	int i;
	for(i=0;i<blib_schreier_size(g);i++){
		result[perm[i]]=i;
	}
}
void blib_schreier_mult(blib_schreier* g, int* p1, int* p2, int* result){
	int i;
	for(i=0;i<blib_schreier_size(g);i++){
		result[i]=p1[p2[i]];
	}
}

int blib_schreier_test(blib_schreier* g, int* perm){
	int i,j,k,found;
	for(i=0;i<blib_schreier_size(g);i++){
		g->temp[i]=perm[i];
	}
	for(i=0;i<blib_schreier_size(g);i++){
		found=0;
		for(j=0;j<blib_schreier_size(g);j++){
			if(g->perms[i][j][i]==g->temp[i]){
				blib_schreier_inverse(g,g->perms[i][j],g->temp2);
				blib_schreier_mult(g,g->temp2,g->temp,g->temp3);
				for(k=0;k<blib_schreier_size(g);k++)
					g->temp[k]=g->temp3[k];
				found=1;
				break;
			}
		}
		if(!found)
			return i;
	}
	return blib_schreier_size(g);
}

int blib_schreier_get_temp(blib_schreier* g){
	int i,index;
	int** new_arr;
	int* new_used_arr;
	if(g->temps_allocated == g->temps_used){
		g->temps_allocated*=2;
		new_arr=BLIB_MALLOC(sizeof(int**)*(g->temps_allocated));
		new_used_arr=BLIB_MALLOC(sizeof(int*)*(g->temps_allocated));
		for(i=0;i<g->temps_used;i++){
			new_arr[i]=g->temp_stack[i];
			/*Actually these are all full so we could just set them to 1*/
			new_used_arr[i]=g->temp_stack_used[i];
		}
		free(g->temp_stack);
		g->temp_stack=new_arr;
		free(g->temp_stack_used);
		g->temp_stack_used=new_used_arr;
		for(i=g->temps_used;i<g->temps_allocated;i++){
			g->temp_stack[i]=BLIB_MALLOC(sizeof(int*)*blib_schreier_size(g));
			g->temp_stack_used[i]=0;
		}
		index=g->temps_used;
	}
	else{
		for(i=0;i<g->temps_allocated;i++){
			if(!g->temp_stack_used[i]){
				index=i;
				break;
			}
		}
	}
	g->temps_used++;
	g->temp_stack_used[index]=1;
	return index;
}

int* blib_schreier_temp(blib_schreier* g, int index){
	return g->temp_stack[index];
}

void blib_schreier_free_temp(blib_schreier*g, int index){
	if(!g->temp_stack_used[index]){
		BLIB_ERROR("Trying to free an empty temp variable");
	}
	else{
		g->temp_stack_used[index]=0;
		g->temps_used--;
	}
}


int blib_schreier_enter(blib_schreier* g, int* new_perm)
{
	int i,j;
	int tmp;
	i=blib_schreier_test(g,new_perm);
	if(i!=blib_schreier_size(g)){
		for(j=i;j<blib_schreier_size(g);j++){
			tmp=blib_schreier_get_temp(g);
			blib_schreier_mult(g,g->perms[i][j],new_perm,blib_schreier_temp(g,tmp));
			blib_schreier_enter(g, blib_schreier_temp(g,tmp));
			blib_schreier_free_temp(g,tmp);
		}
		return 1;
	}
	return 0;
}


void blib_schreier_test2(blib_schreier* g, int* base, int* perm){
#ifdef I_AM_TOO_LAZY_TO_COMMENT_THIS_OUT
	int i,j,x;
	for(i=0;i<blib_schreier_size(g);i++){
		x=perm[base[i]];
		if(1/*there is an h in g.U[i] such that h[base[i]]==x*/)
		{
			blib_schreier_inverse(g,h,g->temp);
			blib_schreier_mult(g,g->temp,perm,g->temp2);
			for(j=0;j<blib_schreier_size(g);j++){
				perm[j]=g->temp2[j];
			}
		}
		else
			return i;
	}
	return blib_schreier_size(g);
#endif
}

void blib_schreier_enter2(blib_schreier* g,int* perm){
#ifdef BLAHBLAH
	int i,j;
	i=blib_schreier_test2(g,perm);
	if(i== blib_schreier_size(g)){
		return;
	}
	else{
		/*Append perm to U[i]*/
	}
	for(j=0;j<=i;j++){
		/* Take every permutation in U[j], multiply it with g, then enter it in g under the base*/
		
	}
#endif

}

void blib_schreier_change_base(blib_schreier* g, int* new_base){
	int i;
	for(i=0;i<blib_schreier_size(g);i++){
		/*set U'[i] to the identity perm*/
	}
	/*Store U' as a new group H*/
	for(i=0;i<blib_schreier_size(g);i++){
		/*Enter every permutation from our oringinal group into the empty group h using the new base*/
		/*i.e. copy the dang thing???*/
	}
}

/*
void blib_schreier_print(blib_schreier* g, FILE* dest){
	int i,j,printed;
	fprintf(dest,"<");
	for(i=0;i<g->size;i++){
		printed=0;
		for(j=0;j<i;j++){
			if(blib_schreier_is_perm(g,i,j)){
				fprintf(dest, "%d ",j);
				printed=1;
				break;
			}
		}
		if(!printed)
			fprintf(dest,"%d ",i);
	}
	fprintf(stderr,">\n");
}
*/

#ifdef BLIB_UNIT_TEST
void blib_schreier_unit(){
	
	/*Create a simple group and test it's properties*/
	
	
	
}


#endif



#endif /*_BLIB_SCHREIER_DEF_*/

