#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Семантика повединия сигналов (Sys-V или BSD) считается не определенной.
// тогда sigacton вместо signal

volatile sig_atomic_t counter = 0;
volatile sig_atomic_t keep_running = 1;

void sig_handler(int signo) {
  if (SIGINT == signo) {
    ++counter;
  } else if (SIGTERM == signo) {
    keep_running = 0;
  }
}

int main(int argc, char const* argv[]) {
  pid_t pid = getpid();
  printf("%d\n", pid);
  fflush(NULL);

  struct sigaction sa_int = {0};

  sa_int.sa_flags = SA_RESTART;
  sa_int.sa_handler = sig_handler;
  if (-1 == sigaction(SIGINT, &sa_int, NULL)) {
    perror("Failed to set signal hander");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa_term = {0};
  sa_term.sa_handler = sig_handler;
  if (-1 == sigaction(SIGTERM, &sa_term, NULL)) {
    perror("Failed to set signal hander");
    exit(EXIT_FAILURE);
  }

  while (keep_running) {
    pause();
  }

  printf("%d\n", counter);

  return 0;
}
