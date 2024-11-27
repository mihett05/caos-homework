#define STDIN 0
#define STDOUT 1
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_EXIT 60

long syscall(long number, ...);

long read(int fd, char* buffer, unsigned int buffer_size) {
  return syscall(SYS_READ, fd, buffer, buffer_size);
}

long write(int fd, char* buffer, unsigned int buffer_size) {
  return syscall(SYS_WRITE, fd, buffer, buffer_size);
}

void _exit(int status) { syscall(SYS_EXIT, status); }

void _start() {
  int read_amount = 0;
  const unsigned int buffer_size = 1024;
  char buffer[buffer_size];

  while ((read_amount = read(STDIN, buffer, buffer_size)) > 0) {
    if (-1 == write(STDOUT, buffer, read_amount)) {
      _exit(EXIT_FAILURE);
    }
  }

  if (-1 == read_amount) {
    _exit(EXIT_FAILURE);
  }

  _exit(EXIT_SUCCESS);
}