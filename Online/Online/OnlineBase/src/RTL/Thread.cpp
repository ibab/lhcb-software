#include "RTL/Thread.h"

RTL::Thread::Thread() /* : m_id(0) */ {
  //  m_status = lib_rtl_create_thread(m_id);
  if ( !lib_rtl_is_success(m_status) )   {
    ::lib_rtl_signal_message(LIB_RTL_OS,"Cannot aquire Thread. Status=%d", m_status);
  }
}

RTL::Thread::~Thread()  {
  //  m_status = lib_rtl_unthread(m_id);
  if ( !m_status )  {
    // throw exception ?
  }
}

/// Create now named Thread
int RTL::Thread::create(const char* name, lib_rtl_thread_t& id)  {
  int status = 0; //lib_rtl_start_thread(name, &id);
  if (!lib_rtl_is_success(status))    {
    id = 0;
    ::lib_rtl_signal_message(LIB_RTL_OS,"Error in creating Thread %s. Status=%d", name, status);
  }
  return status;
}
