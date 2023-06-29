// STEP 1 : Create a socket file descriptor
// STEP 2 : Connect to the TCP server
// STEP 3 : recv/send for data transfer
// STEP 4 : Close the socket(fd)

#include <stdio.h>
#include <sys/socket.h> // Socket system
#include <netinet/in.h> // Kernet status
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SERVER_PORT 9000


int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Insert the IP and PORT\n");
    exit(1);
  }

  struct sockaddr_in server_addr;
  int client_fd = 0;
  int len_sockaddr = sizeof(struct sockaddr);
  char buffer[1024] = {0};
  strcpy(buffer, "Hello : SERVER");

  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));
  
  connect(client_fd, (struct sockaddr *) &server_addr, len_sockaddr);

  while(1) {
    int len_bytes = send(client_fd, buffer, sizeof(buffer), 0);
    printf("Sent [%d]: %s\n", len_bytes, buffer);
    sleep(3);
  }


  close(client_fd);

  return 0;
}