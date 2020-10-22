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
#include <map>
#include <iomanip>

using namespace std;

#define LOCAL_HOST "127.0.0.1"
#define SERVER_UDP_PORT "32777"
#define SERVER_TCP_PORT "33777"
#define SERVER_A_UDP_PORT "30777"
#define SERVER_B_UDP_PORT "31777"
#define BACKLOG 10
#define MAXDATASIZE 1024
#define FAIL -1

int client_TCP_sockfd, backend_server_UDP_sockfd;
int child_TCP_sockfd;
struct addrinfo hints;
struct addrinfo *server_TCP_info, *client_TCP_info;
struct addrinfo *server_UDP_info, *backend_server_info;
struct addrinfo *server_A_info, *server_B_info;
socklen_t addr_len;
// struct sockaddr_in client_addr;
map<string, string> country_list;
char buf[MAXDATASIZE];
istringstream ss;

struct sockaddr_in child_TCP_info;

void init_TCP_Socket();

void init_UDP_Socket();

void communicate_server_A(string);

void communicate_server_B(string);

void send_to_client(string);

void recv_from_client();

void print_table();

void process_country_list();

void init_TCP_Socket() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(LOCAL_HOST, NULL, &hints, &client_TCP_info);

	client_TCP_sockfd = socket(client_TCP_info->ai_family, client_TCP_info->ai_socktype, client_TCP_info->ai_protocol);
	if(client_TCP_sockfd == FAIL) {
		perror("ERROR: Cannot open client socket");
		close(client_TCP_sockfd);
		exit(1);
	}
	getaddrinfo(LOCAL_HOST, SERVER_TCP_PORT, &hints, &server_TCP_info);

	if((bind(client_TCP_sockfd, server_TCP_info->ai_addr, server_TCP_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to bind TCP socket");
		close(client_TCP_sockfd);
		exit(1);
	}

	if((listen(client_TCP_sockfd, BACKLOG)) == FAIL) {
		perror("ERROR: Fail to listen from client");
		close(client_TCP_sockfd);
		exit(1);
	}
}

void init_UDP_Socket() {
	//fill out info for server A
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_A_UDP_PORT, &hints, &server_A_info);
	//fill out info for server B
	getaddrinfo(LOCAL_HOST, SERVER_B_UDP_PORT, &hints, &server_B_info);
	// fill out info for backend server
	getaddrinfo(LOCAL_HOST, NULL, &hints, &backend_server_info);
	// initialize backend UDP socket
	backend_server_UDP_sockfd = socket(backend_server_info->ai_family, backend_server_info->ai_socktype, backend_server_info->ai_protocol);
	// if fail to initialize, print error and exit
	if(backend_server_UDP_sockfd == FAIL) {
		perror("ERROR: Cannot open backend server socket");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
	// fill out main server UDP info for bind socket to the port 32777
	getaddrinfo(LOCAL_HOST, SERVER_UDP_PORT, &hints, &server_UDP_info);
	// bind the socket to the port 32777 and if fail, print error and exit 
	if((bind(backend_server_UDP_sockfd, server_UDP_info->ai_addr, server_UDP_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to bind UDP socket");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
}

void communicate_server_A(string s) {
	int len = s.length();
	char req[len];
	strcpy(req,s.c_str());
	addr_len = sizeof server_A_info;
	// to server A
	if((sendto(backend_server_UDP_sockfd, req, len, 0, server_A_info->ai_addr, server_A_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to send data to backend server");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
	// empty buffer
	memset(buf, 0, sizeof buf);
	if((recvfrom(backend_server_UDP_sockfd, buf, MAXDATASIZE, 0, server_A_info->ai_addr, &addr_len)) == FAIL) {
		perror("ERROR: Fail to receive reply from server A");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
}

void communicate_server_B(string s) {
	int len = s.length();
	char req[len];
	strcpy(req,s.c_str());
	addr_len = sizeof server_B_info;
	if((sendto(backend_server_UDP_sockfd, req, len, 0, server_B_info->ai_addr, server_B_info->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to send data to backend server");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
	// empty buffer
	memset(buf, 0, sizeof buf);
	if((recvfrom(backend_server_UDP_sockfd, buf, MAXDATASIZE, 0, server_B_info->ai_addr, &addr_len)) == FAIL) {
		perror("ERROR: Fail to receive reply from server B");
		close(backend_server_UDP_sockfd);
		exit(1);
	}
}

void send_to_client(string s) {
	char reply[s.length()];
	strcpy(reply, s.c_str());
	if(send(child_TCP_sockfd, reply, s.length(), 0) == FAIL) {
		perror("ERROR: Fail to send reply to client");
		close(child_TCP_sockfd);
		exit(1);
	}
}

void recv_from_client() {
	// empty buffer
	memset(buf, 0, sizeof buf);
	if((recv(child_TCP_sockfd, buf, MAXDATASIZE, 0)) == FAIL) {
		perror("ERROR: Fail to receive request from client");
		close(child_TCP_sockfd);
		exit(1);
	}
}

void print_table(){
	string t1 = "";
	string t2 = "";
	for(map<string,string>::iterator it=country_list.begin(); it != country_list.end(); it++) {
		if(it->second == "0")
			t1 += it->first + " ";
		else
			t2 += it->first + " ";
	}

	istringstream s1(t1);
	istringstream s2(t2);
	cout << left << setfill(' ') << setw(20) << "Server A" << "|Server B" << endl;
	string w1, w2;
	while((!s1.eof() || !s2.eof())) {
		s1 >> w1;
		s2 >> w2;
		if(s1.eof())
			w1 = " ";
		if(s2.eof())
			w2 = " ";
		if(w1 != " " || w2 != " ")
			cout << left << setfill(' ') << setw(20) << w1 << "|" << w2 << endl;
	}
}

void process_country_list() {
	string backend_server_reply = buf;
	ss.clear();
	ss.str(backend_server_reply);
	string server;
	string word;
	ss >> server;
	while(ss >> word) {
		country_list.insert(pair<string, string>(word, server));
	}
}

int main() {
	// initialize UDP and TCP socket and print message
	init_UDP_Socket();
	init_TCP_Socket();
	printf("The Main server is up and running\n");

	// request to backend server for country list
	string request = "ask for list";
	communicate_server_A(request);	
	printf("The Main server has received the country list from server A using UDP over port %s\n", SERVER_UDP_PORT);

	process_country_list();
	// to server B
	communicate_server_B(request);
	printf("The Main server has received the country list from server B using UDP over port %s\n", SERVER_UDP_PORT);
	process_country_list();
	/* finish receiving country list 
		 List country list */
	print_table();

	// TCP child process
	while(true) {
		addr_len = sizeof child_TCP_info;
		if((child_TCP_sockfd = accept(client_TCP_sockfd, (struct sockaddr *) &child_TCP_info, &addr_len)) == FAIL) {
			perror("ERROR: Fail to accept TCP connection");
			close(client_TCP_sockfd);
			exit(1);
		}
		if(!fork()){
			while(true) {
				recv_from_client();	
				ss.clear();
				ss.str(buf);
				string country, id;
				ss >> country;
				ss >> id;
				printf("The Main server has received the request on User%s in %s from the client using TCP over port %s\n", id.c_str(), country.c_str(), SERVER_TCP_PORT);
				map<string, string>::iterator it = country_list.find(country);
				if(it != country_list.end()) { // if country found in country list
					printf("%s shows up in server A/B\n", country.c_str());

					string server = it->second;
					if(server == "0") {
						communicate_server_A(country + " " + id);
					} else {
						communicate_server_B(country + " " + id);
					}
					printf("The Main Server has sent request from User%s to server A/B using UDP over port %s\n", id.c_str(), SERVER_UDP_PORT);
					ss.clear();
					ss.str(buf);
					string command, backend_server;
					ss >> command;
					ss >> backend_server;
					if(command == "0") { // if no user id found
						printf("The Main server has received \"User ID: Not Found\" from server %s\n", backend_server.c_str());
						send_to_client("1 " + country + " " + id);
						printf("The Main Server has sent error to client using TCP over %s\n", SERVER_TCP_PORT);
					} else {
						string neighbor_users;
						string users = "";
						while(ss >> neighbor_users) {
							users += neighbor_users + " ";
						}
						// received reply from backend server and send to client
						printf("The Main server has received searching result(s) of User%s from server%s\n", id.c_str(), backend_server.c_str());
						send_to_client("2 "  + country + " " + id + " " + users);
						printf("The Main Server has sent seraching result(s) to client using TCP over port %s\n", SERVER_TCP_PORT);
					}

				} else { // if country not found in country list
					printf("%s does not show up in server A&B\n", country.c_str());
					send_to_client("0 " + country + " " + id);
					printf("The Main Server has sent \"Country Name: Not Found\" to the client using TCP over port %s\n", SERVER_TCP_PORT);
				}
			}
		}
	}
	
	return 0;
}