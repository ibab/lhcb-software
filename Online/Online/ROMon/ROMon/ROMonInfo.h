// $Id: ROMonInfo.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/ROMonInfo.h,v 1.1 2008-02-01 17:41:45 frankm Exp $
#ifndef ROMON_ROMONINFO_H
#define ROMON_ROMONINFO_H 1

// C++ include files
#include <string>

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  // Forward declarations
  class RODimListener;

  /**@class ROMonInfo ROMonInfo.h ROMon/ROMonInfo.h
   *
   *   DIM info handler to detect new/dead services
   *
   *   @author M.Frank
   */
  class ROMonInfo  {
  protected:
    /// DIM info identifier
    unsigned int      m_info;
    /// Reference to parent/owner object
    RODimListener*    m_server;

    /// Extract node/service name from DNS info
    void getServiceNode(char* s, std::string& svc, std::string& node)  const;
    /// Add handler for a given message source
    void addHandler(const std::string& node, const std::string& svc);
    /// Remove handler for a given message source
    void removeHandler(const std::string& node, const std::string& svc);
    /// DimInfo overload to process messages
    static void infoHandler(void* tag, void* address, int* size);

  public:

    /// Standard constructor
    ROMonInfo(RODimListener* s);
    /// Standard destructor
    virtual ~ROMonInfo();
  };
}      // End namespace ROMon
#endif /* ROMON_ROMONINFO_H */

