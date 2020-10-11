all: serverA.cpp serverB.cpp mainserver.cpp client.cpp
				g++ -o serverA serverA.cpp
				g++ -o serverB serverB.cpp
				g++ -o mainserver mainserver.cpp
				g++ -o client client.cpp

.PHONY: serverA
serverA: ./serverA
	@echo "1"

serverB: serverB.cpp
				./serverB

mainserver: mainserver.cpp
				./mainserver

clien: client.cpp
		 		./client