#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mergesort.h"
#include <sys/time.h>
#include "resource.h"
#include <unistd.h>
#include <malloc.h>

#define NUM_THREADS 8
#define CHUNK_SIZE 4096


struct timeval tv_delta(struct timeval start, struct timeval end){
  struct timeval delta = end;
  delta.tv_sec -= start.tv_sec;
  delta.tv_usec -= start.tv_usec;
  if (delta.tv_usec < 0) {
    delta.tv_usec += 1000000;
    delta.tv_sec--;
  }
  return delta;
}

struct rusage start_ru, end_ru;
struct timeval start_wall, end_wall;
struct timeval diff_ru, diff_su, diff_wall;


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

int is_permutation(int *A, int *B, int bound, int size) {
  int *AA, *BB, count;
  AA = malloc(sizeof(int)*bound);
  BB = malloc(sizeof(int)*bound);
  memset(AA, 0, sizeof(int)*bound);
  memset(BB, 0, sizeof(int)*bound);
  for(count = 0; count < size; count++) {
    AA[A[count]] += 1;
  }
  for(count = 0; count < size; count++) {
    BB[B[count]] += 1;
  }
  for(count = 0; count < bound; count++) {
    if(AA[count] != BB[count])
      return 0;
  }
  //printarray(AA, bound);
  return 1;
}


int main(int argc, char **argv) {
  int n, s, m, *A, *S, i;
  n = atoi(argv[1]);
  s = atoi(argv[2]);
  m = atoi(argv[3]);
  A = malloc(n*sizeof(int));
  srand(s);
  for (i = 0; i < n; i++)
    A[i] = rand() % m;
  S = malloc(n*sizeof(int));
  for (i = 0; i < n; i++)
    S[i] = A[i];
  gettimeofday(&start_wall, NULL);
  getrusage(RUSAGE_SELF, &start_ru);
  mergesort(S, (size_t)n, sizeof(int), compareint);
  gettimeofday(&end_wall, NULL);
  getrusage(RUSAGE_SELF, &end_ru);
  diff_ru = tv_delta(start_ru.ru_utime,end_ru.ru_utime);
  diff_su = tv_delta(start_ru.ru_stime,end_ru.ru_stime);
  diff_wall = tv_delta( start_wall,end_wall);
  printf("0 threads:  ");
  printf("%ld.%06ld", 
	 diff_wall.tv_sec, 
	 diff_wall.tv_usec);
  printf("s wall;  ");
  printf("%ld.%06ld", 
	 diff_ru.tv_sec,
	 diff_ru.tv_usec);
  printf("s user;  ");
  printf("%ld.%06ld", 
	 diff_su.tv_sec,
	 diff_su.tv_usec);
	 printf("s sys;\n");
	 /*
	 if(is_permutation(A, S, m, n)) {
	   printf("was a permutation\n");
	   }*/
	 //printarray(S, n);

  int count;
  for(count = 1; count < 9; count++) {
  for (i = 0; i < n; i++)
    S[i] = A[i];
  gettimeofday(&start_wall, NULL);
  getrusage(RUSAGE_SELF, &start_ru);
  mt_mergesort(S, n, CHUNK_SIZE, sizeof(int), compareint, count);
  gettimeofday(&end_wall, NULL);
  getrusage(RUSAGE_SELF, &end_ru);
  diff_ru = tv_delta(start_ru.ru_utime,end_ru.ru_utime);
  diff_su = tv_delta(start_ru.ru_stime,end_ru.ru_stime);
  diff_wall = tv_delta( start_wall,end_wall);
  printf("%d threads:  ", count);
  printf("%ld.%06ld", 
	 diff_wall.tv_sec, 
	 diff_wall.tv_usec);
  printf("s wall;  ");
  printf("%ld.%06ld", 
	 diff_ru.tv_sec,
	 diff_ru.tv_usec);
  printf("s user;  ");
  printf("%ld.%06ld", 
	 diff_su.tv_sec,
	 diff_su.tv_usec);
  printf("s sys;\n");
  /*
  	 if(is_permutation(A, S, m, n)) {
	   //printf("was a permutation\n");
	   }*/
  }
  //printarray(S,n);
  free(A);
  free(S);
  return 0;
}
