// $Id: ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
//====================================================================
//
//  Package    : OnlineBase
//  Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/ISuspendable.h,v 1.2 2007-12-06 14:39:34 frankb Exp $
#ifndef _MBM_MANAGER_H
#define _MBM_MANAGER_H

// Include files
#include "RTL/rtl.h"
typedef struct ServerBMID_t* ServerBMID;

/*
 *    MBM Namespace declaration
 */
namespace MBM {

  /// Basic class to access buffer manager information for monitoring purposes.
  /*
   * @author  M.Frank
   * @version 1.0
   * @date    10/01/2006
   */
  struct Manager  {
    /// BM identifier
    ServerBMID    m_bm;
    /// Global section module name for the control sections
    char          ctrl_mod[64];
    /// Global section module name for the buffer data
    char          buff_mod[64];
    /// Buffer id
    const   char* bm_id;
    /// Global section handle to buffer inventory
    lib_rtl_gbl_t bm_all;
    /// Flag to prevent unmapping shared buffers
    bool          m_unmap;
  private:
    /// A copy constructor cannot be provided
    Manager(const Manager& c);
    /// The assignment operator cannot be provided
    Manager& operator=(const Manager& c);
  public:
    /// Default constructor
    Manager();
    /// Constructor taking already mapped buffers
    Manager(ServerBMID bm);
    /// Default destructor 
    virtual ~Manager();
    /// Access the server's BMID
    ServerBMID bmid() const {  return m_bm; }
    /// Map BM memory sections
    virtual int mapSections();
    /// Map BM monitoring memory sections
    virtual int mapMonitorSections();
    /// Unmap BM memory sections
    virtual int unmapSections();
    /// Release the server's BMID and give ownership to the caller
    ServerBMID releaseBMID();
    /// Parse installer options
    virtual int optparse (const char* c) = 0;
    /// Get installer options
    virtual void getOptions(int argc, char** argv);
    /// Setup manager
    virtual void setup(const char* id=0);
  };
}
#endif // _MBM_MANAGER_H
