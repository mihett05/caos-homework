#define _XOPEN_SOURCE 500

#include <unistd.h>

#include "chunks.h"
#include "errors.h"
#include "merge.h"

// Обратите внимание на ограничение по памяти: размер файла может быть во много
// раз больше доступной памяти.

const size_t SET_CAPACITY = 4096;
const size_t CHUNK_CAPACITY = 32;  // буфер для чтения

int main(int argc, char const* argv[]) {
  if (argc < 2) {
    ERROR("Invalid amount of arguments");
  }

  chunks_set_t* chunks_set =
      chunks_set_new(argv[1], SET_CAPACITY, CHUNK_CAPACITY);

  chunks_set_read(chunks_set);

  k_way_merge(chunks_set);

  chunks_set_free(chunks_set);

  return 0;
}
