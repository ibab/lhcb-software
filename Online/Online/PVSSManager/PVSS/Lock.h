// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/Lock.h,v 1.1 2007-03-01 15:48:59 frankb Exp $
//  ====================================================================
//  Lock.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
// $Id: Lock.h,v 1.1 2007-03-01 15:48:59 frankb Exp $
#ifndef _PVSS_PVSSMANAGER_LOCK_H
#define _PVSS_PVSSMANAGER_LOCK_H

/*
 *   PVSS namespace declaration
 */
namespace PVSS  {

  /**@class Lock Lock.h PVSS/Lock.h
    *
    * @author  M.Frank
    * @version 1.0
    */
  class Lock  {
    /// Execution mode (inverted or not)
    bool           m_mode;
    /// Lock status
    int            m_status;
    /// Lock identifier
    void*          m_id;
  public:
    /// Initializing constructor for inverted lock
    Lock(void* id, bool inverted=false);
    /// Standard destructor
    ~Lock();
    /// Create now named lock
    static int create(void*& id);
    /// Delete lock
    static int unlink(void* id);
    /// Validity check
    operator int () const  {      return (1&m_status) == 1;    }
    /// Access to object status
    int status() const     {      return m_status;             }
  };
}

#endif // _PVSS_PVSSMANAGER_LOCK_H
