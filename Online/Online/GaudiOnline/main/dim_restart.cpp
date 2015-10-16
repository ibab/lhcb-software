#include <cerrno>
#include <cstdio>
#include <unistd.h>
extern "C" {
#include "dis.h"
}

int main(int /* argc */, char** /* argv */) {
  static int run = 0;
  int id = ::dis_add_service((char*)"Initial_ID/run",(char*)"I",&run,sizeof(run),0,0);
  ::dis_start_serving((char*)"Initial_ID");
  printf("Dim started with ID:Initial_ID and service created. Wait for 10 secs.\n");
  ::sleep(10);
  ::dis_remove_service(id);
  ::dis_stop_serving();
  ::dim_stop();
  printf("Dim is now stopped. Wait for 10 secs.\n");
  ::sleep(10);

  id = ::dis_add_service((char*)"Updated_ID/run",(char*)"I",&run,sizeof(run),0,0);
  ::dim_init();
  ::dis_start_serving((char*)"Updated_ID");
  printf("Dim started with ID:Updated_ID and service created. Wait......\n");
  while(1) ::sleep(1);
  return 1;
}


/*

g++ -o xxxx -I../../dim -ldim -lrt -pthread ../test/dim_restart.cpp

[frankm@plus04 cmt]$ ./xxxx
PID:20407 Register Child to DNS....
PID 20407 - Fri Dec  3 22:01:57 2010 - (FATAL) Child: Some Services already known to DNS
PID:20404 RE-Register Parent to DNS....

*/
