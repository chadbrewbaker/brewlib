#include <blib_partition.h>
#include 

int main(){

int arr[]={1,  0,  2,  3,  7,  5,  4,  6,  10,  8,  9,  11,  15,  13,  12,  14,  18,  16,  17,  19};

	blib_partition_bicolor(pre_part,arr,1);
	/*blib_partition_print(pre_part,stderr);
	fprintf(stderr,"(");
	for(j=0;j<=i;j++){
		fprintf(stderr,"%d, ", used[j]);	
	}
	fprintf(stderr,")\n");*/
	if(blib_partition_assert(pre_part)){
		blib_partition_print(pre_part,stderr);
	}
	
	blib_graph_auto_persistent(g,orbits,NULL,NULL,pre_part,stuff);




}
