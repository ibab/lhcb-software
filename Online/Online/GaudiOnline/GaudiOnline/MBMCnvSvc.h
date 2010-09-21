// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/MBMCnvSvc.h,v 1.6 2010-09-21 14:28:24 frankb Exp $
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
  class RawBank;
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
    int  m_partitionID;
  public:
    /// Initializing constructor
    MBMCnvSvc(const std::string& nam, ISvcLocator* loc);

    /// Initializing constructor for inheritance
    MBMCnvSvc(const std::string& nam, ISvcLocator* loc, long type);

    /// Standard destructor      
    virtual ~MBMCnvSvc();

    /// Allocate data space for output
    virtual std::pair<char*,int> getDataSpace(void* const ioDesc, size_t len);

    /// Declare event to data space
    virtual StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len);

    /// Open MBM buffer manager
    virtual void* openIO(const std::string& fname, const std::string&  mode) const;

    /// Close MBM buffer manager
    virtual StatusCode closeIO(void* ioDesc) const;

    /// Read raw banks
    virtual StatusCode readRawBanks(void* ioDesc, std::vector<RawBank*>& banks, long long& offset);

    /// RawDataCnvSvc overload: Read raw banks
    virtual StatusCode readRawBanks(RawDataAddress* pAddr,MDFDescriptor& data)
    {  return this->RawDataCnvSvc::readRawBanks(pAddr,data);                      }

  };
}      // End namespace LHCb
#endif // GAUDIONLINE_MBMCNVSVC_H
