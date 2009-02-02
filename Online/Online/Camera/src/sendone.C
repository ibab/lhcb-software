
#include "client.h"

int main(int argc, char ** argv)
{
 #ifdef _WIN32
  printf("I will not work properly under windows\n");
   
 #endif
  if (argc <3)
  {
    printf("usage: %s host message\n",argv[0]);
    exit(1);
  }
  
  client c(argv[1],12345); 

  c.Connect();
  
  char buf[512];
    
  sprintf(buf,"ManualMessage/0/%s\n",argv[2]);
  
  c.wr(buf,strlen(buf));
  c.shut_close();
    
}
