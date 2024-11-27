#ifndef FILTER_H
#define FILTER_H

#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void print_error(const char* msg, size_t size, int code);
#define ERROR(msg, code) print_error((msg), sizeof((msg)) - 1, code)

typedef struct {
  size_t buffer_size;
  int in_fd;
  int digits_fd;
  int other_fd;
  char* in_buffer;
  char* digits_buffer;
  char* other_buffer;
} filter_t;

filter_t* filter_new();

void filter_open(filter_t* filter, const char* in_path, const char* digits_path,
                 const char* other_path);

void filter_run(filter_t* filter);

void filter_close(filter_t* filter);

void close_pair(int fd, char* buffer);

#endif  // FILTER_H