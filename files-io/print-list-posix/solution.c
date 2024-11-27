#define _XOPEN_SOURCE 500

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct Item {
  int value;
  uint32_t next_pointer;
};

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    perror("Invalid number of arguments\n");
    return 1;
  }
  int fd = open(argv[1], O_RDONLY);

  struct Item buffer;
  uint32_t offset = 0;

  while (1) {
    int read_amount = pread(fd, &buffer, sizeof(struct Item), offset);
    if (-1 == read_amount) {
      perror("Failed to read from file\n");
      close(fd);
      exit(1);
    } else if (0 == read_amount) {
      break;
    }

    printf("%d ", buffer.value);
    if (buffer.next_pointer == 0) {
      break;
    } else {
      offset = buffer.next_pointer;
    }
  }

  close(fd);

  return 0;
}
