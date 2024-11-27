#include "filter.h"

#include <stdio.h>

void print_error(const char* msg, size_t size, int code) {
  write(STDERR_FILENO, msg, size);
  exit(code);
}

filter_t* filter_new() {
  filter_t* filter = (filter_t*)malloc(sizeof(filter_t));
  memset(filter, 0, sizeof(filter_t));

  filter->in_fd = -1;
  filter->digits_fd = -1;
  filter->other_fd = -1;

  filter->buffer_size = 1024;

  return filter;
}

void filter_open(filter_t* filter, const char* in_path, const char* digits_path,
                 const char* other_path) {
  filter->in_fd = open(in_path, O_RDONLY);
  if (-1 == filter->in_fd) {
    if (ENOENT == errno) {
      filter_close(filter);
      ERROR("Input file doesn't exist\n", 1);
    }
    filter_close(filter);
    ERROR("Can't open input file\n", 3);
  }

  filter->digits_fd = open(digits_path, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (-1 == filter->digits_fd) {
    filter_close(filter);
    ERROR("Can't create digits file\n", 2);
  }

  filter->other_fd = open(other_path, O_CREAT | O_TRUNC | O_RDWR, 0666);
  if (-1 == filter->other_fd) {
    filter_close(filter);
    ERROR("Can't create other file\n", 2);
  }

  filter->in_buffer = (char*)calloc(filter->buffer_size, sizeof(char));
  filter->digits_buffer = (char*)calloc(filter->buffer_size, sizeof(char));
  filter->other_buffer = (char*)calloc(filter->buffer_size, sizeof(char));

  if (NULL == filter->in_buffer || NULL == filter->digits_buffer ||
      NULL == filter->other_buffer) {
    filter_close(filter);
    ERROR("Can't allocate buffers\n", 3);
  }
}

void filter_run(filter_t* filter) {
  int read_amount = 0;
  int digits_written = 0;
  int other_written = 0;

  while (1) {
    read_amount = read(filter->in_fd, filter->in_buffer, filter->buffer_size);
    digits_written = 0;
    other_written = 0;

    if (-1 == read_amount) {
      filter_close(filter);
      ERROR("Failed to read from in file\n", 3);
    } else if (0 == read_amount) {
      break;
    }

    for (int i = 0; i < read_amount; ++i) {
      if (filter->in_buffer[i] >= '0' && filter->in_buffer[i] <= '9') {
        filter->digits_buffer[digits_written++] = filter->in_buffer[i];
      } else {
        filter->other_buffer[other_written++] = filter->in_buffer[i];
      }
    }

    if (digits_written > 0) {
      if (-1 ==
          write(filter->digits_fd, filter->digits_buffer, digits_written)) {
        filter_close(filter);
        ERROR("Failed to write to digits file\n", 3);
      }
    }
    if (other_written > 0) {
      if (-1 == write(filter->other_fd, filter->other_buffer, other_written)) {
        filter_close(filter);
        ERROR("Failed to write to other file\n", 3);
      }
    }
  }
}

void filter_close(filter_t* filter) {
  close_pair(filter->in_fd, filter->in_buffer);
  close_pair(filter->digits_fd, filter->digits_buffer);
  close_pair(filter->other_fd, filter->other_buffer);
}

void close_pair(int fd, char* buffer) {
  if (buffer != NULL) {
    free(buffer);
  }
  if (fd > -1) {
    close(fd);
  }
}