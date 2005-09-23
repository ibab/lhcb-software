#include "RTL/rtl.h"
#include "WT/wtdef.h"
#include "MBM/Client.h"
#include <stdexcept>

// Initializing constructor
MBM::Client::Client(const std::string& buffer_name, const std::string& client_name, int partition_id)
  : m_buffName(buffer_name), m_name(client_name), m_partID(partition_id), m_facility(0), m_blocking(true)
{
  static bool first = true;
  if ( first )  {
    first = false;
    ::wtc_init();
  }
  m_bmid = ::mbm_include(m_buffName.c_str(),m_name.c_str(),m_partID);
  if ( int(m_bmid) == -1 )  {
    throw std::runtime_error("Failed to include into MBM buffer:"+m_buffName);
  }
}

// Standard destructor
MBM::Client::~Client()
{
  if ( int(m_bmid) != -1 ) {
    int status = ::mbm_exclude(m_bmid);
    if ( status != MBM_NORMAL )  {
      throw std::runtime_error("Failed to exclude from MBM buffer:"+m_buffName);
    }
  }
}

// Access to process id
int MBM::Client::pid()  {
  return lib_rtl_pid();
}

// Switch to non-blocking execution mode
void MBM::Client::setNonBlocking(int facility, bool /* subscribe */) {
  m_blocking = false;
  m_facility = facility;
}

// Switch to non-blocking execution mode
void MBM::Client::setBlocking() {
  if ( !m_blocking ) {
    ::wtc_remove(m_facility);
  }
  m_blocking = true;
  m_facility = 0;
}


// Run the application with WT
int MBM::Client::run() {
  while(1)  {
    int sub_status;
    void* userpar;
    unsigned int facility;
    int status = ::wtc_wait(&facility, &userpar, &sub_status);
    ::printf("Exited WAIT>>>> Facility = %d Status=%d Sub-Status = %d\n", 
      facility, status, sub_status);
  }
  return 1;
}
