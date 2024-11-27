#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "filter.h"

int main(int argc, char const* argv[]) {
  if (argc < 4) {
    ERROR("Invalid amount of arguments\n", 3);
  }

  filter_t* filter = filter_new();

  filter_open(filter, argv[1], argv[2], argv[3]);
  filter_run(filter);
  filter_close(filter);

  return 0;
}
