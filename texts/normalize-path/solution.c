#include <memory.h>
#include <stdint.h>
#include <stdio.h>

void normalize_path(char* path) {
  size_t len = 0;
  if ('\0' == path[0]) {
    return;
  }

  while (path[len++] != '\0') {
  }

  size_t i = 1;
  while (path[i] != '\0') {
    if ('/' == path[i - 1] && '/' == path[i]) {
      memmove(path + i, path + i + 1, len - i - 1);
      len -= 1;
      --i;
    }
    if (i > 2 && 0 == memcmp(path + i - 2, "../", 3)) {
      if ('/' == path[i - 3] && 3 == i) {
        memmove(path + 1, path + 4, len - 3);
        len -= 3;
        i -= 3;
      } else if (i > 3) {
        int j;
        for (j = i - 4; j >= 0; --j) {
          if ('/' == path[j]) {
            break;
          }
        }
        if (j >= 0) {
          memmove(path + j + 1, path + i + 1, len - i - 1);
          len -= i - j;
          i -= i - j;
        }
      }
    }
    if ('.' == path[i - 1] && '/' == path[i]) {
      memmove(path + i - 1, path + i + 1, len - i - 1);
      len -= 2;
      i -= 2;
    }

    ++i;
  }
}