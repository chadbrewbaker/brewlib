#ifndef _BLIB_CELL_STACK_DEF_
#define _BLIB_CELL_STACK_DEF_
#include "blib_error.h"
typedef struct blib_cell_stack_t{
	int* arr;
	int* arr_size;
	int elts_allocated;
	int elts_used;
	int cells_used;
	int cells_allocated;
}blib_cell_stack;

int blib_cell_stack_depth(blib_cell_stack* cell_stack){
	return cell_stack->cells_used;
}

int blib_cell_stack_assert(blib_cell_stack* cs){
	int i,sum;
	if(cs==NULL){
		BLIB_ERROR("NOT ALLOCATED");	
		return 1;
	}
	if(cs->arr==NULL || cs->arr_size==NULL){
		BLIB_ERROR("NOT ALLOCATED");	
		return 1;
	}
	
	if(cs->cells_used > cs->cells_allocated){
		BLIB_ERROR(" ");
		return 1;
	}
	if(cs->elts_used > cs->elts_allocated){
		BLIB_ERROR(" ");
		return 1;
	}
	if(cs->cells_used > cs->elts_used){
		BLIB_ERROR(" ");
		return 1;
	}
	if(cs->cells_allocated > cs->elts_allocated){
		BLIB_ERROR("Why would you do that? ");
		return 1;
	}
	
	
	
	sum=0;
	for(i=0;i<cs->cells_used;i++){
		sum+=cs->arr_size[i];
	}
	if(sum != cs->elts_used){
		BLIB_ERROR(" ");
		return 1;
	}
	
	return 0;
}

blib_cell_stack* blib_cell_stack_allocate( int depth, int cells)
{
	blib_cell_stack* cell_stack;
	cell_stack = (blib_cell_stack*)BLIB_MALLOC(sizeof(blib_cell_stack));
	cell_stack->arr=(int*)BLIB_MALLOC(sizeof(int*)*depth);
	cell_stack->arr_size=(int*)BLIB_MALLOC(sizeof(int)*cells);
	cell_stack->elts_allocated=depth;
	cell_stack->cells_allocated=cells;
	cell_stack->cells_used=0;
	cell_stack->elts_used=0;
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	return cell_stack;
}





void blib_cell_stack_free(blib_cell_stack* cell_stack)
{
	free(cell_stack->arr);
	free(cell_stack->arr_size);
	free(cell_stack);
}




blib_cell_stack* blib_cell_stack_copy(blib_cell_stack* a, blib_cell_stack* b){
	
	int i;
	blib_cell_stack_assert(a);
	if(b->elts_allocated < a->elts_used){
		if(b!=NULL)
			blib_cell_stack_free(b);
		b=blib_cell_stack_allocate(a->elts_allocated,a->cells_allocated);
	}
	blib_cell_stack_assert(b);
	for(i=0;i< a->elts_used;i++){
		if(i>b->elts_allocated){
			BLIB_ERROR("i:%d b->elts_allocated:%d",i,b->elts_allocated);	
		}
		b->arr[i]=a->arr[i];
	}
	for(i=0;i<a->cells_used;i++)
		b->arr_size[i]=a->arr_size[i];
	b->elts_used=a->elts_used;
	b->cells_used=a->cells_used;
	
	if(blib_cell_stack_assert(b)){
		BLIB_ERROR(" ");	
	}
	
	return b;
}

blib_cell_stack* blib_cell_stack_grow(blib_cell_stack* cell_stack, int more_depth, int more_cells){
	int db1,db2;
	blib_cell_stack* new_cell_stack;
	if(more_depth <=0 && more_cells <=0){
		BLIB_ERROR("Now why the heck would you want to do that");
	}
	db1=(cell_stack->elts_allocated)+more_depth;
	db2=(cell_stack->cells_allocated)+more_cells;
	new_cell_stack=blib_cell_stack_allocate((cell_stack->elts_allocated)+more_depth,(cell_stack->cells_allocated)+more_cells);
	
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	
	return blib_cell_stack_copy(cell_stack,new_cell_stack);
}

/*Pushes cell on the the cell_stack.*/
blib_cell_stack*  blib_cell_stack_push(blib_cell_stack* cell_stack, int* cell, int cell_size)
{
	int i,db0,db1,db2;
	
	db0=cell_stack->cells_allocated;
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	if(cell_size > (cell_stack->elts_allocated - cell_stack->elts_used))
		cell_stack=blib_cell_stack_grow(cell_stack, (cell_stack->elts_allocated)/3 +1,0);
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	
	db1=cell_stack->cells_allocated;
	db2=(cell_stack->cells_allocated)/3 +1;
	if(cell_stack->cells_used >= cell_stack->cells_allocated)
		cell_stack=blib_cell_stack_grow(cell_stack, 0,(cell_stack->cells_allocated)/3 +1);
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	
	
	for(i=0;i<cell_size;i++)
		cell_stack->arr[(cell_stack->elts_used)+i]=cell[i];
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	
	cell_stack->elts_used += cell_size;
	cell_stack->arr_size[cell_stack->cells_used]=cell_size;
	cell_stack->cells_used++;
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	return cell_stack;
}

/*Assumes you were smart enough to allocate a list big enough to hold it*/
int blib_cell_stack_pop(blib_cell_stack* cell_stack, int* cell,int* cell_size)
{
	int i;
	if(cell_stack->cells_used<1){
		BLIB_ERROR(" ");
		return -1; /*empty*/
	}
	(*cell_size)=cell_stack->arr_size[(cell_stack->cells_used)-1];
	for(i=0;i<(*cell_size);i++){
		cell[i]=cell_stack->arr[(cell_stack->elts_used)- (*cell_size)+i];
	}
	cell_stack->cells_used--;
	cell_stack->elts_used -= (*cell_size);
	
	if(blib_cell_stack_assert(cell_stack)){
		BLIB_ERROR(" ");	
	}
	
	return 0;
}
#endif /*_BLIB_CELL_STACK_DEF_*/