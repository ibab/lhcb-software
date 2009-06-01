
#include "client.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc , char ** argv)
{
 #ifdef _WIN32
  printf("I will not work properly under windows\n");
   
 #endif

  if (argv[1] == NULL){
    cerr << "usage: camswitch name host ..."<<endl;
    exit(1);
  }
  if ((std::string)argv[1] == "-h"){
    cerr << "usage: camswitch name host ..."<<endl;
    exit(0);
  }

  if (argv[2] == NULL){
    cerr << "usage: camswitch name host ..."<<endl;
    exit(1);
  }

  std::string msg = "COMMAND/9/switch "+(std::string) argv[1]+"\n";

  for (int i=2; i<argc;++i){
    std::string s = argv[i];
    std::string nam,port;
    port = "";
    nam = s;
    std::size_t pos  = s.find(":");
    if (pos != string::npos){
      nam = s.substr(0,pos);
      port = s.substr(pos+1);
    }
    if (port == "") port = "12345";
    std::cerr << "Connecting to "<<nam<<" "<<port<<std::endl;
    client c(nam.c_str(),atoi(port.c_str()));
    

    c.Connect();


    //  c.wait();
    char buf[3];
    c.rd(buf,3);
    if (strncmp(buf,"GO",2) ==0)
      {
	c.wr(msg.c_str(),msg.length());
	c.wr("\n",1);
	
      }
    else {
      printf("Server unavailable. try again\n");
    }
    c.shut_close();
    
  }
  return 0;
}
