#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mergesort.h"

void mergesort(void *arr, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  char *other;
  if(arr && cmp) {
    other = malloc(num_elem*elem_size);
    other = mergesorth(arr, other, arr, num_elem, elem_size, cmp);
    if(arr != other) {
      memcpy(arr, other, num_elem*elem_size);
    }
  }
}

void *merge(void *f, void *s, void* using, void *other, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  char *first, *second, *result, *fend, *send;
  result = other;
  result += (first - (char *)using);
  fend = (char *)f;
  fend += (num_elem/2)*elem_size;
  send = (char *)s;
  send += (num_elem - num_elem/2)*elem_size;
  first = (char *)f;
  second = (char *)s;
  while(first < fend && second < send) {
    if(cmp((void *) first, (void*) second) < 0 )  {
      memcpy(result, second, elem_size);
      result += elem_size;
      second += elem_size;
    } else {
      memcpy(result, first, elem_size);
      result += elem_size;
      first += elem_size;
    }
  }
  if(first < fend) {
    memcpy(result, first, fend-second);
  } else {
    memcpy(result, second, send-second);
  }
  return other; 
}

void *mergesorth(void *using, void *other, void *start, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  char *second, *temp;
  if(num_elem < 2) {
    return using;
  }
  second = (char *)using;
  second += elem_size*(num_elem/2);
  temp = mergesorth(using, other, start,  num_elem/2, elem_size, cmp);
  if(using != mergesorth(using, other, second, num_elem - num_elem/2, elem_size, cmp)) {
    temp = using;
    using = other;
    other = temp;
  }
  return (void *)merge(start, second, using, other, num_elem, elem_size, cmp);
}

void mt_mergesort(void *arr, size_t num_elem, int chunk_size, size_t elem_size, Compare_fn cmp, int num_threads) {
}
