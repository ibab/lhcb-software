#ifndef _MBM_MANAGER_H
#define _MBM_MANAGER_H

// Include files
#include "RTL/rtl.h"
typedef struct ServerBMID_t* ServerBMID;

/*
 *    MBM Namespace declaration
 */
namespace MBM {

  /* @class MBM::Manager  Manager.h  MBM/Manager.h
   *
   * @author  M.Frank
   * @version 1.0
   * @date    10/01/2006
   */
  struct Manager  {
    /// BM identifier
    ServerBMID    m_bm;
    /// Global section module name for the control sections
    char          ctrl_mod[32];
    /// Global section module name for the buffer data
    char          buff_mod[32];
    /// Buffer id
    const   char* bm_id;
    /// Global section handle to buffer inventory
    lib_rtl_gbl_t bm_all;

  public:
    /// Default constructor
    Manager();
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
