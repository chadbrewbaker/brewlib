#ifndef _BLIB_CELL_STACK_DEF_
#define _BLIB_CELL_STACK_DEF_
#include "blib_error.h"
typedef struct blib_cell_stack_t{
	int* arr;
	int* arr_size;
	int elts_allocated;
	int elts_used;
	int cells_used;
}blib_cell_stack;

int blib_cell_stack_depth(blib_cell_stack* cell_stack){
	return cell_stack->cells_used;
}



blib_cell_stack* blib_cell_stack_allocate( int depth)
{
	int i;
	blib_cell_stack* cell_stack;
	cell_stack = (blib_cell_stack*)BLIB_MALLOC(sizeof(blib_cell_stack));
	cell_stack->arr=(int*)BLIB_MALLOC(sizeof(int*)*depth);
	cell_stack->arr_size=(int*)BLIB_MALLOC(sizeof(int)*depth);
	cell_stack->elts_allocated=depth;
	cell_stack->cells_used=0;
	cell_stack->elts_used=0;
	return cell_stack;
}




void blib_cell_stack_free(blib_cell_stack* cell_stack)
{
	int i;
	free(cell_stack->arr);
	free(cell_stack->arr_size);
	free(cell_stack);
}







blib_cell_stack* blib_cell_stack_copy(blib_cell_stack* a, blib_cell_stack* b){
	int i;
	if(b->elts_allocated < a->elts_used){
		blib_cell_stack_free(b);
		b=blib_cell_stack_allocate(a->elts_allocated);
	}
	for(i=0;i< a->elts_used;i++)
		b->arr[i]=a->arr[i];
	for(i=0;i<a->cells_used;i++)
		b->arr_size[i]=a->arr_size[i];
	b->elts_used=a->elts_used;
	b->cells_used=a->cells_used;
	return b;
}

blib_cell_stack* blib_cell_stack_grow(blib_cell_stack* cell_stack, int more_depth){
	if(more_depth <=0){
		BLIB_ERROR("Now why the heck would you want to do that");
	}
	blib_cell_stack* new_cell_stack;
	new_cell_stack=blib_cell_stack_allocate(cell_stack->cells_allocated+more_depth);
	return blib_cell_stack_copy(cell_stack,new_cell_stack);
}

/*Pushes cell on the the cell_stack.*/
blib_cell_stack*  blib_cell_stack_push(blib_cell_stack* cell_stack, int* cell, int cell_size)
{
	int i;
	if(cell_size > cell_stack->elts_allocated-cell_stack->elts_used)
		cell_stack=blib_cell_stack_grow(cell_stack, (cell_stack->elts_allocated)/3 +1);
	for(i=0;i<cell_size;i++)
		cell_stack->arr[cell_stack->elts_used+i]=cell[i];
	cell_stack->elts_used += cell_size;
	cell_stack->arr_size[cell_stack->cells_used]=cell_size;
	cell_stack->cells_used++;
	return cell_stack;
}

/*Assumes you were smart enough to allocate a list big enough to hold it*/
int blib_cell_stack_pop(blib_cell_stack* cell_stack, int* cell,int* cell_size)
{
	int i;
	if(cell_stack->cells_used<1)
		return -1; /*empty*/
	*cell_size=cell_stack->arr_size[(cell_stack->cells_used)-1];
	for(i=0;i<*cell_size;i++){
		cell[i]=cell_stack->arr[(cell_stack->elts_used)- *cell_size+i];
	}
	cell_stack->cells_used--;
	cell_stack->elts_used -= *cell_size;
	return 0;
}
#endif /*_BLIB_CELL_STACK_DEF_*/