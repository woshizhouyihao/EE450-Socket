Name: Yihao Zhou
ID: 9751577777

This assignment makes use of c++ socket package and implements a TCP socket between client and server and 2 UDP sockets between main server and backend server. This assignment consists of 4 ends, main server, backend server A, backend server B and client. 2 backend severs will read in and prepare the data for the incoming request from main server. Client will send request to main server and main server will direct the request to either backend server A or backend server B based on the request.

Code files: 
client.cpp
- initialize TCP socket connection to main server
- send and receive reply from main server

servermain.cpp
- initialize TCP socket and UDP socket between 2 backend servers
- listen to the client request and responsible for directing request to server A and B based on the content of the requset
- receive and direct the backend server reply back to client

serverA.cpp
- read in data file and process the data
- look up the corresponding recommendation and send result to main server

serverB.cpp
- read in data file and process the data
- look up the corresponding recommendation and send result to main server

The format of all messages exchanged
Since all messages exchange need to be char array, I preprocess all information, add necessary information of command, server info and content and separated all key words by a whitespace. And on each end first read the command and necessary info, then process the content.

Each variable is separated by a space

client
- request to main server: country + id
- reply from main server: command + country + id + content
						command = 0, country not found
								= 1, id not found
								= 2, 
								if first word of content not integer,
								"The client has results from Main Server: " + id + " is connected to all other users, no new recommentation"
								else "The client has results from Main Server: " + recommend_user_id + " is/are possible friend(s) of User" + id + country

serverA
boot up phase: 
- request from main server: "ask for list"
- reply to main server: server code + country list
						server code = 0

after booting up:
- request from main server: country + id
- reply to main server: command + server_info + content
							command = 0, user id not found
									= 1, user id found
									content = recommendation user id
							server_info = A, message from server A

serverB
boot up phase: 
- request from main server: "ask for list"
- reply to main server: server code + country list
						server code = 1

after booting up:
- request from main server: country + id
- reply to main server: command + server_info + content
							command = 0, user id not found
									= 1, user id found
									content = recommendation user id
							server_info = B, message from server B

mainserver
boot up phase:
- request to backend servers
- reply from backend servers

after booting up:
- request from client
- request to backend server
- reply from backend server
- message to client



Idiosyncrasy Based on the test results run on given environment (Ubuntu), there was no idiosyncrasy found

Reuse code:
Beej's Code: http://www.beej.us/guide/bgnet/
    Create sockets (TCP / UDP);
    Bind a socket;
    Send & receive (TCP / UDP);
