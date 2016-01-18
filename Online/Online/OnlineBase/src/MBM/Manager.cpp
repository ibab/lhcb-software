// $Id: $
//==========================================================================
//  LHCb Online software suite
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see OnlineSys/LICENSE.
//
// Author     : M.Frank
//
//==========================================================================
#define MBM_IMPLEMENTATION
#include "MBM/bmstruct.h"
#include "MBM/Manager.h"
#include "MBM/bmserver.h"
#include "bm_internals.h"
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
  m_unmap = true;
}

/// Constructor taking already mapped buffers
MBM::Manager::Manager(ServerBMID bm) : m_bm(bm), bm_all(0) {
  ctrl_mod[0] = buff_mod[0] = 0;
  setup(m_bm->bm_name);
  m_unmap = false;
}

/// Default destructor 
MBM::Manager::~Manager()  {
  if ( m_bm && m_unmap ) {
    ::mbmsrv_unmap_memory(m_bm);
    delete m_bm;
  }
  m_bm = 0;
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
  return m_unmap ? ::mbmsrv_map_memory(bm_id, m_bm) : MBM_NORMAL;
}

/// Map BM monitoring memory sections
int MBM::Manager::mapMonitorSections()  {
  return m_unmap ? ::mbmsrv_map_mon_memory(bm_id, m_bm) : MBM_NORMAL;
}

/// Unmap BM memory sections
int MBM::Manager::unmapSections()  {
  return m_unmap ? ::mbmsrv_unmap_memory(m_bm) : MBM_NORMAL;
}

/// Release the server's BMID and give ownership to the caller
ServerBMID MBM::Manager::releaseBMID() {
  ServerBMID tmp = m_bm;
  m_bm = 0;
  return tmp;
}
