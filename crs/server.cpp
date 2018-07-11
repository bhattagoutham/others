#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <arpa/inet.h>
#include <time.h>
#include "client_request_parse.cpp"
#define PORT 12345



int main() {
	
	//create_ds();
	signal(SIGINT, sig_hand);
	time_t timer;
	
	int no_of_connections = 5;
	
	printf("Trying to create a socket..");
	while((server_socket = socket(AF_INET, SOCK_STREAM, 0))== 0) {
		printf(".");
		sleep(1);
	}

	struct sockaddr_in serv_addr;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("Socket Created...\n");
	listen(server_socket, no_of_connections);
	
	
	
	
  	log_file.open("log.txt", std::ios::out|std::ios::trunc|std::ios::binary);
  	char ip[INET_ADDRSTRLEN];
  	char time_buffer[80];
	while(1) {

		struct sockaddr_in cli_addr;
		socklen_t cli_len;
		printf("Waiting for clients...\n");
		//client_socket = accept(server_socket, NULL, NULL);
		client_socket = accept(server_socket,(struct sockaddr *)&cli_addr,&cli_len);

		printf("Connected to client...\n");
		
		inet_ntop(AF_INET, &(cli_addr.sin_addr), ip, INET_ADDRSTRLEN);
		
		char cmd[250];
		recv(client_socket, &cmd, sizeof(cmd), 0);
		std::string s = cmd;
		printf("Received from client: %s\n", s.c_str());
		time(&timer);
		struct tm * timeinfo = localtime (&timer);
		strftime (time_buffer,80,"%c",timeinfo);
		std::string time_str(time_buffer);
		std::string temp = "At" + time_str + " Received " + cmd + " from " + ip + "\n";
		log_file.write(temp.c_str(), temp.size());
		std::string res = get_search_details(s);
		printf("res:%s", res.c_str());
		
		char const *c = res.c_str();	
		send(client_socket, c, res.size(), 0);
		close(client_socket);
	}

	close(server_socket);
	printf("Socket Closed...\n");
	return 0;
}
