#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

int main(int argc, char const* argv[]) {
  int count = 0;
  char buffer[4096];
  int result = 0;

  while (1) {
    pid_t pid = fork();
    if (-1 == pid) {
      ERROR("Failed to create fork");
    }
    if (0 == pid) {
      if (1 == scanf("%s", buffer)) {
        return 0;
      } else {
        return 1;
      }
    } else {
      wait(&result);
      if (0 == result) {
        ++count;
      } else {
        break;
      }
    }
  }

  printf("%d\n", count);

  return 0;
}
