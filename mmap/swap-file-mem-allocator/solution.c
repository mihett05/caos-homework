#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct Node {
  struct Node* prev;
  struct Node* next;
  char used;
} node_t;

enum MyAllocErrorsEnum {
  ERROR_UNINITIALIZED = 1,
  ERROR_INIT_FAILED,
  ERROR_FINAL_FAILED,
};

int myalloc_errno;

struct Allocator {
  char* memory;  // первый доступный байт
  node_t* start;
  node_t* end;
  size_t size;
};

struct Allocator allocator;

size_t get_node_size(node_t* node) {
  if (NULL != node->next) {
    return (size_t)node->next - (size_t)node - sizeof(node_t);
  }
  return (size_t)allocator.memory + allocator.size - (size_t)node -
         sizeof(node_t);
}

node_t* split(node_t* node, size_t next_block_size) {
  if (node->used || get_node_size(node) < next_block_size) {
    return NULL;
  }
  if (get_node_size(node) == next_block_size) {
    return node;
  }

  if (NULL == node->next) {
    node_t* new_node =
        allocator.memory + allocator.size - sizeof(node_t) - next_block_size;
    new_node->prev = node;
    new_node->next = node->next;
    new_node->used = 0;
    node->next = new_node;

    return new_node;
  }

  node_t* new_node = node->next - sizeof(node_t) - next_block_size;
  new_node->prev = node;
  new_node->next = node->next;
  new_node->used = 0;
  node->next->prev = new_node;
  node->next = new_node;

  return new_node;
}

node_t* merge_right(node_t* left) {
  if (NULL == left || left->used || NULL == left->next || left->next->used) {
    return NULL;
  }

  if (NULL == left->next->next) {
    left->next = NULL;
    return left;
  }

  left->next->next->prev = left;
  left->next = left->next->next;

  return left;
}

node_t* merge_free(node_t* node) {
  if (NULL == node) {
    return NULL;
  }
  if (node->used) {
    return NULL;
  }

  node_t* new_node = node;
  if (NULL != node->prev) {
    while (NULL != (new_node = merge_right(new_node->prev))->prev) {
    }
  }

  if (NULL != new_node->next) {
    while (NULL != (new_node = merge_right(new_node))->next) {
    }
  }

  return new_node;
}

void myalloc_initialize(int fd) {
  memset(&allocator, 0, sizeof(allocator));

  struct stat sb;
  if (-1 == fstat(fd, &sb)) {
    myalloc_errno = ERROR_INIT_FAILED;
    return;
  }

  allocator.memory =
      (char*)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (NULL == allocator.memory) {
    myalloc_errno == ERROR_INIT_FAILED;
    return;
  }
  allocator.size = sb.st_size;
  allocator.start = (node_t*)allocator.memory;
  allocator.end = (node_t*)allocator.memory;

  memset(allocator.start, 0, sizeof(node_t));
}

void myalloc_finalize() {
  if (-1 == munmap(allocator.memory, allocator.size)) {
    myalloc_errno = ERROR_FINAL_FAILED;
  }
}

void* my_malloc(size_t size) {
  node_t* runner = allocator.start;
  node_t* best = runner;
  do {
    if (runner->used || get_node_size(runner) < size) {
      continue;
    }
    if (get_node_size(runner) < get_node_size(best)) {
      best = runner;
    }
  } while (NULL != (runner = runner->next));

  if (best->used) {
    return NULL;
  }

  if (get_node_size(best) > size) {
    node_t* new_node = split(best, size);
    new_node->used = 1;
    return (size_t)new_node + sizeof(node_t);
  } else if (get_node_size(best) == size) {
    best->used = 1;
    return (size_t)best + sizeof(node_t);
  }

  return NULL;
}

void my_free(void* ptr) {
  if (NULL == ptr || ptr < allocator.memory ||
      ptr >= allocator.memory + allocator.size) {
    return;
  }

  node_t* node = (node_t*)(ptr - sizeof(node_t));
  if (NULL != node->prev) {
    if (node->prev < allocator.memory ||
        node->prev >= allocator.memory + allocator.size) {
      return;
    }
  }
  if (NULL != node->next) {
    if (node->next < allocator.memory ||
        node->next >= allocator.memory + allocator.size) {
      return;
    }
  }

  // я очень надеюсь, что мне не закинут рандомный адрес памяти сюда
  // потому что иначе нужно по всему списку пробегаться

  if (!node->used) {
    return;
  }

  if (node->used) {
    node->used = 0;
    merge_free(node);
    node_t* runner = allocator.start;
  }
}