#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

int main(int argc, char const* argv[]) {
  if (argc < 3) {
    ERROR("Invalid argument count:\n./solution [FILENAME] [SUBSTRING]");
  }

  int fd = open(argv[1], O_RDONLY);
  if (-1 == fd) {
    ERROR("Failed to open provided file");
  }

  struct stat sb;

  if (-1 == fstat(fd, &sb)) {
    close(fd);
    ERROR("Failed to get stats of provided file");
  }

  char* content = (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (NULL == content) {
    close(fd);
    ERROR("Failed to map file to memory");
  }

  const char* needle = argv[2];
  size_t needle_len = strlen(needle);
  if (needle_len <= sb.st_size) {
    char* haystack = content;
    // strstr не работает с не cstring (как в 5 тесте)
    for (size_t i = 0; i <= sb.st_size - needle_len; ++i) {
      if (0 == memcmp(haystack, needle, needle_len)) {
        printf("%ld ", i);
      }
      ++haystack;
    }
  }

  if (-1 == munmap(content, sb.st_size)) {
    close(fd);
    ERROR("Failed to unmap file");
  }
  if (-1 == close(fd)) {
    ERROR("Failed to close file");
  }

  return 0;
}
