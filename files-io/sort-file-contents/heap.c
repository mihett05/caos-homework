#include "heap.h"

heap_t* heap_new(size_t capacity) {
  heap_t* heap = (heap_t*)malloc(sizeof(heap_t));
  heap->capacity = capacity;
  heap->size = 0;
  heap->array = (node_t*)malloc(sizeof(node_t) * capacity);

  return heap;
}

void swap(node_t* a, node_t* b) {
  node_t tmp = *a;
  *a = *b;
  *b = tmp;
}

void heap_heapify(heap_t* heap, int i) {
  int min = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;

  if (left < heap->size && heap->array[left].value < heap->array[min].value) {
    min = left;
  }

  if (right < heap->size && heap->array[right].value < heap->array[min].value) {
    min = right;
  }

  if (min != i) {
    swap(&heap->array[i], &heap->array[min]);
    heap_heapify(heap, min);
  }
}

void heap_build(heap_t* heap) {
  for (int i = (heap->size - 1) / 2; i >= 0; --i) {
    heap_heapify(heap, i);
  }
}

void heap_push(heap_t* heap, node_t value) {
  if (heap->size == heap->capacity) {
    ERROR("Failed to push to heap: capacity overflow");
  }

  heap->array[heap->size++] = value;

  int i = heap->size - 1;
  while (0 != i && heap->array[(i - 1) / 2].value > heap->array[i].value) {
    swap(&heap->array[i], &heap->array[(i - 1) / 2]);
    i = (i - 1) / 2;
  }
}

node_t heap_pop(heap_t* heap) {
  if (0 == heap->size) {
    ERROR("Failed to pop from heap: heap is empty");
  } else if (1 == heap->size) {
    --heap->size;
    return heap->array[0];
  }

  node_t min = heap->array[0];
  heap->array[0] = heap->array[heap->size - 1];
  --heap->size;
  heap_heapify(heap, 0);

  return min;
}

void heap_free(heap_t* heap) {
  if (NULL != heap->array) {
    free(heap->array);
  }
}
