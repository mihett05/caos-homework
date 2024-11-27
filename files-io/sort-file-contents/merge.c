#include "merge.h"

int flush_merge_buffer_to_file(chunks_set_t* chunks_set, int write_offset) {
  int written_amount =
      pwrite(chunks_set->source_fd, chunks_set->active_chunk,
             chunks_set->active_chunk_size * sizeof(int), write_offset);
  if (-1 == written_amount) {
    ERROR("Failed to write in file: I/O error");
  }
  chunks_set->active_chunk_size = 0;
  return written_amount;
}

void k_way_merge(chunks_set_t* chunks_set) {
  heap_t* heap = heap_new(chunks_set->set_size);
  for (size_t i = 0; i < chunks_set->set_size; ++i) {
    if (chunks_set->offsets_set[i] < chunks_set->sizes_set[i]) {
      node_t buffer = {
          .chunk_index = i,
          .value = chunks_set_get_int(chunks_set, i),
      };
      heap->array[i] = buffer;
      ++heap->size;
    }
  }
  heap_build(heap);

  node_t node;
  int write_offset = 0;
  chunks_set->active_chunk_size = 0;

  while (heap->size > 0) {
    if (chunks_set->active_chunk_size == chunks_set->chunk_capacity) {
      write_offset += flush_merge_buffer_to_file(chunks_set, write_offset);
    }
    node = heap_pop(heap);
    chunks_set->active_chunk[chunks_set->active_chunk_size++] = node.value;
    if (chunks_set->offsets_set[node.chunk_index] <
        chunks_set->sizes_set[node.chunk_index]) {
      node.value = chunks_set_get_int(chunks_set, node.chunk_index);
      heap_push(heap, node);
    }
  }

  if (chunks_set->active_chunk_size > 0) {
    write_offset += flush_merge_buffer_to_file(chunks_set, write_offset);
  }

  heap_free(heap);
}