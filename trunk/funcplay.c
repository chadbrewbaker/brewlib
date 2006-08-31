#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void foo(){
printf("I am foo\n");
}


void bar(){
printf("I am bar\n");
}

int main(){
void (*fptr)(void);

fptr=foo;
printf("%d %d %d %d %d %d\n",foo,sizeof(foo),bar,sizeof(bar),fptr,sizeof(fptr));

memcpy(fptr,foo,4);
memcpy(foo,bar,4);
memcpy(bar,fptr,4);
foo();

return 0;
}
