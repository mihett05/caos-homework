#include "errors.h"

void print_error_and_exit(const char* msg, size_t msg_length) {
  write(STDERR_FILENO, msg, msg_length);
  exit(EXIT_FAILURE);
}
