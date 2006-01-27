#include "AMS/amsdef.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "WT/wtdef.h"

#define SIZE 20000

static char source[80];
static char *amsname = "TESTAMS";
static int fanout;
static int wsize, length;
static size_t rsize;
static char wmessage [SIZE];
static char rmessage [SIZE];
static int loop=0;

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

extern "C" int amsc_bounce(int ac, char *av[])  {
  int substatus;
  unsigned int facility;
  void* dummy;
  char node[20];

  if (ac > 1)
    amsname = av[1];
  if (ac == 3)
    fanout = (*av[2] == 'o');

  printf (" Starting ams test task %s \n",amsname);
  if (fanout)  {
    printf ("Name of the task: ");
    scanf ("%s", source);
    printf ("Message size: ");
    scanf ("%d", &length);
    if ( length==0 ) length=10;
    printf(" task %s: size %d\n",source,length);

    if (length > SIZE)
      length = SIZE;
  }
  int ams_status = amsc_init (amsname);
  if (ams_status != AMS_SUCCESS)  {
    printf ("Can't initialise ams\n");
    exit (ams_status);
  }


  amsc_get_node( node );

  /* Receive some messages and bounce them */
  if (fanout)  {
    srand (length);
    wsize =  length;
    fill (wmessage, wsize);
    ams_status = amsc_send_message (wmessage, wsize, source, 0, 0);
    if (ams_status != AMS_SUCCESS)
      err_print (ams_status);
  }

  while (1)  {
    do    {
      unsigned int ams_fac;
      rsize = SIZE;
      wtc_wait(&facility,&dummy,&substatus); 
      ams_status = amsc_read_message (rmessage, &rsize, source, &ams_fac, 0);

      /* for no wt_wait 
      ams_status = amsc_get_message (rmessage, &rsize, source,
      0, 10, 0, 0, 0);
      */

      if (ams_status != AMS_SUCCESS)
        err_print (ams_status);
    } while (ams_status != AMS_SUCCESS && !fanout);

    if (fanout)   {
      check (rmessage, wmessage, rsize, wsize);
      wsize =  length;
      fill (wmessage, wsize);
      ams_status = amsc_send_message (wmessage, wsize, source, 0, 0);
    }
    else  {
      ams_status = amsc_send_message (rmessage, rsize, source, 0, 0);
    }
    if (ams_status != AMS_SUCCESS)
      err_print (ams_status);
    if (++loop % 100 == 0){
      printf ("%d\n", loop);
    }
    if ( loop>150000 ) exit(0);
  }
  return 0;
}
