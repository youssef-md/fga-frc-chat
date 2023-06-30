#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {
	int sockfd, last_fd, i;
	struct sockaddr_in server_addr;
	fd_set master, read_fds;
	char client_name[BUFSIZE];

	if (argc != 2) {
		printf("Insira o nome do usuário como parametro!\n");
		exit(1);
	}

	strncpy(client_name, argv[1], BUFSIZE);
	connect_req(&sockfd, &server_addr, client_name);
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(0, &master);
	FD_SET(sockfd, &master);

	last_fd = sockfd;

	while (1) {
		read_fds = master;

		if (select(last_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Erro no select()");
			exit(1);
		}

		for (i = 0; i <= last_fd; i++) {
			if (FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
		}
	}

	close(sockfd);
	return 0;
}
