/*
 * SegvHandler.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: Beat Jost
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "Gaucho/SegvHandler.h"
static void segvhandler (int sig, siginfo_t *siginfo, void *context)
{
  char str[255];
  snprintf (str,255,"********SEGVHANDLER: Received signal %d, Faulting Address: %llx Access error: %s\n",sig,
      (unsigned long long )siginfo->si_addr, (siginfo->si_code == 1) ? "Address Not Mapped" : "insufficient Access Rights" );
  printf("%s",str);
  if (M_SegvHandler != 0)
  {
    if ((M_SegvHandler->oldact.sa_flags & SA_SIGINFO) != 0)
    {
      M_SegvHandler->oldact.sa_sigaction(sig,siginfo,context);
    }
    else
    {
      if (M_SegvHandler->oldact.sa_handler == SIG_DFL)
      {
        _exit(0);
      }
      else if (M_SegvHandler->oldact.sa_handler == SIG_IGN)
      {
        return;
      }
      else
      {
        M_SegvHandler->oldact.sa_handler(sig);
      }
    }
  }
}
SegvHandler::SegvHandler()
{
  if (M_SegvHandler == 0)
  {
    memset (&act,0,sizeof(act));
    act.sa_sigaction = &segvhandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGSEGV,&act,&oldact);
    M_SegvHandler = this;
  }
}

SegvHandler::~SegvHandler()
{
  sigaction(SIGSEGV,&oldact,&act);
  M_SegvHandler = 0;
}
