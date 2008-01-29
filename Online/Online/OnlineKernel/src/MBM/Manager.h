#ifndef _MBM_MANAGER_H
#define _MBM_MANAGER_H

// Include files
#include "RTL/rtl.h"
struct BMDESCRIPT;

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
    BMDESCRIPT* m_bm;
    char ctrl_mod[32];       // Control section name
    char buff_mod[32];       // Buffer section name
    char user_mod[32];       // Userdata section name
    char event_mod[32];      // Event data section name
    char bitmap_mod[32];     // Bitmap section name 
    const   char* bm_id;     // Buffer id
    lib_rtl_gbl_t bm_all;
  public:
    /// Default constructor
    Manager();
    /// Default destructor 
    virtual ~Manager();
    /// Map BM memory sections
    virtual int mapSections();
    /// Map BM monitoring memory sections
    virtual int mapMonitorSections();
    /// Unmap BM memory sections
    virtual int unmapSections();
    /// Parse installer options
    virtual int optparse (const char* c) = 0;
    /// Get installer options
    virtual void getOptions(int argc, char** argv);
    /// Setup manager
    virtual void setup(const char* id=0);
  };
}
#endif // _MBM_MANAGER_H
