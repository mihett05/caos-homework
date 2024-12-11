#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
  char buffer[4096];
  size_t size = 0;
  while (EOF != scanf("%s", buffer)) {
    int fd = open(buffer, O_RDONLY);
    if (-1 == fd) {
      fprintf(stderr, "Failed to open file: %s : errno = %d", buffer, errno);
      continue;
    }

    struct stat sb;
    if (-1 == fstat(fd, &sb)) {
      fprintf(stderr, "Failed to get file stat: %s : errno = %d", buffer,
              errno);
      close(fd);
      continue;
    }

    if (S_ISREG(sb.st_mode)) {
      size += sb.st_size;
    }

    if (-1 == close(fd)) {
      fprintf(stderr, "Failed to close file: %s : errno = %d", buffer, errno);
    }
  }

  printf("%ld\n", size);

  return 0;
}
