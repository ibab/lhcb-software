

#include "client.h"
#include "container.h"

container out;
using namespace std;

int main(int /* argc */, char ** argv)
{
 #ifdef _WIN32
   printf("I will not work properly under windows\n");  
 #endif
  if (argv[1] == NULL){
    cerr << "usage: camtest <host>"<<endl;
    exit(0);
  }

  client c(argv[1],12345);
  out.reset();

  out.add("TEXT","Stress test of bulk warnings");
  out.add("TEXT","Time of report: ");
  time_t t = time(NULL);
  out.add("CTIME",ctime(&t));

  out.add("TTELLIPSE","0.5 0.5 0.25 0.25");
  float f[4];

  f[0] = .75;
  f[1] = .75;
  f[2] = .1;
  f[3] = .1;

  out.add("TELLIPSE",f,4*sizeof(float));

  f[0] = .25;
  f[1] = .75;
  f[2] = .1;
  f[3] = .1;

  out.add("TELLIPSE",f,4*sizeof(float));

  std::stringstream s;
  out.tostream(s);
  int j=0;

  //  while (1)
  for (int cntr=0;cntr<10;++cntr)  {

    c.Connect();


    //  c.wait();

    char buf[3];
    while (  c.rd(buf,2)==-2){};
    buf[3] = 0;
    printf("%s\n",buf);
    if (strncmp(buf,"GO",2) ==0)
    {
      j++;

      c.wr("camtest/1/Hello World\n",22);
      // c.wr(argv[2],strlen(argv[2]));
      //c.wr("\n",1);

      c.wr(s.str().c_str(),s.str().length());
    }
    else{
      cerr << "Problem connecting to server"<<endl;
    }
    c.new_sock();
    // usleep(10000);

    //  cout <<j <<" "<< i++ <<endl;
  }
  c.shut_close();



}
