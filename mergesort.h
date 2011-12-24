#ifndef MERGESORT_H
#define MERGESORT_H

typedef int (*Compare_fn)(const void *, const void *);

typedef struct Chunk {
    struct Chunk	*next;
    void		*arr;		/* address of first */
    int			first;
    int			size;
    int			done;
} Chunk;


void printarray(int *arr, int num_elem);
void mergesort(void *arr, size_t num_elem, size_t elem_size, Compare_fn cmp);
void merge(char *first, char *second, size_t num_elem, size_t elem_size, Compare_fn cmp);
void mt_merge(char *first, char *second, size_t fnum_elem, size_t snum_elem, size_t *arg);
void enqueue(Chunk *item);
void worker_routine(size_t *arg);
Chunk *dequeue(Chunk *prev);
void find_buddy(Chunk *curr, size_t *arg);
void mergesort_h(void *start, size_t num_elem, size_t elem_size, Compare_fn cmp);
void mt_mergesort(void *arr, size_t num_elem, int chunk_size, size_t elem_size, Compare_fn cmp,
                  int num_threads);

#endif
