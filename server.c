// STEP 1 : Create a socket file descriptor
// STEP 2 : Bind the socket with port and IP
// STEP 3 : Listen for client request
// STEP 4 : Accept the TCP connection the client
// STEP 5 : recv/send for data transfer
// STEP 6 : Close the socket(fd)

#include <stdio.h>
#include <sys/socket.h> // Socket system
#include <netinet/in.h> // Kernet status
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT 9000
#define LISTEN_BACKLOG 5 // MAX CONCURRENT CLIENT CONNECTIONS


int main(int argc, char *argv[]) {

  if (argc < 3) {
    printf("Insert the IP and PORT\n");
    exit(1);
  }

  struct sockaddr_in server_addr;
  struct sockaddr_in client_addr;
  int server_fd = 0;
  int connected_fd = 0;
  int len_sockaddr = sizeof(struct sockaddr);
  char buffer[1024] = {0};

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  bind(server_fd, (struct sockaddr *) &server_addr, len_sockaddr);
  listen(server_fd, LISTEN_BACKLOG);

  connected_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len_sockaddr);


  while(1) {
    int len_bytes = recv(connected_fd, buffer, sizeof(buffer), 0);
    if(len_bytes <= 0) continue; // Skip empty message

    printf("Received [%d]: %s\n", len_bytes, buffer);
  }

  close(server_fd);
  close(connected_fd);

  return 0;
}