#ifndef _BLIB_SCHREIER_DEF_
#define _BLIB_SCHREIER_DEF_
#include "blib_error.h"
typedef struct blib_schreier_t{
	int size;
	/*In case we want to change the base permutation to something other than the identity*/
	int* base;
	int* new_base;
	/* The schreier representation. */
	/*Actually we can save have the space because the first n cords of the nth permutation are fixed*/
	int *** perms;
	int *** new_perms;
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
	int i,j;
	blib_schreier* g;
	g=BLIB_MALLOC(sizeof(blib_schreier));
	g->size=size;
	g->perms=(int***)BLIB_MALLOC(sizeof(int**)*size);
	g->new_perms=(int***)BLIB_MALLOC(sizeof(int**)*size);
	for(i=0;i<size;i++){
		g->perms[i]=(int**)BLIB_MALLOC(sizeof(int*)*size);
		g->new_perms[i]=(int**)BLIB_MALLOC(sizeof(int*)*size);
		for(j=0;j<size;j++){
			g->perms[i][j]=(int*)BLIB_MALLOC(sizeof(int)*size);
			g->new_perms[i][j]=(int*)BLIB_MALLOC(sizeof(int)*size);
			g->perms[i][j][0]=-1;
			g->new_perms[i][j][0]=-1;
			/*Start out with no permutations*/
		}
		/*Give each level the identity permutation*/
		for(j=0;j<size;j++){
			g->perms[i][0][j]=j;
			g->new_perms[i][0][j]=j;
		}
	}
	g->perm_count=1;
	g->forb_count=0;
	g->base=(int*)BLIB_MALLOC(sizeof(int)*size);
	for(i=0;i<size;i++){
		g->base[i]=i;
	}
	g->new_base=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp2=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp3=(int*)BLIB_MALLOC(sizeof(int)*size);
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

int blib_schreier_test2(blib_schreier* g, int* perm, int is_temp){
	int i,j,x,y,found;
	for(i=0;i<blib_schreier_size(g);i++){
		if(!is_temp)
			x=perm[g->base[i]];
		else
			x=perm[g->new_base[i]];
		for(j=0;j<blib_schreier_size(g);j++){
			if(!is_temp)
				y=g->perms[i][j][g->base[i]];
			else
				y=g->new_perms[i][j][g->new_base[i]];
			if(y==x)
			{
				if(!is_temp)
					blib_schreier_inverse(g,g->perms[i][j],g->temp2);
				else
					blib_schreier_inverse(g,g->new_perms[i][j],g->temp2);
				blib_schreier_mult(g,g->temp2,perm,g->temp3);
				for(j=0;j<blib_schreier_size(g);j++){
					perm[j]=g->temp3[j];
				}
				found=1;
				break;
			}
		}
		if(!found)
			return i;
	}
	return blib_schreier_size(g);
}


int blib_schreier_enter(blib_schreier* g, int* new_perm)
{
	int i,j;
	int tmp;
	i=blib_schreier_test(g,new_perm);
	if(i==blib_schreier_size(g)){
		return 0;
	}
	for(j=i;j<blib_schreier_size(g);j++){
		if(g->perms[i][j][0]<0)
			continue;
		tmp=blib_schreier_get_temp(g);
		blib_schreier_mult(g,g->perms[i][j],new_perm,blib_schreier_temp(g,tmp));
		blib_schreier_enter(g, blib_schreier_temp(g,tmp));
		blib_schreier_free_temp(g,tmp);
	}
	return 1;
}

int blib_schreier_enter2(blib_schreier* g,int* new_perm,int is_temp){
	int i,j,k;
	int tmp;
	i=blib_schreier_test2(g,new_perm,is_temp);
	if(i== blib_schreier_size(g)){
		return 0;
	}
	for(j=0;j<=i;j++){
		for(k=0;k<blib_schreier_size(g);k++){
			if(g->perms[i][j][0]<0)
				continue;
			tmp=blib_schreier_get_temp(g);
			blib_schreier_mult(g,new_perm,g->perms[i][j],blib_schreier_temp(g,tmp));
			blib_schreier_enter2(g, blib_schreier_temp(g,tmp),is_temp);
			blib_schreier_free_temp(g,tmp);
		}
	}
	return 1;
}

void blib_schreier_change_base(blib_schreier* g, int* new_base){
	int i,j;
	for(i=0;i<blib_schreier_size(g);i++){
		for(j=0;j<blib_schreier_size(g);j++){
			g->new_perms[i][0][j]=j;
		}
		g->new_base[i]=i;
	}

	/*Store U' as a new group H*/
	for(i=0;i<blib_schreier_size(g);i++){
		for(j=0;j<blib_schreier_size(g);j++){
			blib_schreier_enter2(g,g->perms[i][j],1);
		}
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

