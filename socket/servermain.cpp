#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

#define LOCAL_HOST "127.0.0.1"
#define BACKEND_SERVER_UDP_PORT "32777"
#define CLIENT_TCP_PORT "33777"
#define SERVERA_UDP_PORT "30777"
#define SERVERB_UDP_PORT "31777"
#define BACKLOG 10
#define MAXDATASIZE 100
#define FAIL -1

int TCP_Sock, UDP_Sock;
int status;
struct addrinfo hints, *res;

struct sockaddr_in client_addr;


void init_TCP_Socket();

void init_UDP_Socket();

void init_TCP_Socket() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, CLIENT_TCP_PORT, &hints, &res);

	TCP_Sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(TCP_Sock == FAIL) {
		perror("ERROR: Cannot open client socket");
		exit(1);
	}
}

int main() {
	 init_TCP_Socket();
	printf("The Main server is up and running\n");
	status = bind(TCP_Sock, res->ai_addr, res->ai_addrlen);
	if(status == FAIL) {
		perror("ERROR: Fail to bind TCO socket");
		exit(1);
	}

	status = listen(TCP_Sock, BACKLOG);
	cout << 1;
	if(status == FAIL) {
		perror("ERROR: Fail to listen from client");
		exit(1);
	}

	// fork();

	socklen_t client_addr_size = sizeof(client_addr);
	status = accept(TCP_Sock, (struct sockaddr *) &client_addr, &client_addr_size);
	cout << status;

	return 0;
}