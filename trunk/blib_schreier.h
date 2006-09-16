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
	int*** perms;
	int*** new_perms;
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



void blib_schreier_reset(blib_schreier* g){
	int i,j,k,size;
	size=blib_schreier_size(g);
	for(i=0;i<size;i++){
		/*Give each level the identity permutation*/
		/*Set everything else to empty*/
		for(j=0;j<size;j++){
			if(i==j){
				for(k=0;k<size;k++){
					g->perms[i][j][k]=k;
					g->new_perms[i][j][k]=k;
				}
				continue;
			}
			g->perms[i][j][0]=-1;
			g->new_perms[i][j][0]=-1;
		}
	}
	g->forb_count=0;
	for(i=0;i<size;i++){
		g->base[i]=i;
	}
	g->temps_used=0;
	g->temps_allocated=0;
	g->temp_stack_used=NULL;
	g->temp_stack=NULL;
}


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
		}
	}
	g->base=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->new_base=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp2=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp3=(int*)BLIB_MALLOC(sizeof(int)*size);
	blib_schreier_reset(g);
	return g;
}




int blib_schreier_size(blib_schreier* g){
	return g->size;
}

void blib_schreier_print(blib_schreier* g, FILE* dest){
	int i,j,k;
	fprintf(dest,"<");
	for(i=0;i<g->size;i++){
		for(j=0;j<g->size;j++){
			if(g->perms[i][j][0]<0)
				continue;
			fprintf(dest,"[%d",g->perms[i][j][0]);
			for(k=1;k<blib_schreier_size(g);k++){
				fprintf(dest,",%d",g->perms[i][j][k]);
			}
			fprintf(dest,"]");
		}
		fprintf(dest,"\n");
	}
	fprintf(dest,">\n");
}



void blib_schreier_free(blib_schreier* g){
	int i,j;
	for(i=0;i<g->size;i++){
		for(j=0;j<g->size;j++){
			free(g->perms[i][j]);
			free(g->new_perms[i][j]);
		}
		free(g->perms[i]);
		free(g->new_perms[i]);
	}
	for(i=0;i<g->temps_allocated;i++)
		free(g->temp_stack[i]);
	if(g->temp_stack)
		free(g->temp_stack);
	if(g->temp_stack_used)
		free(g->temp_stack_used);
	free(g->perms);
	free(g->new_perms);
	free(g->temp);
	free(g->temp2);
	free(g->temp3);
	free(g);
}


/*Pray this doesn't overflow :)*/
int blib_schreier_order(blib_schreier* g){
	int i,j,prod,sum;
	prod=1;
	for(i=0;i<blib_schreier_size(g);i++){
		sum=0;
		for(j=0;j<blib_schreier_size(g);j++){
			if(g->perms[i][j]>=0)
				sum++;
		}
		prod*=sum;
	}
	return prod;
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
		g->temps_allocated=(g->temps_allocated*2)+50;
		new_arr=BLIB_MALLOC(sizeof(int**)*(g->temps_allocated));
		new_used_arr=BLIB_MALLOC(sizeof(int*)*(g->temps_allocated));
		for(i=0;i<g->temps_used;i++){
			new_arr[i]=g->temp_stack[i];
			/*Actually these are all full so we could just set them to 1*/
			new_used_arr[i]=g->temp_stack_used[i];
		}
		if(g->temp_stack)
			free(g->temp_stack);
		if(g->temp_stack_used)
			free(g->temp_stack_used);

		g->temp_stack=new_arr;
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

void print_perm(int* perm,int size){
	int i;
	fprintf(stderr,"[%d",perm[0]);
	for(i=1;i<size;i++)
		fprintf(stderr,",%d",perm[i]);
	fprintf(stderr,"]\n");
}

int blib_schreier_test(blib_schreier* g, int* perm){
	int i,j,k,found;
	for(i=0;i<blib_schreier_size(g);i++){
		g->temp[i]=perm[i];
	}
	for(i=0;i<blib_schreier_size(g);i++){
		/*BLIB_ERROR("For this level g=");
		print_perm(g->temp,blib_schreier_size(g));*/
		if(g->perms[i][g->temp[i]][0]>=0){
			/*BLIB_ERROR("h=");
			print_perm(g->perms[i][g->temp[i]],blib_schreier_size(g));*/
			blib_schreier_inverse(g,g->perms[i][g->temp[i]],g->temp2);
			/*BLIB_ERROR("h^-1=");
			print_perm(g->temp2,blib_schreier_size(g));*/
			blib_schreier_mult(g,g->temp2,g->temp,g->temp3);
			for(k=0;k<blib_schreier_size(g);k++)
				g->temp[k]=g->temp3[k];
		}
		else{
			return i;
		}
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

int ENTER_DEPTH=0;
int blib_schreier_enter(blib_schreier* g, int* new_perm)
{
	int i,j,k;
	int tmp;
	BLIB_ERROR("Lvl %d Entering:",ENTER_DEPTH);
	print_perm(new_perm,blib_schreier_size(g));
	/*BLIB_ERROR(" On group:");
	blib_schreier_print(g,stderr);*/
	i=blib_schreier_test(g,new_perm);
	BLIB_ERROR("Depth %d Test gave me back %d/%d",ENTER_DEPTH,i,blib_schreier_size(g));

	if(i==blib_schreier_size(g)){
		BLIB_ERROR("Depth %d returning because i=deg",ENTER_DEPTH);
		return 0;
	}
	/*Insert the permutation into Ui*/
	if(g->perms[i][new_perm[i]][0]>=0){
		BLIB_ERROR("**Hey this permutation exists!! Test was wrong!:");
		print_perm(new_perm,blib_schreier_size(g));
	}
	for(j=0;j<blib_schreier_size(g);j++)
		g->perms[i][new_perm[i]][j]=new_perm[j];
	tmp=blib_schreier_get_temp(g);
	if(tmp!=ENTER_DEPTH){
		BLIB_ERROR("NAUGHTY!!");
		BLIB_ERROR(" NAUGHTY!! %d",tmp);
	}
	for(j=0;j<=i;j++){
		for(k=0;k<blib_schreier_size(g);k++){
			
			if(g->perms[j][k][0]<0){
				BLIB_ERROR("Depth %d rejected perm->[%d][%d]",ENTER_DEPTH,j,k);
				continue;
			}
			BLIB_ERROR("About to mult with tmp=%d ",tmp);
			blib_schreier_mult(g,new_perm,g->perms[j][k],blib_schreier_temp(g,tmp));
			/*BLIB_ERROR("Entering with perm=  ");
			print_perm(blib_schreier_temp(g,tmp),blib_schreier_size(g));*/
			BLIB_ERROR("Depth %d h=perm->[%d][%d]",ENTER_DEPTH,j,k);
			ENTER_DEPTH++;
			blib_schreier_enter(g, blib_schreier_temp(g,tmp));
			ENTER_DEPTH--;
			/*	BLIB_ERROR("After entering we have");
			blib_schreier_print(g,stderr);*/
		}
	}
	blib_schreier_free_temp(g,tmp);
	BLIB_ERROR("Depth %d returning because at end",ENTER_DEPTH);
	return 1;
}

int blib_schreier_enter2(blib_schreier* g,int* new_perm,int is_temp){
	int i,j,k;
	int tmp;
	i=blib_schreier_test2(g,new_perm,is_temp);
	if(i== blib_schreier_size(g)){
		return 0;
	}
	tmp=blib_schreier_get_temp(g);
	for(j=0;j<=i;j++){
		for(k=0;k<blib_schreier_size(g);k++){
			if(g->perms[j][k][0]<0)
				continue;
			blib_schreier_mult(g,new_perm,g->perms[j][k],blib_schreier_temp(g,tmp));
			blib_schreier_enter2(g, blib_schreier_temp(g,tmp),is_temp);
		}
	}
	blib_schreier_free_temp(g,tmp);
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





#ifdef BLIB_UNIT_TEST

/*Thrash and pray it doesn't crash*/
int blib_schreier_mem_unit(){
	blib_schreier* g;
	int i;
	g=blib_schreier_alloc(300);
	if(g==NULL){
		BLIB_ERROR(" ");
		return 1;
	}
	if(blib_schreier_size(g)!=300){
		BLIB_ERROR(" ");
		return 1;
	}
	
	
	for(i=0;i<100;i++){
		g=blib_schreier_alloc(10);
		blib_schreier_free(g);
	}
	g=blib_schreier_alloc(10);
	BLIB_ERROR("About to print");
	blib_schreier_print(g,stderr);
	BLIB_ERROR("Printed");
	blib_schreier_free(g);

	
	return 0;
}

/*Allocate a few groups to make sure the non-base part is working. Also check persistant*/
int blib_schreier_group_unit(){
	blib_schreier* g;
	int i;

	int tmp[8];
	/*(0,1,3,7,6,4)(2,5)*/
	int bpa[]={1,3,5,7,0,2,4,6};
	/*(0,1,3,2)(4,5,7,6)*/
	int bpb[]={1,3,0,2,5,7,4,6};
	
	/*Generators of A4 (Alternating group on 5 elements)*/
	int perma[]={1,0,3,2,4};
	int permb[]={2,1,4,3,0};
	int cyc3[]={1,0,2};
	
	g=blib_schreier_alloc(3);
	blib_schreier_enter(g,cyc3);
	
	blib_schreier_print(g,stderr);
	
	blib_schreier_free(g);
	BLIB_ERROR("ping");

	
	
	g=blib_schreier_alloc(8);
	blib_schreier_enter(g,bpa);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);

	blib_schreier_enter(g,bpb);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);

	blib_schreier_free(g);
	

	return 0;
	
	g=blib_schreier_alloc(5);
	BLIB_ERROR("Entering the first permutation of A5");
	blib_schreier_enter(g,permb);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);
	if(blib_schreier_order(g)!=3){
		BLIB_ERROR("Wrong order:%d",blib_schreier_order(g));
		return 1;
	}
	blib_schreier_enter(g,perma);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);
	if(blib_schreier_order(g)!=60){
		BLIB_ERROR("Wrong Order %d",blib_schreier_order(g));
		return 1;
	}
	blib_schreier_free(g);
	return 0;	
}
/*Allocate a few groups with base changing. Also check persistance.*/
int blib_schreier_group_base_unit(){
/*	blib_schreier* g;
	int i;
*/	
	return 0;
}

int blib_schreier_unit(){
	BLIB_ERROR("Starting blib unit");
	blib_schreier_mem_unit();
	BLIB_ERROR("Done with mem unit");
	if(blib_schreier_group_unit())
		return 1;
	if(blib_schreier_group_base_unit())
		return 1;
	return 0;
}


#endif



#endif /*_BLIB_SCHREIER_DEF_*/

