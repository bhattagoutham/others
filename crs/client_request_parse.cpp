// string::find_last_of
#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>         // std::size_t
#include "ds.cpp"

void del(std::string path) {

  char *pch;
  pch = strtok ((char*)path.c_str(),"|");
  struct file_attr temp;

  if (pch != NULL)
  {
    temp.ip_addr = pch;
    pch = strtok (NULL, "|");
    temp.alias = pch;
    pch = strtok (NULL, "|");
    path = pch;
  }

  std::size_t tmp = path.find_last_of("/");
  std::string fname;

    if(tmp == std::string::npos) {
      temp.relative_path = "./";
      fname = path;
    } else {
      temp.relative_path = path.substr(0, tmp);
      fname = path.substr(tmp+1);
    }

    delete_map(fname, temp);
    
}

void share(std::string path) {

  char *pch;
  pch = strtok ((char*)path.c_str(),"|");
  struct file_attr temp;

  if (pch != NULL)
  {
    temp.ip_addr = pch;
    pch = strtok (NULL, "|");
    temp.alias = pch;
    pch = strtok (NULL, "|");
    path = pch;
  }

  std::size_t tmp = path.find_last_of("/");
  std::string fname;

    if(tmp == std::string::npos) {
      temp.relative_path = "./";
      fname = path;
    } else {
      temp.relative_path = path.substr(0, tmp);
      fname = path.substr(tmp+1);
    }

    insert_map(fname, temp);
    
}

file_to_ip search (std::string str)
{
  
  std::size_t found = str.find_last_of("/");
  std::cout << "file:" << str.substr(found+1) << '\n';
  file_to_ip list = get_list(str.substr(found+1));
  return list;

}

void create_ds() {

std::string fname = "f1";
struct file_attr temp;
temp.alias = "goutham";
temp.relative_path = "OS_Assignment";
temp.ip_addr = "192.168.1.1";
insert_map(fname, temp); 
temp.ip_addr = "192.168.1.2";
insert_map(fname, temp);
fname = "f2";
insert_map(fname, temp);
fname = "f3";
insert_map(fname, temp);
temp.ip_addr = "192.168.1.3";
insert_map(fname, temp);
fname = "f1";
temp.ip_addr = "192.168.1.1";
delete_map(fname, temp);
temp.ip_addr = "192.168.1.2";
delete_map(fname, temp);
delete_map(fname, temp);
//disp_map();
}

std::string consolidate(file_to_ip list) {
  
  file_to_ip::iterator it;
  std::string s;
  for (it=list.begin(); it!=list.end(); ++it) {
    //std::cout << it->ip_addr << " | " << it->relative_path <<" | " << it->alias << "\n" ;
    s = s + it->ip_addr + "|" + it->relative_path + "|" +  it->alias + "\n";
  }
  s = s+";";
  return s;
}

std::string get_search_details(std::string input) {

  file_to_ip temp;
  std::string result;

  if(input.find("search")!=std::string::npos) {
    
    std::size_t found = input.find("search");
    std::size_t start = input.find_first_of("\"");
    std::size_t end = input.find_last_of("\"");
    if(start == std::string::npos && end == std::string::npos) {
      temp = search(input.substr(found+7));
      result = consolidate(temp);
      return result;
    } else {
      temp = search(input.substr(start+1, end-start-1));
      result = consolidate(temp);
      return result;
    }
    
  } else if (input.find("share")!=std::string::npos) {
    
    std::size_t share_pos = input.find("share");
    
    std::string path = input.substr(share_pos+6);
    share(path);
    // std::size_t start = input.find_first_of("\"");
    // std::size_t end = input.find_last_of("\"");
    
    // if(start == std::string::npos && end == std::string::npos) {
    //   share(path);
    //   return result;
    // } else {
    //   share(path.substr(start+1, end-start-1));
    //   return result;
    // }


  } else if (input.find("del")!=std::string::npos) {
    
        std::size_t del_pos = input.find("del");     

        std::string path = input.substr(del_pos+4);
        del(path);
        // std::size_t start = input.find_first_of("\"");
        // std::size_t end = input.find_last_of("\"");
        
        // if(start == std::string::npos && end == std::string::npos) {
        //   share(path);
        //   return result;
        // } else {
        //   share(path.substr(start+1, end-start-1));
        //   return result;
        // }

  }

}

