#ifndef _BLIB_CELL_STACK_DEF_
#define _BLIB_CELL_STACK_DEF_
#include "blib_error.h"
typedef struct blib_cell_stack_t{
	int max_cell_size;
	int** arr;/*Each element has 1 int to give the size followed by the listing   */
	int* arr_size;
	int cells_allocated;
	int cells_used;
}blib_cell_stack;


int blib_cell_stack_depth(blib_cell_stack* cell_stack){
	return cell_stack->cells_used;
}

blib_cell_stack* blib_cell_stack_allocate( int max_cell_size, int depth)
{
	int i;
	blib_cell_stack* cell_stack;
	cell_stack = (blib_cell_stack*)BLIB_MALLOC(sizeof(blib_cell_stack));
	cell_stack->arr=(int**) BLIB_MALLOC(sizeof(int*)*depth);
	for(i=0;i<depth;i++)
		cell_stack->arr[i]=(int*)BLIB_MALLOC(sizeof(int)*(max_cell_size+1));
	cell_stack->arr_size=(int*)BLIB_MALLOC(sizeof(int)*depth);
	cell_stack->max_cell_size = max_cell_size;
	cell_stack->cells_allocated=depth;
	cell_stack->cells_used=0;
	return cell_stack;
}




void blib_cell_stack_free(blib_cell_stack* cell_stack)
{
	int i;
	for(i=0;i<cell_stack->cells_allocated;i++)
		free(cell_stack->arr[i]);
	free(cell_stack->arr);
	free(cell_stack->arr_size);
	free(cell_stack);
}




blib_cell_stack* blib_cell_stack_grow(blib_cell_stack* cell_stack, int more_depth){
	int i,j;
	if(more_depth <=0){
		BLIB_ERROR("Now why the heck would you want to do that");
	}
	blib_cell_stack* new_cell_stack;
	new_cell_stack=blib_cell_stack_allocate(cell_stack->max_cell_size,cell_stack->cells_allocated+more_depth);
	for(i=0;i< cell_stack->cells_used;i++){
		for(j=0;j<cell_stack->max_cell_size;j++)
			new_cell_stack->arr[i][j]=cell_stack->arr[i][j];
	}
	blib_cell_stack_free(cell_stack);
	return new_cell_stack;
}


void blib_cell_stack_copy(blib_cell_stack* a, blib_cell_stack* b){
	int i,j;
	if(b->cells_allocated < a->cells_used)
		blib_cell_stack_grow(b,a->cells_used);
	for(i=0;i<a->cells_used;i++){
		for(j=0;j<a->max_cell_size;j++)
			b->arr[i][j]=a->arr[i][j];
	}
}


void blib_cell_stack_fatten(blib_cell_stack* cell_stack, int new_cell_size){
	blib_cell_stack* new_cell_stack;
	new_cell_stack=blib_cell_stack_allocate(new_cell_size,cell_stack->cells_allocated);
	blib_cell_stack_copy(cell_stack,new_cell_stack);
	blib_cell_stack_free(cell_stack);
	cell_stack=new_cell_stack;
}

/*Pushes cell on the the cell_stack. For growing pourposes we should probably return a new value*/
blib_cell_stack*  blib_cell_stack_push(blib_cell_stack* cell_stack, int* cell, int cell_size)
{
	int i;
	if(cell_size > cell_stack->max_cell_size)
	{BLIB_ERROR("stack not fat enough");}
	if(cell_stack->cells_allocated==cell_stack->cells_used) 
		cell_stack=blib_cell_stack_grow(cell_stack, (cell_stack->cells_allocated/3)+1);
	for(i=0;i<cell_size;i++)
		cell_stack->arr[cell_stack->cells_used][i]=cell[i];
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
		cell[i]=cell_stack->arr[(cell_stack->cells_used)-1][i];
	}
	cell_stack->cells_used--;
	return 0;
}
#endif /*_BLIB_CELL_STACK_DEF_*/