#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mergesort.h"

void printarray(int *arr, int num_elem);
void merge(char *first, char *second, size_t num_elem, size_t elem_size, Compare_fn cmp);
void mt_merge(char *first, char *second, size_t fnum_elem, size_t snum_elem, size_t *arg);
void enqueue(Chunk *item);
void worker_routine(size_t *arg);
Chunk *dequeue(Chunk *prev);
void find_buddy(Chunk *curr, size_t *arg);
void mergesort_h(void *start, size_t num_elem, size_t elem_size, Compare_fn cmp);

pthread_mutex_t queutex = PTHREAD_MUTEX_INITIALIZER;
Chunk *queue = NULL;
Compare_fn func;


void mergesort(void *arr, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  if(arr && cmp) {
    mergesort_h(arr, num_elem, elem_size, cmp);
  }
}

void printarray(int *arr, int num_elem) {
  int count = 0;
  while (count < num_elem) {
    printf("%d, ", *arr++);
    count++;
  }
  printf("\n");
  return;
}

void merge(char *first, char *second, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  char *pos, *fend, *send, *arr, *res;
  arr = first;
  fend = first + (elem_size*(num_elem/2));
  send = second + (elem_size*(num_elem - (num_elem/2)));
  pos = malloc(num_elem*elem_size);
  res = pos;
  while(first < fend && second < send) {
    if(cmp((void *) first, (void*) second) < 0 )  {
      memcpy(pos, first, elem_size);
      pos += elem_size;
      first += elem_size;
    } else {
      memcpy(pos, second, elem_size);
      pos += elem_size;
      second += elem_size;
    }
  }
  if(first < fend) {
    memcpy(pos, first, fend-first);
  } else {
    memcpy(pos, second, send-second);
  }
  // printarray((int*)res, num_elem);
  memcpy(arr, res, num_elem*elem_size);
  if(pos) {
    free(res);
  }
}

void mergesort_h(void *start, size_t num_elem, size_t elem_size, Compare_fn cmp) {
  char *second;
  if(num_elem < 2) {
    return;
  }
  second = (char *)start;
  second += elem_size*(num_elem/2);
  mergesort_h(start,  num_elem/2, elem_size, cmp);
  mergesort_h(second, num_elem - num_elem/2, elem_size, cmp);
  merge(start, second, num_elem, elem_size, cmp);
}

void mt_mergesort(void *arr, size_t num_elem, int chunk_size, size_t elem_size, Compare_fn cmp, int num_threads) {
  Chunk *temp, *last;
  pthread_t *threads;
  int count;
  size_t arg[2];
  char *chunk_start, *end = (char *)arr + num_elem*elem_size;
  chunk_start = arr;
  queue = NULL;
  // printf("There will be %d chunks\n", num_elem/chunk_size);
  while(chunk_start < end) {
    temp = malloc(sizeof(Chunk));
    temp->arr = chunk_start;
    temp->next = NULL;
    temp->first = (int)(chunk_start-(char *)arr);
    temp->first = temp->first/elem_size;
    //printf("%d\n", temp->first);
    temp->size = chunk_size;
    temp->done = 0;
    if(!queue) {
      queue = temp;
      last = queue;
    } else {
      last->next = temp;
      last = last->next;
    }
    chunk_start += chunk_size*elem_size;
  }
  func = cmp;
  threads = malloc(sizeof(pthread_t)*num_threads);
  arg[0] = num_elem;
  arg[1] = elem_size;
  for(count = 0; count < num_threads; count++) {
    pthread_create((threads+count), NULL, (void *(*)(void*))worker_routine, arg);
  }
  for(count = 0; count < num_threads; count++) {
    pthread_join(threads[count], NULL);
  }
  return; 
}

void enqueue(Chunk *item) {
  Chunk *runner = queue;
  if(runner) {
    while(runner->next) {
      runner = runner->next;
    }
    runner->next = item;
    item->next = NULL;
    return;
  }
  queue = item;
}

struct timeval tv_delta2(struct timeval start, struct timeval end){
  struct timeval delta = end;
  delta.tv_sec -= start.tv_sec;
  delta.tv_usec -= start.tv_usec;
  if (delta.tv_usec < 0) {
    delta.tv_usec += 1000000;
    delta.tv_sec--;
  }
  return delta;
}

void worker_routine(size_t *arg) {
  Chunk *curr, *runner, *prev;
  int search;
  curr = NULL;
  while(1) {
    if(!curr) {
      pthread_mutex_lock(&queutex);
      curr = dequeue(NULL);
      pthread_mutex_unlock(&queutex);
    }
    if(curr) {
      curr->next = NULL;
      //printf("Thread %d, dequeued %d\n", pthread_self()%919, curr->first);
      if(curr->done) {
	if(curr->size == arg[0]) {
	  return;
	}
	while(1) {	
	  if(curr->first/curr->size % 2) {
	    search = curr->first - curr->size;
	  } else {
	    search = curr->first + curr->size;
	  }
	  prev = NULL;
	  pthread_mutex_lock(&queutex);
	  runner = queue;
	  while(runner) {
	    if(!curr) {
	      printf("wtf");
	      scanf("%d");
	    }
	    if(runner->first == search  && runner->done && runner->size == curr->size)
	      break;
	    prev = runner;
	    runner = runner->next;
	  }
	  if(runner){
	    runner = dequeue(prev);
	  } else {
	    //printf("Thread %d, enqueued %d\n", pthread_self()%919, curr->first);
	    enqueue(curr);
	  }
	  pthread_mutex_unlock(&queutex);
	  if(runner) {
	    if(curr->first > runner->first) {
	      prev = curr;
	      curr = runner;
	      runner = prev;
	    }
	    mt_merge(curr->arr, runner->arr, curr->size, runner->size, arg);
	    //printf("Thread %d merged %d and %d \n", pthread_self()%919, curr->first, runner->first );
	    curr->size += runner->size;
	    if(curr->size == arg[0]) {
	      return;
	    }
	    // free(runner);
	  }
	  else {
	    curr = NULL;
	    break;
	  }
	}
	
      } else {
	mergesort(curr->arr, curr->size, arg[1], func);
	curr->done = 1;
	/*
	pthread_mutex_lock(&queutex);
	enqueue(curr);
	printf("Thread %d, enqueued %d\n", pthread_self()%919, curr->first);
	pthread_mutex_unlock(&queutex);
	curr = NULL;*/
      }
    } else {
      //printf("dequeue was null\n");
      return;
    }
  }
}

void find_buddy(Chunk *curr, size_t *arg) {
  
}

Chunk *dequeue(Chunk *prev) {
  Chunk *temp;
  if(!prev) {
    temp = queue;
    if(queue && !queue->done) {
      queue = queue->next;
    } else {
      while(temp && temp->done) {
	temp = temp->next;
      }
    }
  } else {
    temp = prev->next;
    prev->next = temp->next;
  }
  return temp;
}

void mt_merge(char *first, char *second, size_t fnum_elem, size_t snum_elem, size_t *arg) {
  char *pos, *fend, *send, *arr, *res;
  arr = first;
  fend = first + arg[1]*fnum_elem;
  send = second + arg[1]*snum_elem;
  pos = malloc(arg[1]*(fnum_elem+snum_elem));
  res = pos;
  while(first < fend && second < send) {
    if(func((void *) first, (void*) second) < 0 )  {
      memcpy(pos, first, arg[1]);
      pos += arg[1];
      first += arg[1];
    } else {
      memcpy(pos, second, arg[1]);
      pos += arg[1];
      second += arg[1];
    }
  }
  if(first < fend) {
    memcpy(pos, first, fend-first);
  } else {
    memcpy(pos, second, send-second);
  }
  /* printarray((int*)res, num_elem);*/
  memcpy(arr, res, (fnum_elem + snum_elem)*arg[1]);
  if(res) {
    free(res);
  } 
}
