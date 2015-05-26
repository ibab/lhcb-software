
#ifdef _WIN32
#include <stdio.h>
int main(){
  printf("I will not work at all under windows\nNot one bit...\n");
}
#endif


#ifndef _WIN32

#include "client.h"
#include <unistd.h>
#include <iostream>

#include <string>

using namespace std;
int main(int /* argc */, char ** argv){
  if (argv[1] ==NULL){
    // cerr  << "Address, please?"<<endl;
    _exit(1);
  }
  
  string str = argv[1];
  
  string::size_type position1 = str.find("/");

  string add = str.substr(0,position1);
  string file = str.substr(position1+1);

  
  
  string::size_type position2 = str.find(":");
  string host,port;
  
  host = add;
  port = "8888";
  
  if (position2 != string::npos){
    host = add.substr(0,position2);
    port = add.substr(position2+1);
  }
  
  
  client c(host.c_str(),atoi(port.c_str()));
  if (c.Connect()>0){
    c.wr("GET ",4);
    c.wr(file.c_str(),strlen(file.c_str()));
    c.wr("\n",1);
    c.shutwr();
    char buf[512];int r;
    while ((r = c.rd(buf,511))!=-1){
      buf[r] = '\0';
      // cout << buf<<endl;
      write(0,buf,r );
    }
  }
  else{
    // std::cerr << "Error Connecting"<<std::endl;
  }
  
}
#endif
