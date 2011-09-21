/*
 * BRTL_Lock.cpp
 *
 *  Created on: Jul 26, 2011
 *      Author: beat
 */

#include <string.h>
#include <stdio.h>
#include "RTL/rtl.h"
#include "Gaucho/BRTL_Lock.h"
#include <errno.h>

BRTLLock::BRTLLock(void)
{
  pthread_mutex_t tmp = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
  m_lid = tmp;
  m_name = "Lock_Name_Not_Set";
}

BRTLLock::~BRTLLock(void)
{
  pthread_mutex_destroy(&m_lid);
}

int BRTLLock::lockMutex()
{
  int status = pthread_mutex_lock(&m_lid);
  if (status != 0)
  {
    printf("%s: Error from locking mutex %s Status %s\n", RTL::processName().c_str(),m_name.c_str(),strerror(status));
  }
  return status;
}
int BRTLLock::unlockMutex()
{
  int status = pthread_mutex_unlock(&m_lid);
  if (status != 0)
  {
    printf("%s: Error from unlocking mutex %s Status %s\n",  RTL::processName().c_str(),m_name.c_str(),strerror(status));
    if (status == EPERM)
    {
      status =0;
    }
  }
  return status;
}

