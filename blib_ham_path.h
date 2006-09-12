#ifndef _BLIB_HAM_PATHS_DEF_
#define _BLIB_HAM_PATHS_DEF_
#include "blib_error.h"
#include "blib_graph.h"
#include "blib_partition.h"
#include "blib_graph_transform.h"


int COUNTER=0;

void blib_ham_path_hello_world(int* arr,int size,int* orbits){
	int i;
	COUNTER++;
	/*if(COUNTER%1000 == 0){*/
		printf("Howdy(");
		for(i=0;i<size;i++){
			printf("%d->",arr[i]);
		}
		printf(")\n");
	/*}*/
}
/*Dump this badboy to file*/
/*sizeOgraph, depth, (used),(scratch at 0).... -1, (scratch at 1)...-1,...., (scratch at depth-1)...-1 */

void blib_ham_path_dump(blib_graph* g,int depth, int* used, int** scratch, FILE* dest){
	int i,j;
	fprintf(dest,"%d %d\n",blib_graph_size(g),depth);
	for(i=0;i<depth;i++)
		fprintf(dest," %d ",used[i]);
	for(i=0;i<depth;i++){
		for(j=0;j<blib_graph_size(g);j++){
			if(scratch[i][j]>=0)
				fprintf(dest," %d ",j);
			}
		fprintf(dest," -1 ");
	}
}

void blib_ham_path_load(int* depth, int* used, int** scratch, FILE* src){
	int i,j,size,tmp;
	fscanf(src,"%d",&size);
	fscanf(src,"%d",depth);
	for(i=0;i<(*depth);i++){
		fscanf(src,"%d",(&used[i]));
	}
	for(i=0;i<(*depth);i++){
		while(1){
			fscanf(src,"%d",&tmp);
			if(tmp>=0){
				scratch[i][j]=1;
			}
			else{
				break;
			}
		}
	}
}



/*Traverse all paths not using the parents again*/
void blib_ham_path_sub(blib_graph* g,blib_graph* eg,void(*path_func)(int*,int,int*), int depth, int* used, int** scratch,int* junk,int* orbits,
					   blib_partition* pre_part,blib_graph_auto_storage* gstuff, blib_graph_auto_storage* egstuff,int* checked_for_auts)
{
	int i,j,k,l,edge_counter;
	FILE* out_file;
	/*vused,vunused,eused,eunused*/
	/*depth,(size-depth),depth-1, everything else*/
	int new_cells[4];
	int new_cell_count;
	/*record the path*/
	if(depth==blib_graph_size(g)){
		/*record the path*/
		/*Case 1 If the graph is isomorphic up to path reversal accept  */
		/*Case 2. */
		/*Right ------| is different from |------*/
		
		
		(*path_func)(used,depth,orbits);
		/*kill off isomorphic ancestors*/
		for(i=0;i<depth;i++){
			BLIB_DEBUG_ARR[i]=used[i];
		}
		BLIB_DEBUG_ARR[depth]=-1;
		
		/*Start with depth-2 because at depth-1 there is only the vertex (depth)*/
		for(i=depth-2;i>=1;i--){
			if(checked_for_auts[i])
				break;
			BLIB_DEBUG_X=i;
			/*Break out the edge colored isomorphism code to kill off identical siblings*/
			
			for(j=0;j<blib_graph_size(eg);j++){
				orbits[j]=0;
			}
			/*Mark this vert as part of the ham path*/
			for(j=0;j<=i;j++){
				orbits[used[j]]=1;
			}
			edge_counter=0;
			
			for(j=0;j<blib_graph_size(g);j++){
				for(k=j+1;k<blib_graph_size(g);k++){
					if(blib_graph_is_edge(g,j,k)){
						for(l=1;l<=i;l++){
							/*Mark this "edge" as used in the ham path*/
							if((used[l-1]==j && used[l]==k) || (used[l-1]==k && used[l]==j)){
								orbits[blib_graph_size(g)+edge_counter]=1;
								break;
							}
						}
						edge_counter++;
					}
				}
			}
			k=0;
			
			for(j=0;j<blib_graph_size(g);j++){
				if(orbits[j]){
					junk[k]=j;
					k++;
				}
			}
			for(j=0;j<blib_graph_size(g);j++){
				if(!orbits[j]){
					junk[k]=j;
					k++;
				}
			}
			for(j=blib_graph_size(g);j<blib_graph_size(eg);j++){
				if(orbits[j]){
					junk[k]=j;
					k++;
				}
			}
			for(j=blib_graph_size(g);j<blib_graph_size(eg);j++){
				if(!orbits[j]){
					junk[k]=j;
					k++;
				}
			}
			
			blib_partition_reorder(pre_part,junk);
			new_cell_count=1;
			new_cells[0]=0;
			if(i>0){
				new_cells[1]=i;
				new_cell_count++;
			}
			if(blib_graph_size(eg)>blib_graph_size(g)){
				new_cells[2]=blib_graph_size(g);
				new_cell_count++;
				if(i>1){
					new_cells[3]=blib_graph_size(g)+(i-1);
					new_cell_count++;
				}
			}
			blib_partition_recell(pre_part,new_cells,new_cell_count);
			blib_partition_print(pre_part,stderr);
			BLIB_ERROR("Calling Auto");
			out_file=fopen("larry.gr","w");
			BLIB_ERROR("is it null(%d)",out_file==NULL);
			blib_graph_print_dreadnaut(eg,out_file);
			blib_partition_print_dreadnaut(pre_part,out_file);
			fprintf(out_file,"x\no\n");
			BLIB_ERROR(" ");
			fclose(out_file);
			blib_graph_auto_persistent(eg,orbits,NULL,NULL,pre_part,egstuff);
			checked_for_auts[i]=1;
			for(j=0;j<blib_graph_size(g);j++){
				if(orbits[j]!=j){
				scratch[i][j]=-1;	
				}
			}
		}
		return;
	}
	/*First time looking at this node*/
	checked_for_auts[depth]=0;
		
	/*should make this into a sparse list*/
	for(i=0;i<g->size;i++)
		scratch[depth][i]=1;
	/*Mark the verts we have already used*/
	for(i=0;i<depth;i++)
		scratch[depth][used[i]]=-1;	
	for(i=0;i<blib_graph_size(g);i++){
		/*Already used so ignore it*/
		if(scratch[depth][i]<0)
			continue;
		if(!blib_graph_is_edge(g,used[depth-1],i))
			continue;
		used[depth]=i;
		blib_ham_path_sub(g,eg,path_func,depth+1,used, scratch,junk,orbits,pre_part,gstuff,egstuff,checked_for_auts);
	}
}

blib_ham_path(blib_graph* g, void(*path_func)(int*,int,int*) )
{	
	int* used;
	int* main_orbits;
	int* orbits;
	int* junk;
	int* checked_for_auts;
	int** scratch;
	int i;	
	blib_graph_auto_storage* gstuff;
	blib_graph* eg;
	blib_graph_auto_storage* egstuff;
	blib_partition* pre_part;

	scratch= (int**) BLIB_MALLOC(sizeof(int*)*blib_graph_size(g));
	used= (int*) BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	checked_for_auts=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	for(i=0;i<blib_graph_size(g);i++){
		scratch[i]=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	}
	
	
	/*prune out iso verts*/
	eg=blib_graph_transform_edge_adorn(g);
	pre_part=blib_partition_allocate(blib_graph_size(eg));
	main_orbits=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(g));
	orbits=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(eg));
	junk=(int*)BLIB_MALLOC(sizeof(int)*blib_graph_size(eg));
	egstuff=blib_graph_auto_init(eg,NULL,orbits,NULL);
	gstuff=blib_graph_auto_init(g,NULL,main_orbits,NULL);
	blib_graph_auto_persistent(g,main_orbits,NULL,NULL,NULL,gstuff);
	for(i=0;i<blib_graph_size(g);i++){
		if(main_orbits[i]==i){
			used[0]=i;
			blib_ham_path_sub(g,eg,path_func,1,used,scratch,junk,orbits,pre_part,gstuff,egstuff,checked_for_auts);
		}
	}
	blib_graph_auto_finalize(gstuff,g,NULL,orbits,NULL);
	blib_graph_auto_finalize(egstuff,eg,NULL,orbits,NULL);
	blib_graph_free(eg);
	for(i=0;i<blib_graph_size(g);i++)
		free(scratch[i]);
	free(checked_for_auts);
	free(scratch);
	free(used);
	free(main_orbits);							  
	free(orbits);
	free(junk);
}
#endif /*_BLIB_HAM_PATHS_*/
