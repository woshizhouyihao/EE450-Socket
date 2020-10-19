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
#define SERVER_B_UDP_PORT "31777"
#define MAIN_SERVER_UDP_PORT "32777"
#define FAIL -1

int UDP_Connect_Sock;
struct addrinfo hints, *main_server_sock, *serverA_sock;
map<string, set<string> > all_users;
map<string, map<string, set<string> > > user_list_by_country;
string country_data;

bool is_init(string);

void read_data();

void init_UDP_Socket();

void send_data(string);

bool is_int(string s) {
	return s.find_first_not_of( "0123456789" ) == string::npos;
}

void read_data() {
	//loading data1.txt and store the data 
	ifstream file("data2.txt");
	string line;
	string country = "";
	bool ready = false;
	country_data = "0 ";
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
	getaddrinfo(LOCAL_HOST, MAIN_SERVER_UDP_PORT, &hints, &main_server_sock);

	UDP_Connect_Sock = socket(main_server_sock->ai_family, main_server_sock->ai_socktype, main_server_sock->ai_protocol);
	if(UDP_Connect_Sock == FAIL) {
		perror("ERROR: Cannot open server B socket");
		close(UDP_Connect_Sock);
		exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(LOCAL_HOST, SERVER_B_UDP_PORT, &hints, &serverA_sock);

	if((bind(UDP_Connect_Sock, serverA_sock->ai_addr, serverA_sock->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to bind UDP socket");
		close(UDP_Connect_Sock);
		exit(1);
	}

	printf("The server B is up and running using UDP on port %s\n", SERVER_B_UDP_PORT);
}

void send_data(string s) {
	int len = s.length();
	char country_list[len];
	strcpy(country_list, s.c_str());
	if((sendto(UDP_Connect_Sock, country_list, len, 0, main_server_sock->ai_addr, main_server_sock->ai_addrlen)) == FAIL) {
		perror("ERROR: Fail to send data to main server");
		close(UDP_Connect_Sock);
		exit(1);
	}

	printf("The server B has sent a country list to Main Server\n");
}

int main() {
	
	read_data();
	init_UDP_Socket();
	send_data(country_data);

	// for(map<string, map<string, set<string> > >::iterator ite = user_list_by_country.begin(); ite != user_list_by_country.end(); ite++) {
	// 	cout << ite->first << endl;
	// 	for(map<string, set<string> >::iterator iter = ite->second.begin(); iter != ite->second.end(); iter++) {
	// 	  cout<<iter->first << endl;
	// 	  for (set<string>::iterator it=iter->second.begin(); it!=iter->second.end(); ++it) {
	//     	cout << ' ' << *it;
	//     }
	//     cout << endl;
	// 	}
	// }

	
	
	// printf("The server B has received request for finding possible friends of User%s in %s\n", id, country);
	// printf("User%s does not show up in %s\n", id, country);
	// printf("The server B has sent \"User%s not found\" to Main Server\n", id);

	// printf("The server B is searching possible friends for User%s ...\n");
	// printf("Here are the results: User...\n");

	// printf("The server B has sent the result(s) to Main Server\n");
}