#include <iostream>
#include <list>
#include <string.h>
#include <unordered_map>
#include <signal.h>

int server_socket, client_socket; 
std::fstream log_file;

typedef std::list<struct file_attr> file_to_ip;

struct file_attr {
  std::string alias;
  std::string ip_addr;
  std::string relative_path;
};



std::unordered_map<std::string,file_to_ip> mymap;

void write_to_file() {

  std::fstream file;
  file.open("Repo.txt", std::ios::out|std::ios::trunc|std::ios::binary);
  std::string temp = "file_name:relative_path:alias:ip_addr\n";
  file.write(temp.c_str(), temp.size());
  for(auto& x: mymap) {
    
    file_to_ip::iterator it;
    std::string temp;

    for (it=x.second.begin(); it!=x.second.end(); ++it) {
     temp  = x.first + ":" + it->relative_path + ":" + it->alias + ":" + it->ip_addr +  "\n" ;
     int length = temp.size();
     file.write(temp.c_str(), length);
    }

  }
  file.close();
}

void sig_hand(int signo) {

  if(signo == SIGINT) {
    printf("SIGINT Recieved\n");
  }
  write_to_file();
  close(server_socket);
  log_file.close();
  exit(1);
}

void disp_list(file_to_ip list) {
  file_to_ip::iterator it;

  for (it=list.begin(); it!=list.end(); ++it) {
    std::string temp =  it->ip_addr + " : " + it->alias + ":" + it->relative_path +" : " + "\n" ;
    std::cout << temp;
  }
}

file_to_ip get_list(std::string fname) {

  file_to_ip temp;
  for(auto x: mymap) {
    if(x.first == fname) {
      return x.second;
    }
  }
  return temp;

}

void disp_map() {
  for(auto& x: mymap) {
    std::cout << x.first << "-->";
    disp_list(x.second);
  }
}

void create_map(std::string fname, struct file_attr node) {
    file_to_ip list;
    list.push_back(node);
    std::pair<std::string,file_to_ip> temp (fname,list);
    mymap.insert(temp);
}

void delete_map(std::string fname, struct file_attr node) {

  if(!mymap.empty()) {
    
    for(auto& x: mymap) {

      if(x.first == fname) {
        // if an entry with file_name is found
        file_to_ip::iterator it;

        // iterate through the list for the file
        for (it=x.second.begin(); it!=x.second.end(); ++it) {

          if(it->ip_addr == node.ip_addr && it->relative_path == node.relative_path) {
            x.second.erase(it);
            break;
          }            

        }

        break;
      }
    }
  }
}

void insert_map(std::string fname, struct file_attr node) {

  if(!mymap.empty()) {

    int found = 0;
    
    for(auto& x: mymap) {

      if(x.first == fname) {
        // if an entry with file_name is found
        file_to_ip::iterator it;
        
        // iterate through the list for the file
        for (it=x.second.begin(); it!=x.second.end(); ++it) {

          if(it->ip_addr != node.ip_addr || it->relative_path != node.relative_path) {
            // if either ip or relative_path doesnt exist then
            x.second.push_back(node);
          } else {
            // if ip and relative path already exists then
            break;
          }

        }
        // file name found and required action has been taken,
        // no need to iterate among the remaining elements
        found = 1;
        break;
      }
  }

  if(found == 0) {
    create_map(fname, node);
  }

} else {
    create_map(fname, node);
}

}
