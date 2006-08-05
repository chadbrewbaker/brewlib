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
		BLIB_ERROR("It's null baby");
		return;
	}
	free(part->cells);
	free(part->perm);
	free(part);
}



int blib_partition_cell_size(blib_partition* part, int cell_index)
{
	if(cell_index > (part->cell_count-1))
		return -1;
	if(cell_index+1 == part->cell_count)/*the last cell*/
		return part->size - part->cells[cell_index];
	else
		return part->cells[cell_index+1]-part->cells[cell_index];
}



int blib_partition_nth_item(blib_partition* part, int n)
{
	if(n >= part->size){
		BLIB_ERROR("OUT OF BOUNDS");
	}
	BLIB_ERROR("n is %d, and part has params size=%d cell_count=%d",n,part->size,part->cell_count);
	return part->perm[n];
	BLIB_ERROR("");
}


/*If second argument is NULL then it allocates a new one*/
blib_partition* blib_partition_copy(blib_partition* a, blib_partition* b){
	int i;
	if(b == NULL)
		b=blib_partition_allocate(a->size);
	if(b->size != a->size){
		blib_partition_free(b);
		b=blib_partition_allocate(a->size);
	}
	for(i=0;i < a->cell_count;i++)
		b->cells[i]=a->cells[i];
	for(i=0;i<a->size;i++)
		b->perm[i] = a->perm[i];
	b->cell_count=a->cell_count;
	return b;
}

int blib_partition_cell_count(blib_partition* part){
	return part->cell_count;
}

int blib_partition_front_unit_count(blib_partition* part)
{
	int i,count;
	count=0;
	for(i=0;i<blib_partition_cell_count(part);i++){
		if(blib_partition_cell_size(part,i)==1)
			count++;
		else
			break;
	}
	return count;
}




int blib_partition_size(blib_partition* part){
	return part->size;
}

int blib_partition_get_cell(blib_partition* part, int index, int* value, int* size)
{
	int i;
	if(index+1 > part->cell_count){
		BLIB_ERROR("Out of bounds");
	}
	*size= blib_partition_cell_size(part,index);
	for(i=0;i< *size;i++)
		value[i]=part->perm[ part->cells[index]+i];
	return 0;
}

int blib_partition_get_cell_at(blib_partition* part, int index, int* value, int* size)
{
	int i,cur_cell;
	if(index+1 > part->cell_count){
		BLIB_ERROR("Out of bounds");
	}
	cur_cell=0;
	for(i=0;i<part->cell_count;i++){
		if(index < part->cells[i])
			break;
		cur_cell++;
	}
	return blib_partition_get_cell(part,cur_cell,value,size);
}



/*Returns -1 if the element or cell does not exist*/
int blib_partition_fix_element(blib_partition* part, int cell_index, int element){
	int i,temp,cell_size;
	if(cell_index > part->cell_count)
		return -1;
	cell_size=blib_partition_cell_size(part,cell_index);
	if(element > (cell_size-1))
		return -1;
	if(cell_size ==1)
		return 0;
	BLIB_ERROR("");
	/*Swap the element to the front of the cell*/
	temp=part->perm[part->cells[cell_index]];
	part->perm[part->cells[cell_index]]= part->perm[part->cells[cell_index]+element];
	part->perm[part->cells[cell_index]+element]=temp;
	/*re-adjust cell indexes*/
	for(i=part->cells[part->cells[0]]; i> cell_index;i--)
		part->cells[i]=part->cells[i-1];
	part->cells[cell_index+1]++;
	part->cell_count++;
	return 0;
}




/*Pass a NULL into dirty_cell_arr if you don't want to return the set of cells that changed value*/

void blib_partition_split_by_key(blib_partition* part, int* keys,int* dirty_cell_arr, int* dirty_cells){
	int i,j,size,cells,index,new_cells,parent_used;
	/*Split each cell by key*/
	cells=blib_partition_cell_count(part);
	index=0;new_cells=0; *dirty_cells=0;
	for(i=0;i<cells;i++){
		size=blib_partition_cell_size(part,i);
		blib_sort(&part->perm[index],&keys[index],size,1);
		parent_used=0;
		for(j=0;j<size-1;j++){
			if(keys[index+j]!=keys[index+j+1]){
				part->cells[cells+new_cells]=index+j+1;
				if(dirty_cell_arr !=NULL){
					if(!parent_used){
						dirty_cell_arr[*dirty_cells]=index;
						*dirty_cells++;
						parent_used=1;
					}
					dirty_cell_arr[*dirty_cells]=index+j+1;
					*dirty_cells++;
				}
				new_cells++;
			}
		}
		index+=size;
	}
	blib_sort(part->cells,part->cells,cells+new_cells,1);
	part->cell_count=cells+new_cells;
}

void blib_partition_get_perm(blib_partition* part, int* value)
{
	int i;
	for(i=0;i<part->size;i++)
		value[i]=part->perm[i];
}
void blib_partition_print(blib_partition* part,FILE* stream){
	int i,cell_index;
	for(i=0;i<part->size;i++){
		if(part->cells[i]==i){
			if(i!=0)
				fprintf(stream,"]");
			fprintf(stream,"[");
		}
		fprintf(stream,"%d ",part->perm[i]);
	}
	fprintf(stream,"]\n");
}

#endif /*_BLIB_PARTITION_DEF*/
