#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(EXIT_FAILURE);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

const char* template =
    "gcc -o tmp.out -x c - <<eof\n#include <stdio.h>\nvoid main() { "
    "printf(\"%%d\\n\", (%s)); }\neof";

size_t trim(char* s) {
  size_t length = strlen(s);
  char* new_str = s;
  while (isspace(new_str[length - 1])) {
    new_str[--length] = '\0';
  }
  while ('\0' != *new_str && isspace(*new_str)) {
    ++new_str;
    --length;
  }

  memmove(s, new_str, length + 1);
  return length;
}

int main(int argc, char const* argv[]) {
  char exp_buffer[1024];
  int read_amount = read(STDIN_FILENO, exp_buffer,
                         sizeof(exp_buffer) / sizeof(exp_buffer[0]));
  if (-1 == read_amount) {
    ERROR("Failed to read from stdin");
  }

  if (0 == trim(exp_buffer)) {
    return 0;
  }

  char template_buffer[4096];
  snprintf(template_buffer,
           sizeof(template_buffer) / sizeof(template_buffer[0]), template,
           exp_buffer);
  printf("%s\n", template_buffer);
  system(template_buffer);

  execl("./tmp.out", (char*)NULL);

  return 0;
}