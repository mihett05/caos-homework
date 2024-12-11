#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(EXIT_FAILURE);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

struct Item {
  int value;
  uint32_t next_pointer;
};

int main(int argc, char const* argv[]) {
  if (argc < 2) {
    ERROR("Invalid arguments amount: ./solution [FILENAME]");
  }

  int fd = open(argv[1], O_RDONLY);
  if (-1 == fd) {
    ERROR("Failed to open provided file");
  }

  struct stat sb;
  if (-1 == fstat(fd, &sb)) {
    close(fd);
    ERROR("Failed to get file stats");
  }

  if (sb.st_size == 0) {
    close(fd);
    return 0;
  }

  void* base = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (NULL == base) {
    close(fd);
    ERROR("Failed to map file to memory");
  }

  struct Item* item = (struct Item*)base;
  while (NULL != item) {
    printf("%d ", item->value);
    if (0 == item->next_pointer) {
      item = NULL;
    } else {
      item = (struct Item*)(base + item->next_pointer);
    }
  }

  munmap(base, sb.st_size);
  close(fd);

  return 0;
}
