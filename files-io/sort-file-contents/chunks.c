#include "chunks.h"

#include <errno.h>
#include <sys/types.h>

const size_t NAME_MAX_LENGTH = 256;

void chunks_set_free(chunks_set_t* chunks_set) {
  char name[NAME_MAX_LENGTH];
  for (size_t i = 0; i < chunks_set->set_size; ++i) {
    snprintf(name, NAME_MAX_LENGTH, "%ld.bin", i);
    remove(name);
    close(chunks_set->fd_set[i]);
  }

  if (NULL != chunks_set->fd_set) {
    free(chunks_set->fd_set);
  }
  if (NULL != chunks_set->sizes_set) {
    free(chunks_set->sizes_set);
  }
  if (NULL != chunks_set->active_chunk) {
    free(chunks_set->active_chunk);
  }
  if (NULL != chunks_set->offsets_set) {
    free(chunks_set->offsets_set);
  }
  free(chunks_set);

  if (chunks_set->source_fd >= 0) {
    close(chunks_set->source_fd);
  }
}

chunks_set_t* chunks_set_new(const char* file, size_t set_capacity,
                             size_t chunk_capacity) {
  chunks_set_t* set = (chunks_set_t*)malloc(sizeof(chunks_set_t));

  if (NULL == set) {
    chunks_set_free(set);
    ERROR("Failed to allocate memory for chunks_set_t");
  }

  memset(set, 0, sizeof(chunks_set_t));

  set->source_fd = open(file, O_RDWR);
  if (-1 == set->source_fd) {
    chunks_set_free(set);
    ERROR("Failed to open provided file");
  }

  set->set_capacity = set_capacity;
  set->chunk_capacity = chunk_capacity;
  set->set_size = 0;

  set->active_chunk = (int*)malloc(sizeof(int) * set->chunk_capacity);
  set->active_chunk_size = 0;

  if (NULL == set->active_chunk) {
    chunks_set_free(set);
    ERROR("Failed to allocate memory for chunk buffer");
  }

  set->fd_set = (int*)malloc(sizeof(int) * set_capacity);
  set->sizes_set = (int*)malloc(sizeof(int) * set_capacity);
  set->offsets_set = (int*)calloc(set_capacity, sizeof(int));

  if (NULL == set->fd_set || NULL == set->sizes_set) {
    chunks_set_free(set);
    ERROR("Failed to allocate memory for fd and sizes sets");
  }

  return set;
}

int compare(const void* a, const void* b) { return (*(int*)a - *(int*)b); }

void chunks_set_flush(chunks_set_t* chunks_set) {
  if (chunks_set->set_size + 1 >= chunks_set->set_capacity) {
    chunks_set_free(chunks_set);
    ERROR("Failed to flush chunk: not enough memory for chunks");
  }
  qsort(chunks_set->active_chunk, chunks_set->active_chunk_size, sizeof(int),
        compare);

  char name[NAME_MAX_LENGTH];

  int written_amount =
      snprintf(name, sizeof(name), "%ld.bin", chunks_set->set_size);

  if (written_amount < 0) {
    chunks_set_free(chunks_set);
    ERROR("Failed to flush chunk due to formatting error");
  }

  int chunk_fd = open(name, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (-1 == chunk_fd) {
    chunks_set_free(chunks_set);
    ERROR("Failed to create file for chunk");
  }

  chunks_set->fd_set[chunks_set->set_size] = chunk_fd;
  chunks_set->sizes_set[chunks_set->set_size] = chunks_set->active_chunk_size;
  ++chunks_set->set_size;

  written_amount = write(chunk_fd, chunks_set->active_chunk,
                         chunks_set->active_chunk_size * sizeof(int));
  if (-1 == written_amount) {
    chunks_set_free(chunks_set);
    ERROR("Failed to flush chunk to file");
  }

  chunks_set->active_chunk_size = 0;
}

void chunks_set_read(chunks_set_t* chunks_set) {
  while (1) {
    int read_amount =
        read(chunks_set->source_fd,
             chunks_set->active_chunk + chunks_set->active_chunk_size,
             (chunks_set->chunk_capacity - chunks_set->active_chunk_size) *
                 sizeof(int));
    if (-1 == read_amount) {
      chunks_set_free(chunks_set);
      ERROR("Failed to read from input file");
    }

    chunks_set->active_chunk_size += read_amount / sizeof(int);

    if (0 == read_amount ||
        chunks_set->active_chunk_size == chunks_set->chunk_capacity) {
      chunks_set_flush(chunks_set);
    }

    if (0 == read_amount) {
      break;
    }
  }
}

int chunks_set_get_int(chunks_set_t* chunks_set, int chunk_index) {
  if (chunk_index > chunks_set->set_size) {
    ERROR("Failed to get int from chunk: chunk index is invalid");
  }

  int buffer = 0;
  int read_amount =
      pread(chunks_set->fd_set[chunk_index], (void*)&buffer, sizeof(int),
            chunks_set->offsets_set[chunk_index] * sizeof(int));
  if (-1 == read_amount) {
    ERROR("Failed to get int from chunk: I/O error");
  }

  if (0 == read_amount) {
    ERROR("Failed to get int from chunk: end of file");
  }

  ++chunks_set->offsets_set[chunk_index];

  return buffer;
}