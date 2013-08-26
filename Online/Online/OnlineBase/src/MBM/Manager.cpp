#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "bm_internals.h"
#include "Manager.h"
#include <stdexcept>

/// A copy constructor cannot be provided
MBM::Manager::Manager(const Manager&) : m_bm(0), bm_id(0), bm_all(0)
{
  ctrl_mod[0] = buff_mod[0] = 0;
}

/// The assignment operator cannot be provided
MBM::Manager& MBM::Manager::operator=(const Manager&)   {
  return *this;
}

/// Default constructor
MBM::Manager::Manager() : m_bm(0), bm_id(0), bm_all(0) {
  ctrl_mod[0] = buff_mod[0] = 0;
  bm_id = (char*)::getenv("BM_ID");
  if(!bm_id) bm_id = "0";
  m_bm = new ServerBMID_t();
}

/// Default destructor 
MBM::Manager::~Manager()  {
  if ( m_bm ) {
    ::mbmsrv_unmap_memory(m_bm);
    delete m_bm;
  }
}

/// Setup manager
void MBM::Manager::setup(const char* id)  {
  if ( id )   {
    bm_id = (char*)id;
  }
  if ( bm_id )   {
    ::snprintf(ctrl_mod, sizeof(ctrl_mod), "bm_ctrl_%s",bm_id);
    ::snprintf(buff_mod, sizeof(buff_mod), "bm_buff_%s",bm_id);
    return;
  }
  throw std::runtime_error("Invalid MBM buffer name supplied [NULL-string]");
}

/// Get installer options
void MBM::Manager::getOptions(int argc, char** argv)    {
  while (--argc > 0) {        /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
  setup(bm_id);
}

/// Map BM memory sections
int MBM::Manager::mapSections()  {
  return ::mbmsrv_map_memory(bm_id, m_bm);
}

/// Map BM monitoring memory sections
int MBM::Manager::mapMonitorSections()  {
  return ::mbmsrv_map_mon_memory(bm_id, m_bm);
}

/// Unmap BM memory sections
int MBM::Manager::unmapSections()  {
  return ::mbmsrv_unmap_memory(m_bm);
}

/// Release the server's BMID and give ownership to the caller
ServerBMID MBM::Manager::releaseBMID() {
  ServerBMID tmp = m_bm;
  m_bm = 0;
  return tmp;
}
