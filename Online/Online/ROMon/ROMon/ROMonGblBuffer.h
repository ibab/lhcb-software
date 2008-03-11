// $Id: ROMonGblBuffer.h,v 1.2 2008-03-11 12:37:52 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonGblBuffer.h,v 1.2 2008-03-11 12:37:52 frankb Exp $
#ifndef ROMON_ROMONGBLBUFFER_H
#define ROMON_ROMONGBLBUFFER_H 1

// C++ include files
#include <string>
// Framework include files
#include "RTL/rtl.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  /**@class ROMonGblBuffer ROMonGblBuffer.h ROMon/ROMonGblBuffer.h
   *
   *   Global section buffer to store monitoring information.
   *
   *   @author M.Frank
   */
  class ROMonGblBuffer  {
  private:
    lib_rtl_gbl_t     m_gbl;
  protected:
    int               m_delay;
    int               m_print;
    bool              m_verbose;
    bool              m_mapOnly;
    lib_rtl_lock_t    m_lock;
    std::string       m_section_name;
    size_t            m_section_size;

  public:
    /// Standard constructor
    ROMonGblBuffer();
    /// Initializing constructor
    ROMonGblBuffer(int argc , char** argv, void (*help_fun)());
    /// Default destructor
    virtual ~ROMonGblBuffer();
    /// Initialize the global section
    int initialize();
    /// Set section map-only flag
    void setMapOnly(bool val)                   {  m_mapOnly=val;           }
    /// Set section name
    void setSectionName(const std::string& val) {  m_section_name=val;      }
    /// Access section name
    const std::string& sectionName() const      {  return m_section_name;   }
    /// Set section size in KBytes
    void setSectionSize(int val)                {  m_section_size=val*1024; }
    /// Access section name in Bytes
    size_t sectionSize() const                  {  return m_section_size;   }
    /// Access to shared memory data buffer
    void* data() const;
    /// Help printout in case of -h /? or wrong arguments
    static void help();
  };  

  struct ROMonGBLData {
#ifndef _WIN32
    sem_t lock;
#endif
    char data[4];
  };

}      // End namespace ROMon
#endif /* ROMON_ROMONGBLBUFFER_H */

