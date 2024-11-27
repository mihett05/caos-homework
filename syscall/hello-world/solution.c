#define STDOUT 1
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define SYS_WRITE 1
#define SYS_EXIT 60

long syscall(long number, ...);

const char message[] = "Hello, World!\n";
const unsigned int message_size = sizeof(message) / sizeof(message[0]);

void _start() {
  if (syscall(SYS_WRITE, STDOUT, message, message_size - 1) == -1) {
    syscall(SYS_EXIT, EXIT_FAILURE);
  } else {
    syscall(SYS_EXIT, EXIT_SUCCESS);
  }
}