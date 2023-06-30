#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

#define BUFSIZE 1024



void send_recv(int i, fd_set* master, int socket_fd, int last_fd, char* recv_buf, char* client_names[]) {
	int nbytes_recvd, j;

	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0)
			printf("O socket %d foi desconectado\n", i);
		else
			perror("Erro no recv()");
		
		close(i);
		FD_CLR(i, master);
		free(client_names[i]);
		client_names[i] = NULL;
	} else {
		if (client_names[i] == NULL) {
			client_names[i] = malloc(nbytes_recvd + 1);
			strncpy(client_names[i], recv_buf, nbytes_recvd);
			client_names[i][nbytes_recvd] = '\0';
			printf("Socket %d foi conectado com o nome: %s\n", i, client_names[i]);
		} else {
			for (j = 0; j <= last_fd; j++)
				send_to_all(j, i, socket_fd, nbytes_recvd, recv_buf, master, client_names);
		}
	}
}

void connection_accept(fd_set* master, int* last_fd, int socket_fd, struct sockaddr_in* client_addr, char* client_names[]) {
	socklen_t addrlen = sizeof(struct sockaddr_in);
	int new_fd;

	if ((new_fd = accept(socket_fd, (struct sockaddr*)client_addr, &addrlen)) == -1) {
		perror("Erro no accept()");
		exit(1);
	} else {
		FD_SET(new_fd, master);

		if(new_fd > *last_fd)
			*last_fd = new_fd;

		fflush(stdout);
		client_names[new_fd] = NULL;
	}
}

void connect_request(int* socket_fd, struct sockaddr_in* my_addr) {
	int yes = 1;

	if ((*socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Erro no socket()");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(8001);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof(my_addr->sin_zero));

	if (bind(*socket_fd, (struct sockaddr*)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Erro no bind()");
		exit(1);
	}

	if (listen(*socket_fd, 10) == -1) {
		perror("Erro no listen()");
		exit(1);
	}

	printf("O servidor TCP está ouvindo por novas conexões...\n");
	fflush(stdout);
}

int main() {
	struct sockaddr_in my_addr, client_addr;
	int socket_fd = 0;
	fd_set master, read_fds;
	int last_fd, i;
	char recv_buf[BUFSIZE];
	char* client_names[FD_SETSIZE];

	for (i = 0; i < FD_SETSIZE; i++)
		client_names[i] = NULL;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&socket_fd, &my_addr);
	FD_SET(socket_fd, &master);

	last_fd = socket_fd;

	while (1) {
		read_fds = master;

		if (select(last_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Erro no select()");
			exit(4);
		}

		for (i = 0; i <= last_fd; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == socket_fd)
					connection_accept(&master, &last_fd, socket_fd, &client_addr, client_names);
				else
					send_recv(i, &master, socket_fd, last_fd, recv_buf, client_names);
			}
		}
	}

	return 0;
}
