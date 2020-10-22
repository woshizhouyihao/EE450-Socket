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
#include <fstream>
#include <map>
#include <set>
#include <sstream>

using namespace std;

#define LOCAL_HOST "127.0.0.1"
#define SERVER_A_UDP_PORT "30777"
#define MAIN_SERVER_UDP_PORT "32777"
#define FAIL -1
#define MAXDATASIZE 1024

int UDP_Connect_Sock;
struct addrinfo hints, *main_server_info, *server_A_info;
socklen_t addr_len;
map<string, map<string, set<string> > > user_list_by_country;
string country_data;
char buf[MAXDATASIZE];

bool is_init(string);

void read_data();

void init_UDP_Socket();

void send_data(string);

void recv_request();

bool is_int(string s) {
	return s.find_first_not_of( "0123456789" ) == string::npos;
}

void read_data() {
	//loading data1.txt and store the data 
	ifstream file("data1.txt");
	string line;
	string country = "";
	bool ready = false;
	country_data = "0 ";
	map<string, set<string> > all_users;
	while (getline(file, line)) {
		istringstream ss(line);
		string word;
		ss >> word;
		string user_id = "";

		if(is_int(word)) {
			user_id = word;
		} else {
			country_data += word + " ";
			if(ready) {
				user_list_by_country.insert(pair<string, map<string, set<string> > >(country, all_users));
			}
			country = word;
			all_users.clear();
			ready = true;
		}
		
		set<string> neighbor_user_list;
		while(ss >> word) {
			neighbor_user_list.insert(word);
		}	
		all_users.insert(pair<string, set<string> >(user_id, neighbor_user_list));
	}
	user_list_by_country.insert(pair<string, map<string, set<string> > >(country, all_users));
}

void init_UDP_Socket() {
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, MAIN_SERVER_UDP_PORT, &hints, &main_server_info);

	UDP_Connect_Sock = socket(main_server_info->ai_family, main_server_info->ai_socktype, main_server_info->ai_protocol);
	if(UDP_Connect_Sock == FAIL) {
		perror("ERROR: Cannot open server A socket");
		close(UDP_Connect_Sock);
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_A_UDP_PORT, &hints, &server_A_info);

	if(bind(UDP_Connect_Sock, server_A_info->ai_addr, server_A_info->ai_addrlen) == FAIL) {
		perror("ERROR: Fail to bind UDP socket");
		close(UDP_Connect_Sock);
		exit(1);
	}

	printf("The server A is up and running using UDP on port %s\n", SERVER_A_UDP_PORT);
}

void send_data(string s) {
	int len = s.length();
	char country_list[len];
	strcpy(country_list, s.c_str());
	if(sendto(UDP_Connect_Sock, country_list, len, 0, main_server_info->ai_addr, main_server_info->ai_addrlen) == FAIL) {
		perror("ERROR: Fail to send data to main server");
		close(UDP_Connect_Sock);
		exit(1);
	}

	printf("The server A has sent a country list to Main Server\n");
}

void recv_request() {
	// empty buffer
	memset(buf, 0, sizeof buf);
	addr_len = sizeof main_server_info;
	if(recvfrom(UDP_Connect_Sock, buf, MAXDATASIZE, 0, main_server_info->ai_addr, &addr_len) == FAIL) {

	}
}

int main() {
	read_data();
	init_UDP_Socket();
	recv_request();
	send_data(country_data);
	while(true) {
		recv_request();
		istringstream ss(buf);
		string country, id;
		ss >> country;
		ss >> id;
		printf("The server A has received request for finding possible friends of User%s in %s\n", id.c_str(), country.c_str());
		map<string, map<string, set<string> > >::iterator it = user_list_by_country.find(country);
		if(it != user_list_by_country.end()) {
			map<string, set<string> >::iterator ite = it->second.find(id);
			if(ite != it->second.end()) {
				printf("The server A is searching possible friends for User%s ...\n", id.c_str());
				set<string>::iterator iter;
				string neighbor_users = "A ";
				for(iter = ite->second.begin(); iter != ite->second.end(); iter++) {
					neighbor_users += (*iter + " ");
				}
				printf("Here are the results: %s\n", neighbor_users.c_str());
				send_data("1 " + neighbor_users);
				printf("The server A has sent the result(s) to Main Server\n");
			} else {
				printf("User%s does not show up in %s\n", id.c_str(), country.c_str());
				send_data("0 A");
				printf("The server A has sent \"User%s not found\" to Main Server\n", id.c_str());
			}
		}
	}
	return 0;
}