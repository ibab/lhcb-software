/*
** QUEUE ROUTINES BY TIM CHARITY - Sep 89
*/
#include "RTL/Lock.h"
#include <cerrno>
#include <cstdio>

RTL::Lock::Lock(const char* name, lib_rtl_lock_t id) : m_id(id) {
  m_status = lib_rtl_lock(name,id);
  if ( !lib_rtl_is_success(m_status) )   {
    errno = m_status;
    ::printf("Cannot aquire lock %s. Status %d\n",name, m_status);
  }
}

RTL::Lock::~Lock()  {
  m_status = lib_rtl_unlock(m_id);
  if ( !m_status )  {
    // throw exception ?
  }
}

/// Create now named lock
int RTL::Lock::create(const char* name, lib_rtl_lock_t& id)  {
  int status = lib_rtl_create_lock(name, &id);
  if (!lib_rtl_is_success(status))    {
    ::printf("error in creating lock %s. Status %d\n",name,status);
  }
  return status;
}

