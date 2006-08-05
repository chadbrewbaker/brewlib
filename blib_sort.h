#ifndef _BLIB_SORT_DEF_
#define _BLIB_SORT_DEF_
#include "blib_error.h"
#define BLIB_SORT_CUTOFF 25

void blib_sort_brute(int* vals,int* keys,int size, int is_ascending){
	int i,j,swap_key,swap_val,is_dirty;
	if(is_ascending){
		for(i=0;i<size;i++){
			is_dirty=0;
			for(j=i+1;j<size;j++){
				if(keys[j-1] >keys[j]){
					swap_key=keys[j];
					swap_val=vals[j];
					keys[j]=keys[j-1];
					vals[j]=vals[j-1];
					vals[j-1]=swap_val;
					keys[j-1]=swap_key;
					is_dirty=1;
				}
			}
			if(!is_dirty)
				break;
		}
	}
	else{
		for(i=0;i<size;i++){
			is_dirty=0;
			for(j=i+1;j<size;j++){
				if(keys[j-1] < keys[j]){
					swap_key=keys[j];
					swap_val=vals[j];
					keys[j]=keys[j-1];
					vals[j]=vals[j-1];
					vals[j-1]=swap_val;
					keys[j-1]=swap_key;
					is_dirty=1;
				}
			}
			if(!is_dirty)
				break;
		}
	}
	return;
}



void blib_sort(int* arr, int* keys,int size, int is_ascending){
	int i,right_used,left_used,swap_key,swap_val;
	
	/*Debug the rest sometime */
	blib_sort_brute(arr,keys,size,is_ascending);
	
	if(size<=BLIB_SORT_CUTOFF){
		blib_sort_brute(arr,keys,size,is_ascending);
		return;
	}
	/* Make a bitonic sequence*/
	blib_sort(arr,keys,(size/2),is_ascending);
	blib_sort(&arr[size/2],&keys[size/2],size-(size/2),!is_ascending);
	/*Merge the two*/
	left_used=right_used=0;
	for(i=0;i<size/2;i++){
		if(keys[left_used]<=keys[(size-1)-right_used])
			left_used++;
		else
			right_used++;
	}
	for(i=0;i<right_used;i++){
		swap_key=keys[(size-1)-i];
		swap_val=arr[(size-1)-i];
		keys[(size-1)-i]=keys[(size/2)-1-i];
		arr[(size-1)-i]=arr[(size/2)-1-i];
		keys[(size/2)-i]=swap_key;
		arr[(size/2)-i]=swap_val;
	}
	/*sort the merged halfs*/
	blib_sort(arr,keys,(size/2),is_ascending);
	blib_sort(&arr[size/2],&keys[size/2],size-(size/2),is_ascending);
}

#endif /*_BLIB_SORT_DEF_*/