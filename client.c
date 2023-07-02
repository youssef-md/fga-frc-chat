#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define BUFSIZE 1024

void send_recv(int i, int sockfd) {
	char send_buf[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;

	if (i == 0) {
		fgets(send_buf, BUFSIZE, stdin);
		send(sockfd, send_buf, strlen(send_buf), 0);
	} else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		printf("%s\n", recv_buf);

		fflush(stdout);
	}
}

void connect_req(int *sockfd, struct sockaddr_in *server_addr, char *client_name) {
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erro no socket()");
		exit(1);
	}

	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(8001);
	server_addr->sin_addr.s_addr = INADDR_ANY;
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);

	if (connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("Erro no connect()");
		exit(1);
	}

	send(*sockfd, client_name, strlen(client_name) + 1, 0);
}

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

	// Inicializa o conjunto de descritores de arquivo
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
