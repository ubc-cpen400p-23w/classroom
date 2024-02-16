#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

char *retptr(void* l) {
  char p ,*q ;
  q = l ;
  return q ; /* deallocation on the stack */
}

int main( ){
  char *a , *b;
  int i ;
  a = malloc(16) ;
  b = a + 5;
  
  b = retptr(b) ;
  free(a) ;

  *b = 'c' ; /* use after free */
}