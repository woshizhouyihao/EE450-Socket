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
#define SERVER_UDP_PORT "32777"
#define SERVER_TCP_PORT "33777"
#define SERVER_A_UDP_PORT "30777"
#define SERVER_B_UDP_PORT "31777"
#define BACKLOG 10
#define MAXDATASIZE 100
#define FAIL -1

int client_TCP_sockfd, backend_server_UDP_sockfd;
int server_TCP_sockfd, server_UDP_sockfd;
struct addrinfo hints;
struct addrinfo *server_TCP_info, *client_TCP_Sock;
struct addrinfo *server_UDP_info, *backend_server_info;
struct addrinfo *server_A_info, *server_B_info;

struct sockaddr_in client_addr;


void init_TCP_Socket();

void init_UDP_Socket();

void init_TCP_Socket() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, NULL, &hints, &client_TCP_Sock);

	client_TCP_sockfd = socket(client_TCP_Sock->ai_family, client_TCP_Sock->ai_socktype, client_TCP_Sock->ai_protocol);
	if(client_TCP_sockfd == FAIL) {
		perror("ERROR: Cannot open client socket");
		close(client_TCP_sockfd);
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, SERVER_TCP_PORT, &hints, &server_TCP_info);

	if((bind(client_TCP_sockfd, server_TCP_info->ai_addr, server_TCP_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to bind TCP socket");
		close(client_TCP_sockfd);
		exit(1);
	}
}

void init_UDP_Socket() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_A_UDP_PORT, &hints, &server_A_info);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_B_UDP_PORT, &hints, &server_B_info);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, NULL, &hints, &backend_server_info);

	backend_server_UDP_sockfd = socket(backend_server_info->ai_family, backend_server_info->ai_socktype, backend_server_info->ai_protocol);
	if(backend_server_UDP_sockfd == FAIL) {
		perror("ERROR: Cannot open backend server socket");
		close(backend_server_UDP_sockfd);
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_UDP_PORT, &hints, &server_UDP_info);

	if((bind(backend_server_UDP_sockfd, server_UDP_info->ai_addr, server_UDP_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to bind UDP socket");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
}

int main() {
	init_UDP_Socket();
	printf("The Main server is up and running\n");

	
	string request = "ask for list";
	int len = request.length();
	char req[len];
	strcpy(req,request.c_str());
	if((sendto(backend_server_UDP_sockfd, req, len, 0, server_A_info->ai_addr, server_A_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to send data to backend server");
		close(backend_server_UDP_sockfd);
		exit(1);
	}

	char buf[MAXDATASIZE];
	struct addrinfo *backend_server_info;
	socklen_t addr_len = sizeof backend_server_info;

	recvfrom(backend_server_UDP_sockfd, buf, MAXDATASIZE, 0, backend_server_info->ai_addr, &addr_len);
	cout << buf << endl;
	printf("The Main server has received the country list from server A using UDP over port %s\n", SERVER_UDP_PORT);

	if((sendto(backend_server_UDP_sockfd, "", 0, 0, server_B_info->ai_addr, server_B_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to send data to backend server");
		close(backend_server_UDP_sockfd);
		exit(1);
	}

	recvfrom(backend_server_UDP_sockfd, buf, MAXDATASIZE, 0, backend_server_info->ai_addr, &addr_len);
	cout << buf << endl;
	printf("The Main server has received the country list from server B using UDP over port %s\n", SERVER_UDP_PORT);

	init_TCP_Socket();
	// /* List country list */

	// printf("The Main server has received the request on User%s in %s from the client using TCP over port %s\n", id, country, SERVER_TCP_PORT);

	// // if country not found
	// printf("%s does not show up in server A&B\n", country);
	// printf("The Main Server has sent \"Country Name: Not Found\" to the client using TCP over port %s\n", SERVER_TCP_PORT);

	// // if found
	// printf("%s shows up in server A/B\n", country);
	// printf("The Main Server has sent request from User%s to server A/B using UDP over port %s\n", id, SERVER_UDP_PORT);

	// // received reply from backend server and send to client
	// printf("The Main server has received searching result(s) of User%s from server%s\n", id, backend_server);
	// printf("The Main Server has sent seraching result(s) to client using TCP over port %s\n", SERVER_TCP_PORT);

	// // if no user id found
	// printf("The Main server has received \"User ID: Not Found\" from server %s\n", backend_server);
	// printf("The Main Server has sent error to client using TCP over %s\n", SERVER_TCP_PORT);

	// if((listen(TCP_Sock, BACKLOG)) == FAIL) {
	// 	perror("ERROR: Fail to listen from client");
	// 	close(TCP_Sock);
	// 	exit(1);
	// }

	// // fork();

	// socklen_t client_addr_size = sizeof(client_addr);
	// accept(TCP_Sock, (struct sockaddr *) &client_addr, &client_addr_size);

	return 0;
}