#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void print_error(const char* msg, size_t size) {
  write(STDERR_FILENO, msg, size);
  exit(1);
}

#define ERROR(msg) print_error((msg), sizeof((msg)) - 1)

int main(int argc, char const* argv[]) {
  if (argc < 3) {
    ERROR("Invalid argument count: ./solution [ADDRESS] [PORT]");
  }

  uint16_t port = 0;
  if (EOF == sscanf(argv[2], "%hd", &port)) {
    ERROR("Failed to read port");
  }

  struct sockaddr_in addr = {.sin_family = AF_INET, .sin_port = htons(port)};
  if (0 == inet_pton(AF_INET, argv[1], &addr.sin_addr)) {
    ERROR("Failed to parse address");
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == sock) {
    ERROR("Failed to open socket connection");
  }

  if (-1 == connect(sock, (struct sockaddr*)&addr, sizeof(addr))) {
    close(sock);
    ERROR("Failed to connect to provided peer");
  }

  int value;
  int status = 0;
  while (EOF != scanf("%d", &value)) {
    status = send(sock, &value, sizeof(value), 0);
    if (-1 == status && ECONNRESET == errno || 0 == status) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      return 0;
    } else if (-1 == status) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      ERROR("Failed to send data");
    }
    status = recv(sock, &value, sizeof(value), 0);
    if (-1 == status && ECONNRESET == errno || 0 == status) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      return 0;
    } else if (-1 == status) {
      shutdown(sock, SHUT_RDWR);
      close(sock);
      ERROR("Failed to send data");
    }
    printf("%d ", value);
  }

  shutdown(sock, SHUT_RDWR);
  close(sock);

  return 0;
}
