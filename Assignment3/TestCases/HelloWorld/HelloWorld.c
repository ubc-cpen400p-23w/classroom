#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZER1   512
#define BUFSIZER2   ((BUFSIZER1/2) - 8)

int main(int argc, char **argv) {
  char *buf1R1;
  char *buf2R1;
  char *buf2R2;
  char *buf3R2;

  void* temp;

  buf1R1 = (char *) malloc(BUFSIZER1);
  buf2R1 = (char *) malloc(BUFSIZER1);

  temp = buf2R1 + 10;
  free(buf2R1);

  buf2R2 = (char *) malloc(BUFSIZER2);
  buf3R2 = (char *) malloc(BUFSIZER2);

  strncpy(temp, argv[1], BUFSIZER1-1);
  free(buf1R1);
  free(buf2R2);
  free(buf3R2);
}
