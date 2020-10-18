all: serverA.cpp serverB.cpp servermain.cpp client.cpp
		g++ -o serverA serverA.cpp
		g++ -o serverB serverB.cpp
		g++ -o mainserver servermain.cpp
		g++ -o client client.cpp

.PHONY: serverA
serverA: 
		./serverA

.PHONY: serverB
serverB: 
		./serverB

.PHONY: mainserver
mainserver: 
		./mainserver

.PHONY: client
client: 
		./client