all: servermake clientmake
servermake: server.cpp client_request_parse.cpp ds.cpp 
	 g++ -std=c++11 server.cpp -o server
clientmake: client.cpp file_share.h
	 g++ -std=c++11 -pthread client.cpp -o client
	 
