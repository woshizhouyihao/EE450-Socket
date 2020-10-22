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
#include <sstream>

using namespace std;

#define LOCAL_HOST "127.0.0.1"
#define MAIN_SERVER_TCP_PORT "33777"
#define FAIL -1
#define MAXDATASIZE 1024

int TCP_sockfd;
struct addrinfo hints, *main_server_info;
char buf[MAXDATASIZE];
istringstream ss;

void init_client_TCP();

void init_mainserver_connection();

void sending_request(string, string);

void recv_server_reply();

void init_client_TCP() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, MAIN_SERVER_TCP_PORT, &hints, &main_server_info);

	TCP_sockfd = socket(main_server_info->ai_family, main_server_info->ai_socktype, main_server_info->ai_protocol);
	if(TCP_sockfd == FAIL) {
		perror("ERROR: Cannot open client socket");
		exit(1);
	}
}

void connect_to_mainserver() {
	if((connect(TCP_sockfd, main_server_info->ai_addr, main_server_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Cannot connect to main server");
		close(TCP_sockfd);
		exit(1);
	}
	printf("The client is up and running\n");
}

void sending_request(string s1, string s2) {
	string message = s1 + " " + s2;
	int len = message.size();
	char client_request[len];
	strcpy(client_request, message.c_str());
	if((send(TCP_sockfd, client_request, len, 0)) == FAIL) {
		perror("ERROR: Fail to send request");
		close(TCP_sockfd);
		exit(1);
	}
	printf("The client has sent User%s and %s to Main Server using TCP\n", s2.c_str(), s1.c_str());
}

void recv_server_reply() {
	// empty buffer
	memset(buf, 0, sizeof buf);
	if((recv(TCP_sockfd, buf, MAXDATASIZE, 0)) == FAIL) {
		perror("ERROR: Fail to receive reply from server");
		close(TCP_sockfd);
		exit(1);
	}
	ss.clear();
	ss.str(buf);
	string command, country, id;
	
	ss >> command;
	ss >> country;
	ss >> id;
	if(command == "0") {
		printf("%s not found\n", country.c_str());
	} else if(command == "1") {
		printf("User%s not found\n", id.c_str());
	} else {
		string user;
		string neighbor_users = "";
		while(ss >> user) {
			neighbor_users += "User" + user + ", ";
		}
		string temp = neighbor_users.substr(0, neighbor_users.length() - 2);
		printf("The client has results from Main Server: %s is/are possible friend(s) of User%s in %s\n", temp.c_str(), id.c_str(), country.c_str());
	}
}

int main() {
	init_client_TCP();
	connect_to_mainserver();
	string country_user;
	string id_user;
	while(true) {
		printf("Please enter the Country Name: ");
		getline(cin, country_user);
		printf("Please enter the User ID: ");
		getline(cin, id_user);
		// message going to send to the server
		sending_request(country_user, id_user);
		// receive reply from main server and print the result
		recv_server_reply();
		printf("-----Start a new request-----\n");
	}
	return 0;

	// // Retrieve the locally-bound name of the specified socket and store it in the sockaddr structure 
	// getsock_check=getsockname(TCP_sockfd, (struct sockaddr*)& my_addr, (socklen_t*)& addrlen);
	// //Error checking
	// if (getsock_check== -1) {
	// 	perror("getsockname"); exit(1);
	// }
	
}