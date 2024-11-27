#include <sys/file.h>
#include <unistd.h>

int main() {
  // char buffer[1024];
  // int read_amount = 0;
  // while ((read_amount = read(STDIN_FILENO, buffer, 1024)) > 0) {
  //   for (size_t i = 0; i < read_amount; ++i) {
  //   }
  // }

  // if (-1 == pwrite(STDOUT_FILENO, "test\n", 5, 0)) {
  //   write(STDOUT_FILENO, "error\n", 6);
  // }

  // int buffer_fd = open("buffer.tmp", O_CREAT | O_RDWR);

  // close(buffer_fd);

  char* const* argv = {"tac", (char* const*)NULL};
  char* const* envp = {(char* const*)NULL};

  execve("/usr/bin/tac", argv, envp);

  return 0;
}
