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
#define MAIN_SERVER_TCP_PORT "33777"
#define FAIL -1
#define MAXDATASIZE 100

int TCP_Connect_Sock;
int status;
struct addrinfo hints, *res;

void init_client_TCP();

void init_mainserver_connection();

void sending_request(char*, char*);

void recv_server_reply(char*, char*);

void init_client_TCP() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, MAIN_SERVER_TCP_PORT, &hints, &res);

	TCP_Connect_Sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(TCP_Connect_Sock == FAIL) {
		perror("ERROR: Cannot open client socket");
		exit(1);
	}
}

void init_mainserver_connection() {
	if((connect(TCP_Connect_Sock, res->ai_addr, res->ai_addrlen)) == FAIL) {
		perror("ERROR: Cannot connect to server");
		close(TCP_Connect_Sock);
		exit(1);
	}
	printf("The client is up and running\n");
}

void sending_request(char *c1, char *c2) {
	string message = *c1 + " " + *c2;
	int len = message.size();
	char client_request[len];
	strcpy(client_request, message.c_str());
	if((send(TCP_Connect_Sock, client_request, len, 0)) == FAIL) {
		perror("ERROR: Fail to send request");
		close(TCP_Connect_Sock);
		exit(1);
	}
	printf("The client has sent User%s and %s to Main Server using TCP\n", (char*) c2, (char*) c1);
}

void recv_server_reply(char *c1, char *c2) {
	char reply[MAXDATASIZE];
	if((recv(TCP_Connect_Sock, reply, MAXDATASIZE, 0)) == FAIL) {
		perror("ERROR: Fail to receive reply from server");
		close(TCP_Connect_Sock);
		exit(1);
	}
	printf("%s not found\n", (char*) c1);
	printf("User%s not found\n", (char*) c2);

	printf("The client has received results from Main Server: User... is/are possible friend(s) of User%s in %s\n", (char*) c2, (char*) c1);
}

int main() {

	init_client_TCP();
	init_mainserver_connection();

	while(true) {
		string country_user;
		string id_user;
		printf("Please enter the Country Name: ");
		getline(cin, country_user);
		printf("Please enter the User ID: ");
		getline(cin, id_user);

		char country[country_user.size()];
		char id[id_user.size()];
		strcpy(country, country_user.c_str());
		strcpy(id, id_user.c_str());

		// message going to send to the server
		sending_request(country, id);
		
		recv_server_reply(country, id);
	}

	// // Retrieve the locally-bound name of the specified socket and store it in the sockaddr structure 
	// getsock_check=getsockname(TCP_Connect_Sock, (struct sockaddr*)& my_addr, (socklen_t*)& addrlen);
	// //Error checking
	// if (getsock_check== -1) {
	// 	perror("getsockname"); exit(1);
	// }
	return 0;
}