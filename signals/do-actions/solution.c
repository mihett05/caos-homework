#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t value = 0;

void signal_handler(int signo) {
  switch (signo) {
    case SIGUSR1:
      ++value;
      break;
    case SIGUSR2:
      value *= -1;
      break;
    case SIGINT:
    case SIGTERM:
      exit(0);
    default:
      break;
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    perror("Invalid argument count: ./solution [START-VALUE]");
    exit(EXIT_FAILURE);
  }

  if (EOF == sscanf(argv[1], "%d", &value)) {
    perror("Invalid start value");
    exit(EXIT_FAILURE);
  }

  printf("%d\n", getpid());
  fflush(NULL);

  struct sigaction sa_usr1 = {0};
  sa_usr1.sa_handler = signal_handler;
  if (-1 == sigaction(SIGUSR1, &sa_usr1, NULL)) {
    perror("Failed to set SIGUSR1 handler");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa_usr2 = {0};
  sa_usr2.sa_handler = signal_handler;
  if (-1 == sigaction(SIGUSR2, &sa_usr2, NULL)) {
    perror("Failed to set SIGUSR1 handler");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa_int = {0};
  sa_int.sa_handler = signal_handler;
  if (-1 == sigaction(SIGINT, &sa_int, NULL)) {
    perror("Failed to set SIGINT handler");
    exit(EXIT_FAILURE);
  }

  struct sigaction sa_term = {0};
  sa_term.sa_handler = signal_handler;
  if (-1 == sigaction(SIGTERM, &sa_term, NULL)) {
    perror("Failed to set SIGTERM handler");
    exit(EXIT_FAILURE);
  }

  sigset_t set;

  for (;;) {
    sigfillset(&set);
    sigdelset(&set, SIGUSR1);
    sigdelset(&set, SIGUSR2);
    sigdelset(&set, SIGINT);
    sigdelset(&set, SIGTERM);

    sigsuspend(&set);
    printf("%d ", value);
    fflush(NULL);
  }

  return 0;
}
