#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

typedef struct {
  int fd;
  int input_pipefd[2];
  int output_pipefd[2];
  char* content;
  size_t size;
} launcher_t;

int launcher_close(launcher_t* launcher) {
  int result = 0;
  int fds[] = {
      launcher->fd,
      launcher->input_pipefd[0],
      launcher->input_pipefd[1],
      launcher->output_pipefd[0],
      launcher->output_pipefd[1],
  };

  for (size_t i = 0; i < sizeof(fds) / sizeof(fds[0]); ++i) {
    if (-1 != fds[i]) {
      result = result || close(fds[i]);
    }
  }

  if (NULL != launcher->content) {
    result = result || munmap(launcher->content, launcher->size);
  }

  return result;
}

int main(int argc, char const* argv[]) {
  if (argc < 3) {
    ERROR("Invalid argument count: ./solution [CMD] [IN-FILE]");
  }

  launcher_t launch = {
      .fd = -1,
      .input_pipefd = {-1, -1},
      .output_pipefd = {-1, -1},
      .content = NULL,
      .size = 0,
  };

  if (-1 == pipe(launch.input_pipefd)) {
    launcher_close(&launch);
    ERROR("Failed to open pipe");
  }

  if (-1 == pipe(launch.output_pipefd)) {
    launcher_close(&launch);
    ERROR("Failed to open pipe");
  }

  pid_t pid = fork();
  if (-1 == pid) {
    launcher_close(&launch);
    ERROR("Failed to fork process");
  }

  if (0 == pid) {
    dup2(launch.input_pipefd[0], STDIN_FILENO);
    dup2(launch.output_pipefd[1], STDOUT_FILENO);
    launcher_close(&launch);
    execlp(argv[1], argv[1], (char*)NULL);
    ERROR("Failed to run CMD");
  } else {
    close(launch.output_pipefd[1]);
    close(launch.input_pipefd[0]);
    launch.fd = open(argv[2], O_RDONLY);
    if (-1 == launch.fd) {
      launcher_close(&launch);
      ERROR("Failed to open IN file");
    }

    struct stat sb;
    if (-1 == fstat(launch.fd, &sb)) {
      launcher_close(&launch);
      ERROR("Failed to get IN file stats");
    }

    if (0 == sb.st_size) {
      if (-1 == write(launch.input_pipefd[1], "", 0)) {
        launcher_close(&launch);
        ERROR("Failed to write IN file content to CMD");
      }
    } else {
      launch.size = sb.st_size;
      launch.content =
          (char*)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, launch.fd, 0);
      if ((void*)-1 == launch.content) {
        launcher_close(&launch);
        printf("%d\n", errno);
        ERROR("Failed to map file to memory");
      }

      if (-1 == write(launch.input_pipefd[1], launch.content, launch.size)) {
        launcher_close(&launch);
        ERROR("Failed to write IN file content to CMD");
      }
    }

    close(launch.input_pipefd[1]);

    char buffer[4096];
    int read_amount = 0;
    size_t counter = 0;
    while (0 < (read_amount = read(launch.output_pipefd[0], buffer,
                                   sizeof(buffer) / sizeof(buffer[0])))) {
      counter += read_amount;
    }
    printf("%ld\n", counter);
  }

  launcher_close(&launch);

  return 0;
}
