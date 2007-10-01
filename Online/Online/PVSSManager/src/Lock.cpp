// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/src/Lock.cpp,v 1.2 2007-10-01 14:46:55 frankm Exp $
//  ====================================================================
//  Lock.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Lock.cpp,v 1.2 2007-10-01 14:46:55 frankm Exp $
#include "PVSS/Lock.h"
#include "PVSS/Internals.h"
#include <cerrno>
#include <cstdio>

PVSS::Lock::Lock(void* id, bool inverted) : m_mode(inverted), m_id(id) {
  m_status = m_mode ? pvss_release_lock(m_id) : pvss_take_lock(m_id);
  if ( !m_status )   {
    ::printf("PVSS> Cannot aquire lock. Status %d\n",m_status);
    errno = m_status;
  }
}

PVSS::Lock::~Lock()  {
  m_status = m_mode ? pvss_take_lock(m_id) : pvss_release_lock(m_id);
  if ( !m_status )  {
    // throw exception ?
  }
}

/// Create now named lock
int PVSS::Lock::create(void*& id)  {
  int status = pvss_create_lock(&id);
  if (!status)    {
    ::printf("PVSS> Cannot create lock. Status %d\n",status);
  }
  return status;
}

/// Create now named lock
int PVSS::Lock::unlink(void* id)  {
  int status = pvss_delete_lock(id);
  if (!status)    {
    ::printf("PVSS> Cannot delete lock. Status %d\n",status);
  }
  return status;
}
