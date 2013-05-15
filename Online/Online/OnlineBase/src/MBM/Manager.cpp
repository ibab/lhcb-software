#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "bm_internals.h"
#include "Manager.h"

/// Default constructor
MBM::Manager::Manager() : m_bm(0), bm_id(0), bm_all(0) {
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
  if ( id ) bm_id = (char*)id;
  ::strcat(strcpy(ctrl_mod, "bm_ctrl_"),bm_id);
  ::strcat(strcpy(buff_mod, "bm_buff_"),bm_id);
}

/// Get installer options
void MBM::Manager::getOptions(int argc, char** argv)    {
  while (--argc > 0) {        /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
  setup();
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
