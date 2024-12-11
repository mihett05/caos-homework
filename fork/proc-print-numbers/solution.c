#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

void fork_print(int current, int N) {
  pid_t pid = fork();
  if (-1 == pid) {
    ERROR("Failed to fork process");
  }

  if (0 == pid) {
    if (current - 1 > 0) {
      fork_print(current - 1, N);
      exit(0);
    }
  } else {
    wait(NULL);
    printf("%d", current);
    printf(current == N ? "\n" : " ");
  }
}

int main(int argc, char const* argv[]) {
  if (argc < 2) {
    ERROR("Invalid argument count: ./solution [N]");
  }

  int N;
  sscanf(argv[1], "%d", &N);

  fork_print(N, N);
  return 0;
}
