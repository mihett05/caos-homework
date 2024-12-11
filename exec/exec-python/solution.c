#include <unistd.h>

int main(int argc, char const *argv[]) {
  execlp("python3", "python3", "-c", "print(eval(input()))", (char *)NULL);
  return 0;
}
