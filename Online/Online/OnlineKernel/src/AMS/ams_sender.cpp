#include <cstdio>
#include <cstring>
#include <string>
#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "AMS/amsdef.h"

#define SIZE 20000

static void err_print(int sc) {
  printf("Failed: Error=%d\n",sc);
}

static void fill (char *buff,int len)    {
  for (int i = 0; i < len; i++)
    buff [i] = (len + i) & 0xFF;
}

static void check (char *buff1,char *buff2,int len1,int len2)    {
  int nerr = 0;
  if (len1 != len2)
    printf ("Length mismatch: sent %d received %d bytes\n", len2, len1);
  else
    for (int i = 0; i < len1; i++)    {
      if (nerr++ > 10)
        break;
      if (buff1 [i] != buff2 [i])
        printf ("Mismatch: sent 0x%X received 0x%X at %d\n",
        buff2 [i], buff1 [i], i);
    }
}
static void help() {
  ::printf("amsc_sender -opt [-opt]\n");
  ::printf("    -n=<name>              AMS task name\n");
  ::printf("    -t=<number>            Number of receive/send turns\n");
  ::printf("    -s=<target process>    Client mode (default:server)\n");
  ::printf("    -l=<number>            Message length (Client only)\n");
}

extern "C" int amsc_sender(int argc, char **argv)  {
  std::string amsname("SEND_0"), target;
  char node[20], source[80];
  int wsize, length=256, loop=100000;
  char wmessage [SIZE];
  unsigned int facility=WT_FACILITY_DAQ_EVENT;

  RTL::CLI cli(argc, argv, help);
  cli.getopt("sendto",1,target);
  cli.getopt("name",1,amsname);
  cli.getopt("length",1,length);
  cli.getopt("facility",1,facility);
  cli.getopt("turns",1,loop);

  if ( length==0 ) length=10;
  if (length > SIZE) length = SIZE;
  printf (" Starting ams test task (%s) \n",amsname.c_str());
  strcpy(source,target.c_str());
  printf(" task %s: size %d  facility:%d turns:%d\n",source,length,facility,loop);
  int ams_status = amsc_init (amsname.c_str());
  if (ams_status != AMS_SUCCESS)  {
    printf ("Can't initialise ams\n");
    exit (ams_status);
  }
  amsc_get_node( node );

  // Receive some messages and bounce them
  for (int i=0; loop > 0; --loop, ++i)  {
    //if ( (i%5) == 0 ) lib_rtl_sleep(1);
    wsize =  length;
    fill (wmessage, wsize);
    ams_status = amsc_send_message (wmessage, wsize, source, facility, 0);
    if (ams_status != AMS_SUCCESS) err_print (ams_status);
    if (i % 100 == 0) printf ("%d\n", i);
  }
  printf("Hit key+Enter to exit ...");
  getchar();
  amsc_close();
  return 0;
}
