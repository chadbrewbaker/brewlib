#ifndef _BLIB_PARTITION_DEF_
#define _BLIB_PARTITION_DEF_
#include "blib_error.h"
#include "blib_sort.h"
#include "blib_error.h"

typedef struct blib_partition_t{
	int* cells;
	int* perm;
	int size;
	int cell_count;
}blib_partition;

/*Thread safe temp storage*/
blib_partition** BLIB_PARTITION_SCRATCH=NULL;
int BLI_PARTITION_SCRATCH_ALLOCATED=0;
/*Which are being used*/
int* BLIB_PARTITION_SCRATCH_IN_USE=0;
/*Total being used*/
int BLIB_PARTITION_SCRATCH_USED=0;

blib_partition* blib_partition_allocate(int size){
	int i;
	blib_partition* part;
	part=(blib_partition*) BLIB_MALLOC(sizeof(blib_partition));
	part->size=size;
	part->cells=(int*)BLIB_MALLOC(sizeof(int)*size);
	part->perm=(int*)BLIB_MALLOC(sizeof(int)*size);
	for(i=0;i<size;i++)
		part->perm[i]=i;
	part->cell_count=1;
	part->cells[0]=0;
	return part;
}

void blib_partition_free(blib_partition* part){
	if(part==NULL){
		return;
	}
	free(part->cells);
	free(part->perm);
	free(part);
}

/*Frees all unused global temp storage*/
void blib_partition_garbage(){
	/*int i;
	for(i=0;i<BLIB_PARTITION_SCRATCH_ALLOCATED;i++){
		if(!BLIB_PARTITION_SCRATCH_IN_USE[i]){
			blib_partition_free(BLIB_PARTITION_SCRATCH[i]);
		}
	}*/
}



int blib_partition_size(blib_partition* part){
	return part->size;
}

void blib_partition_reset(blib_partition* part){
	int i;
	for(i=0;i<blib_partition_size(part);i++){
		part->perm[i]=i;
	}
	part->cells[0]=0;
	part->cell_count=1;
}

int blib_partition_assert(blib_partition* part){
	int i,sum;

	/*DANGER WILL ROBINSON THIS IS TURNED OFF*/
	return 0;
	
	if(part==NULL){
		BLIB_ERROR(" ");
		return 1;
	}
	if(part->perm==NULL || part->cells==NULL){
		BLIB_ERROR(" ");
		return 1;
	}
	
	if(part->cell_count>part->size){
		BLIB_ERROR(" ");
		return 1;
	}	
	if(part->cell_count<0 || part->size<0){
		BLIB_ERROR(" ");
		return 1;
	}
	sum=0;
	for(i=1;i<part->cell_count;i++){
		sum+=part->cells[i]-part->cells[i-1];
		if(part->cells[i]<=part->cells[i-1]){
			
			BLIB_ERROR("(%d) (%d) ",part->cells[i],part->cells[i-1]);
			
			return 1;
		}
	}
	sum+=part->size-part->cells[part->cell_count-1];
	if(sum!=part->size){
		i=part->cells[0];
		BLIB_ERROR(" ");
		return 1;
	}
	return 0;	
}

void blib_partition_reorder(blib_partition* part, int* new_perm){
	int i;
	for(i=0;i<blib_partition_size(part);i++){
		part->perm[i]=new_perm[i];
	}
	blib_partition_assert(part);
}

void blib_partition_recell(blib_partition* part, int* new_cells, int new_cell_count){
	int i;
	for(i=0;i<new_cell_count;i++){
		part->cells[i]=new_cells[i];
	}
	part->cell_count=new_cell_count;
	blib_partition_assert(part);
}




void blib_partition_swap_elts(blib_partition* part, int a, int b){
	int i;
	for(i=0;i<blib_partition_size(part);i++){
		
		if(part->perm[i]==a){
			part->perm[i]=b;
		}
		else{
			if(part->perm[i]==b)
				part->perm[i]=a;
		}
	}
}

void blib_partition_resplit(blib_partition* part, int* new_cell_starts, int new_cell_count){
	int i;
	for(i=0;i<new_cell_count;i++)
		part->cells[i]=new_cell_starts[i];
	part->cell_count=new_cell_count;
	blib_partition_assert(part);
}

void blib_partition_swap_cells(blib_partition* part, int a, int b){BLIB_ERROR("NOT IMPLEMENTED");}
/*Merges cells a and b by getting rid of cell b and tacking it on the end of cell a*/
void blib_partition_merge_cells(blib_partition* part,int a, int b){BLIB_ERROR("NOT IMPLEMENTED");}
/*Reverses the ordering of cells*/
void blib_partition_reverse_cells(blib_partition* part){BLIB_ERROR("NOT IMPLEMENTED");}




/*sets the list units to be unit partitions at the front of the list*/
void blib_partition_bicolor(blib_partition* part, int* units, int length){
	int i,used;
	blib_partition_assert(part);
	/*A hack using cells as storge*/
	for(i=0;i<blib_partition_size(part);i++){
		part->cells[i]=1; 
	}
	if(length>part->size){
		BLIB_ERROR(" ");
	}
	for(i=0;i<length;i++){
		part->perm[i]=units[i];
		part->cells[units[i]]=0;
	}
	used=length;
	
	for(i=0;used<blib_partition_size(part);i++){
		/*When we come across a unused place tack it on the end*/
		if(part->cells[i]){
			part->perm[used]=i;
			used++;
		}
	}
	part->cells[0]=0;
	if(length< blib_partition_size(part)){
		part->cells[1]=length;	
		part->cell_count=2;
	}
	else{
		part->cell_count=1;		
	}
	blib_partition_assert(part);
	
}



int blib_partition_cell_size(blib_partition* part, int cell_index)
{
	if(cell_index >= part->cell_count){
		BLIB_ERROR(" CELL DOES NOT EXIST");
		return 99999;
	}
	if(cell_index+1 == part->cell_count)/*the last cell*/
		return part->size - part->cells[cell_index];
	else
		return part->cells[cell_index+1]-part->cells[cell_index];
}



int blib_partition_nth_item(blib_partition* part, int n)
{
     #ifdef BLIB_DEBUG
		if(n >= blib_partition_size(part)    || n<0){
			BLIB_ERROR("index out of bounds");
		}
     #endif /*BLIB_DEBUG*/
	return part->perm[n];
}


int blib_partition_nth_cell(blib_partition* part, int  n){
    #ifdef BLIB_DEBUG
	if(n>=blib_partition_size(part) || n<0)
		BLIB_ERROR("Index out of bounds");
    #endif
	return part->cells[n];
}


int blib_partition_cell_count(blib_partition* part){
	return part->cell_count;
}

/*If second argument is NULL then it allocates a new one*/
blib_partition* blib_partition_copy(blib_partition* a, blib_partition* b){
	int i;
    #ifdef BLIB_DEBUG
	if(a==NULL){
		BLIB_ERROR("copying a null pointer");	
	}
   #endif
	
	if(b == NULL)
		b=blib_partition_allocate(a->size);
	if(b->size != a->size){
		blib_partition_free(b);
		b=blib_partition_allocate(a->size);
     #ifdef BLIB_DEBUG
		if(b==NULL){
			BLIB_ERROR("Couldn't allocate");
		}
     #endif
	}
	for(i=0;i < blib_partition_cell_count(a);i++)
		b->cells[i]=blib_partition_nth_cell(a,i);
	for(i=0;i<blib_partition_size(a);i++)
		b->perm[i] = blib_partition_nth_item(a,i);
	b->cell_count=blib_partition_cell_count(a);
    #ifdef BLIB_DEBUG
	blib_partition_assert(b);
    #endif
	return b;
}


int blib_partition_front_unit_count(blib_partition* part)
{
	int i,count;
	blib_partition_assert(part);
	count=0;
	for(i=0;i<blib_partition_cell_count(part);i++){
		if(blib_partition_cell_size(part,i)==1)
			count++;
		else
			break;
	}
	return count;
}

int blib_partition_get_cell(blib_partition* part, int index, int* value, int* size)
{
	int i;
	blib_partition_assert(part);
	if(index+1 > part->cell_count){
		BLIB_ERROR("Out of bounds");
	}
	(*size)= blib_partition_cell_size(part,index);
	for(i=0;i< (*size);i++)
		value[i]=part->perm[ part->cells[index]+i];
	blib_partition_assert(part);
	
	return 0;
}

int blib_partition_get_cell_at(blib_partition* part, int index, int* value, int* size)
{
	int i,cur_cell;
	
	blib_partition_assert(part);
	
	if(index >= part->size){
		BLIB_ERROR("Out of bounds");
	}
	cur_cell=0;
	for(i=1;i<part->cell_count;i++){
		if(index < part->cells[i])
			break;
		cur_cell++;
	}

	return blib_partition_get_cell(part,cur_cell,value,size);
}



/*Returns -1 if the element or cell does not exist*/
int blib_partition_fix_element(blib_partition* part, int cell_index, int element){
	int i,temp,cell_size;
	
	blib_partition_assert(part);
	if(cell_index > part->cell_count)
		return -1;
	cell_size=blib_partition_cell_size(part,cell_index);
	if(element >=cell_size|| cell_size<=0){
		BLIB_ERROR("OUT OF CELL BOUNDS");
		return -1;
	}
	if(cell_size ==1)
		return 0;
	/*Swap the element to the front of the cell*/
	temp=part->perm[part->cells[cell_index]];/*what was in front*/
	part->perm[part->cells[cell_index]]= part->perm[part->cells[cell_index]+element];
	part->perm[part->cells[cell_index]+element]=temp;
	/*re-adjust cell indexes*/
	for(i=part->cell_count;i>cell_index;i--)
		part->cells[i]=part->cells[i-1];
	part->cells[cell_index+1]++;
	part->cell_count++;
	blib_partition_assert(part);
	
	return 0;
}




/*Pass a NULL into dirty_cell_arr if you don't want to return the set of cells that changed value*/

void blib_partition_split_by_key(blib_partition* part, int* keys,int* dirty_cell_arr, int* dirty_cells){
	int i,j,size,cells,index,new_cells,parent_used;
	/*Split each cell by key*/
	blib_partition_assert(part);
	
	cells=blib_partition_cell_count(part);
	index=0;new_cells=0;
	(*dirty_cells)=0;
	for(i=0;i<cells;i++){
		size=blib_partition_cell_size(part,i);
		blib_sort_brute(&part->perm[index],&keys[index],size,1);
#ifdef BLIB_DEBUG
		blib_sort_assert(&keys[index],size,1);
#endif
		parent_used=0;
		for(j=0;j<size-1;j++){
			if(keys[index+j]!=keys[index+j+1]){
				part->cells[cells+new_cells]=index+j+1;
				if(dirty_cell_arr !=NULL){
					if(!parent_used){
						dirty_cell_arr[(*dirty_cells)]=index;
						(*dirty_cells)++;
						parent_used=1;
					}
					dirty_cell_arr[(*dirty_cells)]=index+j+1;
					(*dirty_cells)++;
				}
				new_cells++;
			}
		}
		index+=size;
	}
	blib_sort(part->cells,part->cells,cells+new_cells,1);
#ifdef BLIB_DEBUG
	if((cells+new_cells)>part->size){
		BLIB_ERROR(" ");
	}
#endif
	part->cell_count=cells+new_cells;

	blib_partition_assert(part);
}

void blib_partition_get_perm(blib_partition* part, int* value)
{
	int i;
	for(i=0;i<part->size;i++)
		value[i]=part->perm[i];
}
void blib_partition_print(blib_partition* part,FILE* stream){
	int i,j,used,is_first;
	used=0;
	for(i=0;i<part->cell_count;i++){
		if(used<1)
			fprintf(stream,"[");
		else
			fprintf(stream,",[");
		is_first=1;
		for(j=0;j<blib_partition_cell_size(part,i);j++){
			if(is_first){
				fprintf(stream,"%d",part->perm[used]+1);
				is_first=0;
			}
			else
				fprintf(stream,",%d",part->perm[used]+1);
			used++;	
		}
		fprintf(stream,"]");
	}
	fprintf(stream,"\n");
#ifdef BLIB_DEBUG
	fprintf(stream,"\ndumping the permutation<");
	for(i=0;i<blib_partition_size(part);i++){
		fprintf(stream,"%d, ",part->perm[i]);
	}
	fprintf(stream,">\n");
#endif
}

void blib_partition_print_dreadnaut(blib_partition* part,FILE* stream){
	int i,j,used;
	fprintf(stream,"f=[");
	used=0;
	for(i=0;i<part->cell_count;i++){
		if(i!=0){
			fprintf(stream,"|");
		}
		for(j=0;j<blib_partition_cell_size(part,i);j++){
			if(j!=0){
				fprintf(stream,", ");
			}
			fprintf(stream,"%d",part->perm[used]);
			used++;	
		}
	}
	fprintf(stream,"]\n");
}	





#ifdef BLIB_UNIT_TEST
/*checks allocate,free,size,nth_item*/

int blib_partition_allocate_unit(void){
	blib_partition* part;
	int i,fail;
	fail=0;
	part=blib_partition_allocate(30);
	if(part==NULL){
		BLIB_ERROR("didn't allocate");
		fail=1;
	}
	if(blib_partition_size(part)!=30){
		BLIB_ERROR("Either alloc or size is whack");
		fail=1;
	}
	for(i=0;i<blib_partition_size(part);i++){
		if(blib_partition_nth_item(part,i)!=i){
			BLIB_ERROR("Nth item is hosed");
			fail=1;
		}
	}
	blib_partition_free(part);
	return fail;
}
/*Tests reset*/
int blib_partition_reset_unit (){
	int i,fail;
	blib_partition* part;
	fail=0;
	part=blib_partition_allocate(30);
	part->cell_count=999;
	part->perm[3]=66;
	blib_partition_reset(part);
	for(i=0;i<blib_partition_size(part);i++){
		if(part->perm[i]!=i){
			BLIB_ERROR(" Didn't set properly");
			fail=1;
		}
	}
	if(part->cell_count !=1){
		BLIB_ERROR("didn't reset cells");
		fail=1;
	}
	if(part->cells[0]!=0){
		BLIB_ERROR("didn't reset first cell to 0");
		fail=1;
	}
	blib_partition_free(part);
	return fail;
}

int blib_partition_reorder_unit(void){
	blib_partition* part;
	int i,fail;
	int order[]={4,2,3,1,0};
	fail=0;
	part=blib_partition_allocate(5);
	blib_partition_reorder(part,order);
	for(i=0;i<blib_partition_size(part);i++){
		if(blib_partition_nth_item(part,i)!=order[i]){
			BLIB_ERROR("It didn't reorder properly");
			fail=1;
		}
	}
	blib_partition_free(part);
	return fail;
}

int blib_partition_recell_unit(void){
	blib_partition* part;
	int i,fail;
	int new_cells[]={0,2,3,4};
	fail=0;
	part=blib_partition_allocate(5);
	blib_partition_recell(part,new_cells,4);
	for(i=0;i<4;i++){
		if(part->cells[i]!=new_cells[i]){
			BLIB_ERROR(" cells not copied properly");	
			fail=1;
		}
	}
	blib_partition_free(part);
	return fail;
}

int blib_partition_swap_elts_unit(void){
	int fail;
	blib_partition* part;
	fail=0;
	part=blib_partition_allocate(10);
	blib_partition_swap_elts(part,0,9);
	if((part->perm[0]!=9)   || (part->perm[9]!=0)){
		BLIB_ERROR("Didn't swap");
		fail=1;
	}
	blib_partition_free(part);
	return fail;
}



int blib_partition_unit(){
	int i=0;
	if(blib_partition_allocate_unit()){
		i=1;
		BLIB_ERROR("Failure");
	}
	if(blib_partition_reset_unit()){
		i=1;
		BLIB_ERROR("Failure");
	}
	if(blib_partition_reorder_unit()){
		i=1;
		BLIB_ERROR("Failure")
	}
	if(blib_partition_recell_unit()){
		i=1;
		BLIB_ERROR("Failure");
	}
	if(blib_partition_swap_elts_unit()){
		i=1;
		BLIB_ERROR("Failure");
	}
	return i;
}
#endif



#endif /*_BLIB_PARTITION_DEF*/
