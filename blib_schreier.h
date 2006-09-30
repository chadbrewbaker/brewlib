#ifndef _BLIB_SCHREIER_DEF_
#define _BLIB_SCHREIER_DEF_
#include "blib_error.h"


typedef struct blib_schreier_t{
	int size;
	/*In case we want to change the base permutation to something other than the identity*/
	int* base;
	/* The schreier representation. */
	/*Actually we can save have the space because the first n cords of the nth permutation are fixed*/
	int*** perms;
	/*Hmm. Question if we take Sn and take out all elements of G except the identity do we get a group back, and if so is this anti-group helpfull?*/
	int forb_count;
	int* temp;
	int* temp2;
	int* temp3;
	int** temp_stack;
	int* temp_stack_used;
	int temps_allocated;
	int temps_used;
	struct blib_schreier_t* temp_schreier;
	}blib_schreier;

/*Inline this badboy*/
int blib_schreier_size(blib_schreier* g){
	return g->size;
}

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
				}
				continue;
			}
			g->perms[i][j][0]=-1;
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
	
	for(i=0;i<size;i++){
		g->perms[i]=(int**)BLIB_MALLOC(sizeof(int*)*size);
		for(j=0;j<size;j++){
			g->perms[i][j]=(int*)BLIB_MALLOC(sizeof(int)*size);
		}
	}
	g->base=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp2=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp3=(int*)BLIB_MALLOC(sizeof(int)*size);
	g->temp_schreier=NULL;
	blib_schreier_reset(g);
	return g;
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
		}
		free(g->perms[i]);
	}
	for(i=0;i<g->temps_allocated;i++)
		free(g->temp_stack[i]);
	if(g->temp_stack)
		free(g->temp_stack);
	if(g->temp_stack_used)
		free(g->temp_stack_used);
	free(g->perms);
	free(g->temp);
	free(g->temp2);
	free(g->temp3);
	if(g->temp_schreier!=NULL)
		blib_schreier_free(g->temp_schreier);
	free(g);
}


/*Pray this doesn't overflow :)*/
int blib_schreier_order(blib_schreier* g){
	int i,j,size,prod,sum;
	prod=1;
	size=blib_schreier_size(g);
	for(i=0;i<size;i++){
		sum=1;
		for(j=0;j<size;j++){
			if(i==j)
				continue;
			if(g->perms[i][j][0]>=0)
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
	if(perm[0]<0){
		fprintf(stderr,"Entering schreier_inverse\n");
		for(i=0;i<blib_schreier_size(g);i++)
			fprintf(stderr,"%d ",perm[i]);
		fprintf(stderr,"\n");
		BLIB_ERROR(" ");
	}
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
		new_arr=(int**)BLIB_MALLOC(sizeof(int*)*(g->temps_allocated));
		new_used_arr=(int*)BLIB_MALLOC(sizeof(int)*(g->temps_allocated));
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
			g->temp_stack[i]=(int*)BLIB_MALLOC(sizeof(int)*blib_schreier_size(g));
			g->temp_stack_used[i]=0;
		}
		index=g->temps_used;
	}
	else{
		index=-1;
		for(i=0;i<g->temps_allocated;i++){
			if(!g->temp_stack_used[i]){
				index=i;
				break;
			}
		}
		if(index<0){
			BLIB_ERROR("DANGER DANGER");
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
		if(g->temps_used<0){
			BLIB_ERROR("TEMP ALLOC PROBLEM");
		}
	}
}

void print_perm(int* perm,int size){
	int i;
	fprintf(stderr,"[%d",perm[0]);
	for(i=1;i<size;i++)
		fprintf(stderr,",%d",perm[i]);
	fprintf(stderr,"]\n");
}
int TEST_COUNT=0;

/*Borked it to manipulate perm*/
int blib_schreier_test(blib_schreier* g, int* perm){
	int i,j;
	BLIB_ERROR("Test %d",TEST_COUNT++);
/*	for(i=0;i<blib_schreier_size(g);i++){
		g->temp[i]=perm[i];
	}*/
	for(i=0;i<blib_schreier_size(g);i++){
		/*BLIB_ERROR("For this level g=");
		print_perm(g->temp,blib_schreier_size(g));*/
		if(g->perms[i][perm[i]][0]>=0){
			/*BLIB_ERROR("h=");
			print_perm(g->perms[i][g->temp[i]],blib_schreier_size(g));*/
			blib_schreier_inverse(g,g->perms[i][perm[i]],g->temp2);
			/*BLIB_ERROR("h^-1=");
			print_perm(g->temp2,blib_schreier_size(g));*/
			blib_schreier_mult(g,g->temp2,perm,g->temp3);
			for(j=0;j<blib_schreier_size(g);j++)
				perm[j]=g->temp3[j];
		}
		else{
			return i;
		}
	}
	return blib_schreier_size(g);
}

int blib_schreier_test2(blib_schreier* g, int* perm){
	int i,j;
	for(i=0;i<blib_schreier_size(g);i++){
		if(g->perms[i][g->base[perm[i]]][0]>=0){
			blib_schreier_inverse(g,g->perms[i][g->base[perm[i]]],g->temp2);
			blib_schreier_mult(g,g->temp2,perm,g->temp3);
			for(j=0;j<blib_schreier_size(g);j++)
				perm[j]=g->temp3[j];
		}
		else{
			return i;
		}
	}
	return blib_schreier_size(g);
}

int ENTER_DEPTH=0;
int ENTER_COUNT=0;
int blib_schreier_enter(blib_schreier* g, int* new_perm)
{
	int i,j,k;
	int tmp;
	BLIB_ERROR("Lvl %d Entering %d:",ENTER_DEPTH,ENTER_COUNT++);
	print_perm(new_perm,blib_schreier_size(g));
	/*BLIB_ERROR(" On group:");
	blib_schreier_print(g,stderr);*/
	i=blib_schreier_test2(g,new_perm);
	BLIB_ERROR("Depth %d Test gave me back %d/%d",ENTER_DEPTH,i,blib_schreier_size(g));

	if(i==blib_schreier_size(g)){
		BLIB_ERROR("Depth %d returning because i=deg",ENTER_DEPTH);
		return 0;
	}
	/*Insert the permutation into Ui*/
	if(g->perms[i][new_perm[i]][0]>=0){
		BLIB_ERROR("**Hey this orbit %d->%d exists!! Test was wrong?:",i,new_perm[i]);
		print_perm(new_perm,blib_schreier_size(g));
		BLIB_ERROR("****");
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
			print_perm(g->perms[j][k],8);
			print_perm(new_perm,8);
			blib_schreier_mult(g,new_perm,g->perms[j][k],blib_schreier_temp(g,tmp));
			print_perm(blib_schreier_temp(g,tmp),8);
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

int blib_schreier_enter2(blib_schreier* g,int* new_perm){	
	int i,j,k;
	int tmp;
	i=blib_schreier_test2(g,new_perm);
	if(i==blib_schreier_size(g)){
		return 0;
	}
	/*Insert the permutation into Ui*/
	/*Do we need to re-base the last coords??? */
	for(j=0;j<blib_schreier_size(g);j++)
		g->perms[i][g->base[new_perm[i]]][j]=new_perm[j];
	tmp=blib_schreier_get_temp(g);
	for(j=0;j<=i;j++){
		for(k=0;k<blib_schreier_size(g);k++){
			if(g->perms[j][k][0]<0){
				continue;
			}
			blib_schreier_mult(g,new_perm,g->perms[j][k],blib_schreier_temp(g,tmp));
			blib_schreier_enter2(g, blib_schreier_temp(g,tmp));
		}
	}
	blib_schreier_free_temp(g,tmp);
	return 1;
}


blib_schreier* blib_schreier_change_base(blib_schreier* g, int* new_base){
	int i,j,size;
	blib_schreier* g_swap;
	size=blib_schreier_size(g);
	if(g->temp_schreier==NULL)
		g->temp_schreier=blib_schreier_alloc(size);
	blib_schreier_reset(g->temp_schreier);
	for(i=0;i<size;i++){
		g->temp_schreier->base[i]=new_base[i];
	}
	
	/*Copy U to a new group H*/
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			/*Can we skip the identity element of each row???*/
			if(g->perms[i][j][0]>=0 ){
				blib_schreier_enter2(g->temp_schreier,g->perms[i][j]);
			}
		}
	}
	/*Switcharoo the pointers so we don't have to copy anything*/
	g_swap=g->temp_schreier;
	g_swap->temp_schreier=g;
	g->temp_schreier=NULL;
	return g_swap;
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
	int i,x;

	/*int tmp[8];*/
	/*(0,1,3,7,6,4)(2,5)*/
	int bpa[]={1,3,5,7,0,2,4,6};
	/*(0,1,3,2)(4,5,7,6)*/
	int bpb[]={1,3,0,2,5,7,4,6};
	
	/*Generators of A4 (Alternating group on 5 elements)*/
	int perma[]={1,0,3,2,4};
	int permb[]={2,1,4,3,0};
	/*int cyc3[]={1,0,2};*/
	
	/*g=blib_schreier_alloc(3);
	blib_schreier_enter(g,cyc3);
	
	blib_schreier_print(g,stderr);
	
	blib_schreier_free(g);
	BLIB_ERROR("ping");*/

	
	
	g=blib_schreier_alloc(8);
	blib_schreier_enter2(g,bpa);
	BLIB_ERROR("*.......Done putting in first perm.........*.");
	blib_schreier_enter2(g,bpb);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);

	blib_schreier_free(g);
	

		
	g=blib_schreier_alloc(5);
	BLIB_ERROR("Entering the first permutation of A5");
	blib_schreier_enter2(g,permb);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);
	x=blib_schreier_order(g);
	if(x!=3){
		BLIB_ERROR("Wrong order:%d in group of base size %d",x,blib_schreier_size(g));
		return 1;
	}
	blib_schreier_enter2(g,perma);
	BLIB_ERROR("printing");
	blib_schreier_print(g,stderr);
	x=blib_schreier_order(g);
	if(x!=60){
		BLIB_ERROR("Wrong Order %d in group of base size %d",x,blib_schreier_size(g));
		return 1;
	}
	
	for(i=0;i<10;i++){
		g=blib_schreier_change_base(g,permb);
		g=blib_schreier_change_base(g,perma);
	}
	blib_schreier_free(g);
	return 0;	
}

int blib_schreier_unit(){
	/*BLIB_ERROR("Starting blib unit");
	blib_schreier_mem_unit();
	BLIB_ERROR("Done with mem unit");*/
	if(blib_schreier_group_unit())
		return 1;
	return 0;
}


#endif



#endif /*_BLIB_SCHREIER_DEF_*/

