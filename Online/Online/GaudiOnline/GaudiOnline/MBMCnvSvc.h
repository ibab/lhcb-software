// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MBMCnvSvc.h,v 1.2 2006-03-17 07:33:06 frankb Exp $
//	====================================================================
//  MBMCnvSvc.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef GAUDIONLINE_MBMCNVSVC_H
#define GAUDIONLINE_MBMCNVSVC_H

#include "MDF/RawDataCnvSvc.h"
#include "MBM/bmdef.h"

namespace MBM  {  class Producer;  }

/*
 *    LHCb namespace
 */
namespace LHCb    {

  // Forward declarations  
  class RawEvent;
  class MEPFragment;

  /** @class MBMCnvSvc MBMCnvSvc.cpp  GaudiOnline/MBMCnvSvc.cpp
  *
  * Conversion service for the online data.
  * 
  * @author  M.Frank
  * @version 1.0
  * @date    01/01/2005
  */
  class MBMCnvSvc : public RawDataCnvSvc  {
  public:
    /// Initializing constructor
    MBMCnvSvc(const std::string& nam, ISvcLocator* loc);

    /// Initializing constructor for inheritance
    MBMCnvSvc(const std::string& nam, ISvcLocator* loc, long type);

    /// Standard destructor      
    virtual ~MBMCnvSvc();

    /// Allocate data space for output
    virtual char* const getDataSpace(void* ioDesc, size_t len);

    /// Declare event to data space
    virtual StatusCode writeDataSpace(void* ioDesc,
                                      size_t len, 
                                      longlong trNumber, 
                                      unsigned int trMask[4],
                                      int evType, 
                                      int hdrType);

    /// Open MBM buffer manager
    virtual void* openIO(const std::string& fname, const std::string&  mode) const;

    /// Close MBM buffer manager
    virtual StatusCode closeIO(void* ioDesc) const;

    /// Read raw banks
    virtual StatusCode readRawBanks(void* ioDesc, std::vector<RawBank*>& banks, long long& offset);
  };
}      // End namespace LHCb
#endif // GAUDIONLINE_MBMCNVSVC_H
