

#include "client.h"
#include "container.h"

container out;
using namespace std;

int main(int  /*argc*/ , char ** argv)
{
 #ifdef _WIN32
   printf("I will not work properly under windows\n");  
 #endif
  if (argv[1] == NULL){
    cerr << "usage: camtest host [port]"<<endl;
    exit(0);
  }
  int port = 12345;
  
  if (argv[2] !=NULL) port = atoi(argv[2]);
  
  client c(argv[1],port);
  
  out.reset();

  out.add("TEXT","Stress test of bulk warnings");
  out.add("TEXT","Time of report: ");
  time_t t = time(NULL);
  out.add("CTIME",ctime(&t));

  out.add("TTELLIPSE","0.5 0.5 0.25 0.25");
  float f[4];

  f[0] = .75f;
  f[1] = .75f;
  f[2] = .1f;
  f[3] = .1f;

  out.add("TELLIPSE",f,4*sizeof(float));

  f[0] = .25f;
  f[1] = .75f;
  f[2] = .1f;
  f[3] = .1f;

  out.add("TELLIPSE",f,4*sizeof(float));

  std::stringstream s;
  out.tostream(s);
  int j=0;

  //  while (1){	
  //usleep(10000);	
	int reps = 10;
  if (argv[3]!=NULL) {
	int t=0;
        if (sscanf(argv[3],"%d",&t)>0)
	 reps = t;
  }
  for (int cntr=0;cntr<reps;++cntr)  {

    c.Connect();
    

    //  c.wait();

    char buf[3];
    while (  c.rd(buf,2)==-2){};
    // NM: the following line has been corrected by Nicola Mangiafave and
    // it has to be checked
    buf[2] = 0;
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
