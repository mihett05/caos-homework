#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

int main(int argc, char const* argv[]) {
  if (argc < 3) {
    ERROR("Invlaid argument count: ./solution [CMD1] [CMD2]");
  }

  int pipefd[2];
  if (-1 == pipe(pipefd)) {
    ERROR("Failed to open input pipe");
  }

  pid_t pid = fork();
  if (-1 == pid) {
    ERROR("Failed to fork process");
  }

  if (0 == pid) {
    close(pipefd[0]);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);
    execlp(argv[1], argv[1], (char*)NULL);
    ERROR("Failed to run 1st command");
  } else {
    close(pipefd[1]);
    dup2(pipefd[0], STDIN_FILENO);
    close(pipefd[0]);
    execlp(argv[2], argv[2], (char*)NULL);
    ERROR("Failed to run 2nd command");
  }

  return 0;
}
