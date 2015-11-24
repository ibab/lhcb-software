#include <cstdlib>
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
    buff [i] = char((len + i) & 0xFF);
}

static void check (char *buff1,char *buff2,int len1,int len2)    {
  int nerr = 0;
  if (len1 != len2)
    ::printf ("Length mismatch: sent %d received %d bytes\n", len2, len1);
  else
    for (int i = 0; i < len1; i++)    {
      if (nerr++ > 10)
        break;
      if (buff1 [i] != buff2 [i])
        ::printf ("Mismatch: sent 0x%X received 0x%X at %d\n",
		  buff2 [i], buff1 [i], i);
    }
}
static void help() {
  ::printf("amsc_bounce -opt [-opt]\n");
  ::printf("    -n=<name>              AMS task name\n");
  ::printf("    -t=<number>            Number of receive/send turns\n");
  ::printf("    -s=<target process>    Client mode (default:server)\n");
  ::printf("    -l=<number>            Message length (Client only)\n");
}

extern "C" int amsc_bounce(int argc, char **argv)  {
  void* dummy;
  size_t rsize;
  std::string amsname, my_name, target;
  char node[20], source[80];
  int  length=256, loop=100000, substatus;
  char wmessage [SIZE], rmessage [SIZE];
  unsigned int facility;

  RTL::CLI cli(argc, argv, help);
  bool fanout = cli.getopt("sendto",1) != 0;
  cli.getopt("sendto",1,target);
  cli.getopt("name",1,amsname);
  cli.getopt("turns",1,loop);
  cli.getopt("length",1,length);

  my_name = amsname;
  if ( length==0 ) length=10;
  if (length > SIZE) length = SIZE;
  ::printf (" Starting ams test task (%s) %s for %d turns\n",
            (fanout) ? "Sender" : "Reader", amsname.c_str(),loop);
  if (fanout)  {
    ::strncpy(source,target.c_str(),sizeof(source));
    source[sizeof(source)-1] = 0;
    ::printf(" task %s: size %d\n",source,length);
  }
  int ams_status = amsc_init (amsname.c_str());
  if (ams_status != AMS_SUCCESS)  {
    ::printf ("Can't initialise ams\n");
    ::exit (ams_status);
  }
  amsc_get_node(node);
  // receive some messages and bounce them
  int  wsize =  length;
  int mx_loop = (fanout) ? loop : loop;
  if (fanout)  {
    srand(length);
    fill (wmessage, wsize);
    ams_status = amsc_send_message (wmessage, wsize, source, 0, 0);
    if (ams_status != AMS_SUCCESS)
      err_print (ams_status);
  }
  ::fflush(stdout);
  while (1)  {
    do    {
      unsigned int ams_fac;
      rsize = SIZE;
      ::wtc_wait(&facility,&dummy,&substatus); 
      ams_status = amsc_read_message (rmessage, &rsize, source, &ams_fac, 0);

      /* for no wt_wait 
         ams_status = amsc_get_message (rmessage, &rsize, source,
         0, 10, 0, 0, 0);
      */
      if (ams_status != AMS_SUCCESS)
        err_print (ams_status);
    } while (ams_status != AMS_SUCCESS && !fanout);
    --mx_loop;
    if (fanout && mx_loop != 0)   {
      check (rmessage, wmessage, rsize, wsize);
      wsize =  length;
      fill (wmessage, wsize);
      ams_status = amsc_send_message (wmessage, wsize, source, 0, 0);
    }
    else  {
      ams_status = amsc_send_message (rmessage, rsize, source, 0, 0);
    }
    if (ams_status != AMS_SUCCESS) err_print (ams_status);
    if (++loop % 200 == 0)   {
      char* p = source, *q = source;
      while ( (p=strchr(p,':')) ) *p++ = 0, q = p; 
      if ( 0 == q ) q = source;
      ::printf("%s %d / %d -- Last msg from:%s\n", 
               my_name.c_str(), loop, mx_loop, q);
      ::fflush(stdout);
    }
    if (mx_loop == 0 ) {
      ::printf ("%s AMS Test successfully ended.\n", my_name.c_str());
      ::printf ("%s Exiting....\n", my_name.c_str());
      ::fflush(stdout);
      ::lib_rtl_sleep(1000);
      ::amsc_close();
      ::lib_rtl_sleep(1000);
      ::printf ("%s Shutdown done....\n", my_name.c_str());
      ::fflush(stdout);
      ::exit(0);
      goto Done;
    }
  }
 Done:
  return 0;
}
