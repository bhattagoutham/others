#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <string.h>
#include <thread>
#include <chrono>
#include <math.h>
#include "file_share.h"
#define CRS_PORT 12345



int crs_socket;
// file_req_socket is the socket using which client sends a request for file sharing
// crs_socket is the socket using which client communicates with crs
// client_server_socket is the socket using which client listens for file sharing request
// client_dowload_socket is the socket using which client transfers the file
std::string crs_ip;

void connect_to_crs() {

	printf("Trying to establish connection with crs..\n");
	while((crs_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf(".");
		sleep(1);
	}
	
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(crs_ip.c_str());
	serv_addr.sin_port = htons(CRS_PORT);

	while(connect(crs_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		printf(".");
	}

}




void search(std::string input) {

		connect_to_crs();
		
		char const *c = input.c_str();
		
		send(crs_socket, c, 40, 0);
		
		char res[250];
		int bytes = recv(crs_socket, &res, sizeof(res), 0);
		
		std::string tmp = res;
		std::size_t found = tmp.find_last_of(";");
		tmp = tmp.substr(0,found);
		printf("%s\n", tmp.c_str());

		memset (res,'\0',bytes);
		close(crs_socket);
	
}

void del(std::string input) {

		connect_to_crs();
		std::string path, ip;

		std::size_t share_pos = input.find("del");
		std::size_t start = input.find_first_of("\"");
    	std::size_t end = input.find_last_of("\"");
    
	    if(start == std::string::npos && end == std::string::npos) {
	      path = input.substr(share_pos+6);
	      
	    } else {
	      path = input.substr(start+1, end-start-1);
	      
	    }

	    std::string output = get_cmd_output("ifconfig | grep -m 1 \"inet addr\"");
  		std::size_t ip_pos = output.find("inet addr");
  		std::size_t ip_end = output.find("Bcast");
		if(ip_pos == std::string::npos) {
		    ip = "127.0.0.1";
		} else {
		    ip = output.substr(ip_pos+10, ip_end-ip_pos-12);
		}

		char *uname = getenv("LOGNAME");
		std::string alias = uname;
		std::string share_data = "del "+ip+"|"+alias+"|"+path;
		char const *c = share_data.c_str();
		send(crs_socket, c, 250, 0);
		close(crs_socket);

}

void share(std::string input) {

		connect_to_crs();
		std::string path, ip;

		std::size_t share_pos = input.find("share");
		std::size_t start = input.find_first_of("\"");
    	std::size_t end = input.find_last_of("\"");
    
	    if(start == std::string::npos && end == std::string::npos) {
	      path = input.substr(share_pos+6);
	      
	    } else {
	      path = input.substr(start+1, end-start-1);
	      
	    }

	    std::string output = get_cmd_output("ifconfig | grep -m 1 \"inet addr\"");
  		std::size_t ip_pos = output.find("inet addr");
  		std::size_t ip_end = output.find("Bcast");
		if(ip_pos == std::string::npos) {
		    ip = "127.0.0.1";
		} else {
		    ip = output.substr(ip_pos+10, ip_end-ip_pos-12);
		}

		char *uname = getenv("LOGNAME");
		std::string alias = uname;
		std::string share_data = "share "+ip+"|"+alias+"|"+path;
		char const *c = share_data.c_str();
		send(crs_socket, c, 250, 0);
		close(crs_socket);
}

int get(std::string input) {

	
	std::size_t ip_pos = input.find("ip");
  	std::size_t fname_pos = input.find("fname");
  	std::string ip, fname;

	if(ip_pos == std::string::npos || fname_pos == std::string::npos) {
	  printf("Invalid get command");
	  return -1;
	}
  
  	ip = input.substr(ip_pos+3, fname_pos-ip_pos-4);
  	fname = input.substr(fname_pos+6);
  
	connect_to_client(ip, fname);
	
	//char const *c = fname.c_str();//"c++ notes";
	send(file_req_socket, fname.c_str(), fname.size(), 0);

	char length[50];
	recv(file_req_socket, length, 50, 0);
	int len = atoi(length);
	if(len == -1) {
		printf("File not present\n");
	} else {
		printf("Length: %d\n", len);
		write_data_to_file(fname.c_str(), len);	
	}

	close(file_req_socket);
	return 0;
}

void exec(std::string input) {

	std::size_t ip_pos = input.find("ip");
	std::size_t cmd_pos = input.find("cmd");
    
    std::string ip = input.substr(ip_pos+3, cmd_pos-ip_pos-4);
    std::string cmd = input.substr(cmd_pos+4);

    connect_to_client(ip, cmd);
    cmd = "cmd="+cmd;
    send(file_req_socket, cmd.c_str(), cmd.size(), 0);
    char result[150];
	recv(file_req_socket, result, 150, 0);
	std::string cmd_output(result);
	printf("*********RPC************\n");
	printf("%s\n", cmd_output.c_str());

}
void get_input() {

	std::string input;

	std::thread t2(fun);
	sleep(1);

	while(1) {

		std::cout << "enter command: " ;	
		std::getline (std::cin,input);

		if(input == "exit") break;

		if(input.find("search")!=std::string::npos) {

			search(input);

		} else if(input.find("share")!=std::string::npos) {

			share(input);

		} else if(input.find("get")!=std::string::npos) {

			get(input);

		} else if(input.find("exec")!=std::string::npos) {

			exec(input);

		}  else if(input.find("del")!=std::string::npos) {

			del(input);

		}
				
	}

	stop = 1;
	// if t2 is sending data to a client.
	// then wait for it to join
	// else if t2 is sending then wait for it to join

	while(is_sending==1);
	shutdown(client_server_socket, 2);

	if(t2.joinable()) {
		t2.join();
	}
	printf("Exiting fun");

}

int main( int argc, char* argv[] ) {
  
	if(argc > 1) {
		crs_ip = argv[1];	
	}
    	
	std::thread t1(get_input);
	
	if(t1.joinable()) {
		t1.join();
	}
	
	return 0;
}