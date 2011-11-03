#include "RTL/Lock.h"
#include <cerrno>
#include <cstdio>

RTL::Lock::Lock(lib_rtl_lock_t id, bool inverted) : m_mode(inverted), m_id(id) {
  m_status = m_mode ? lib_rtl_unlock(m_id) : lib_rtl_lock(m_id);
  if ( !lib_rtl_is_success(m_status) )   {
    lib_rtl_signal_message(LIB_RTL_OS,"Cannot aquire lock. Status %d",m_status);
    errno = m_status;
  }
}

RTL::Lock::~Lock()  {
  m_status = m_mode ? lib_rtl_lock(m_id) : lib_rtl_unlock(m_id);
  if ( !m_status )  {
    // throw exception ?
  }
}

/// Create now named lock
int RTL::Lock::create(const char* name, lib_rtl_lock_t& id)  {
  int status = lib_rtl_create_lock(name, &id);
  if (!lib_rtl_is_success(status))    {
    lib_rtl_signal_message(LIB_RTL_OS,"Cannot create lock %s. Status %d",name,status);
  }
  return status;
}

/// Create now named lock
int RTL::Lock::unlink(lib_rtl_lock_t id)  {
  int status = lib_rtl_delete_lock(id);
  if (!lib_rtl_is_success(status))    {
    lib_rtl_signal_message(LIB_RTL_OS,"Cannot delete lock. Status %d",status);
  }
  return status;
}

