
#include "client.h"
#include "container.h"

container out;
using namespace std;

int main(int argc, char ** argv)
{
 #ifdef _WIN32
  printf("I will not work properly under windows\n");
   
 #endif

  if (argv[1] == NULL){
    cerr << "usage: " << argv[0] << " <ID/Level/Text> <host>:<port> ... \nA space separated list of host:port can be given and port is optional. Defaults to 12345."<<endl;
    exit(EXIT_FAILURE);
  }
  if (argv[2] == NULL){
    cerr << "usage: " << argv[0] << " <ID/Level/Text> <host>:<port> ... \nA space separated list of host:port can be given and port is optional. Defaults to 12345."<<endl;
    exit(EXIT_FAILURE);
  }

  out.reset();
  
  out.add("TEXT","Time of report: ");
  time_t t = time(NULL);
  out.add("CTIME",ctime(&t));
  
  // out.add("UTIME",&t, sizeof(time_t));
  // out.add("DIMA","H2D/node001/RichHitMapsMonitor/R20LocalCH");
  // out.add("THIST","10 0 10 0 0 1 2 3 19 5 4 3 2");
  
  std::stringstream s;
  out.tostream(s);

  // Send command to server list.
  for (int i=2; i<argc;++i){
    // Start by initialising name to host:port and we will split it with string operations later.
    std::string name(argv[i]);
    std::string port;
    // The delimiter between the host and port is a colon.
    // Need to find this to split them. If we don't find
    // a colon then we will default to 12345, which is CAMERA's
    // default inport.
    std::size_t pos = name.find(":");
    if (pos != string::npos){
      port = name.substr(pos+1);
      name = name.substr(0,pos);
    }
    if (port == "") port = "12345";
    std::cerr << "Connecting to "<<name<<" "<<port<<std::endl;
    client c(name.c_str(),atoi(port.c_str()));

    if (0!=c.m_healthy) {
      if (1 == c.Connect()) {
        //  c.wait();
        char buf[3];
        c.rd(buf,3);
        if (strncmp(buf,"GO",2) ==0)
        {
          c.wr(argv[1],strlen(argv[1]));
          c.wr("\n",1);
          
          //    c.wr(s.str().c_str(),s.str().length());
        }
        else {
          printf("Server busy. try again\n");
        }
        c.shut_close();
      }
    }
  }
  return EXIT_SUCCESS;
}
