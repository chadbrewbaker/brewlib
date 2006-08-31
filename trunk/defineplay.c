#include <stdio.h>

#define BONZO #
#define REDEFINE(x,y) ###undefine x\
###define x y

#define BONZO #

int main(){

#define BOB_FRO 1




printf("%d\n",BOB_FRO);

#undef BOB_FRO
#define BOB_FRO 2


printf("%d\n",BOB_FRO);


return 0;
}
