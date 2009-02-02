
#include "client.h"
#include "container.h"

container out;
using namespace std;

int main(int /* argc */, char ** argv)
{

#ifdef _WIN32
   printf("I will not work properly under windows\n");
   
 #endif
  if (argv[1] == NULL)
    cerr << "usage: warn <host> <ID/Level/Text>"<<endl;
  if (argv[2] == NULL)
    cerr << "usage: warn <host> <ID/Level/Text>"<<endl; 

  client c(argv[1],12345);
 
  out.reset();
  
  out.add("TEXT","Time of report: ");
  time_t t = time(NULL);
  out.add("CTIME",ctime(&t));
  
  // out.add("UTIME",&t, sizeof(time_t));
  // out.add("DIMA","H2D/node001/RichHitMapsMonitor/R20LocalCH");
  // out.add("THIST","10 0 10 0 0 1 2 3 19 5 4 3 2");
  
  std::stringstream s;
  out.tostream(s);
  
  c.Connect();
  
  
  //  c.wait();
  char buf[3];
  while ( c.rd(buf,2) == -2){};

  if (strncmp(buf,"GO",2) ==0)
  {
    c.wr(argv[2],strlen(argv[2]));
    c.wr("\n",1);
    
    c.wr(s.str().c_str(),s.str().length());
  }
  c.shut_close();

}
