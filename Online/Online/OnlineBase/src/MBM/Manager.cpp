#define MBM_IMPLEMENTATION
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bm_struct.h"
#include "bm_internals.h"
#include "Manager.h"

MBM::Manager::Manager() : m_bm(0), bm_id(0), bm_all(0) {
  bm_id = (char*)getenv("BM_ID");
  if(!bm_id) bm_id = "0";
}

MBM::Manager::~Manager()  {
  if ( m_bm ) mbm_unmap_memory(m_bm);
}

void MBM::Manager::setup(const char* id)  {
  if ( id ) bm_id = (char*)id;
  strcat(strcpy(ctrl_mod,  "bm_ctrl_"),bm_id);
  strcat(strcpy(user_mod,  "bm_user_"),bm_id);
  strcat(strcpy(event_mod, "bm_event_"),bm_id);
  strcat(strcpy(bitmap_mod,"bm_bitmap_"),bm_id);
  strcat(strcpy(buff_mod,  "bm_buffer_"),bm_id);
}

void MBM::Manager::getOptions(int argc, char** argv)    {
  while (--argc > 0) {                                  /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
  setup();
}

int MBM::Manager::mapSections()  {
  m_bm = mbm_map_memory(bm_id);
  return m_bm ? MBM_NORMAL : MBM_ERROR;
}

int MBM::Manager::mapMonitorSections()  {
  m_bm = mbm_map_mon_memory(bm_id);
  return m_bm ? MBM_NORMAL : MBM_ERROR;
}

int MBM::Manager::unmapSections()  {
  if ( m_bm ) {
    // Internally this deletes the buffer descriptor!
    int sc = mbm_unmap_memory(m_bm);
    m_bm = 0;
    return sc;
  }
  return MBM_NORMAL;
}

