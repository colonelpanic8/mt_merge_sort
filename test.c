#include <stdio.h>
#include <stdlib.h>
#include "mergesort.h"

int compareint(const void* first, const void* second);

int main(int argc, char **argv) {
  int *arr, count;
  printf("wtf\n");
  printf("%d\n",argc);
  arr = malloc(sizeof(int)*(argc-1));
  count = 1;
  while(count < argc) {
    arr[count-1] = atoi(argv[count]);
    count++;
  }
  printarray(arr, argc-1);
  mergesort(arr, argc-1, sizeof(int), compareint);
  printarray(arr, argc-1);
  free(arr);
  return 0;
}

int compareint(const void* first, const void* second) {
  int f,s;
  f = *(int *)first;
  s = *(int *)second;
  if(f > s) {
    return 1;
  } else if (s > f) {
    return -1;
  } else {
    return 0;
  }
}
