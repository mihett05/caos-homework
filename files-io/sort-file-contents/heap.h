#ifndef HEAP_H
#define HEAP_H

#include "chunks.h"
#include "errors.h"

typedef struct {
  int chunk_index;
  int value;
} node_t;

typedef struct {
  node_t* array;
  size_t size;
  size_t capacity;
} heap_t;

heap_t* heap_new(size_t capacity);

void heap_heapify(heap_t* heap, int i);

void heap_build(heap_t* heap);

void heap_push(heap_t* heap, node_t value);

node_t heap_pop(heap_t* heap);

void heap_free(heap_t* heap);

#endif  // HEAP_H