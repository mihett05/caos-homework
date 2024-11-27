#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_error_and_exit(const char* msg, size_t msg_length);

#define ERROR(msg) print_error_and_exit((msg), sizeof((msg)) - 1)

#endif  // ERRORS_H