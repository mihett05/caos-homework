#ifndef CHUNKS_H
#define CHUNKS_H

#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>  // только ради sprintf
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "errors.h"

// Чанк с int'ами
typedef struct {
  int source_fd;

  int* fd_set;
  int* sizes_set;
  int* offsets_set;

  size_t set_size;
  size_t set_capacity;
  size_t chunk_capacity;

  int* active_chunk;
  size_t active_chunk_size;
} chunks_set_t;

void chunks_set_free(chunks_set_t* chunks_set);

chunks_set_t* chunks_set_new(const char* file, size_t set_capacity,
                             size_t chunk_capacity);

void chunks_set_flush(chunks_set_t* chunks_set);

void chunks_set_read(chunks_set_t* chunks_set);

int chunks_set_get_int(chunks_set_t* chunks_set, int chunk_index);

#endif  // CHUNKS_H