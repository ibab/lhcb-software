// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPCnvSvc.cpp,v 1.5 2006-04-03 17:59:21 frankb Exp $
//	====================================================================
//  MEPCnvSvc.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/strcasecmp.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiOnline/MBMCnvSvc.h"
#include "GaudiOnline/MEPManager.h"
#include "MDF/StorageTypes.h"
#include "MDF/MDFHeader.h"
#include "MDF/RawEventHelpers.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include <stdexcept>

using LHCb::MDFHeader;
using MBM::Producer;


namespace LHCb {

  /** @class MEPCnvSvc MEPCnvSvc.cpp  GaudiOnline/MEPCnvSvc.cpp
    *
    * Conversion service for the online data.
    * 
    * @author  M.Frank
    * @version 1.0
    * @date    01/01/2005
    */
  class MEPCnvSvc : public MBMCnvSvc  {
    
    /// Pointer to MEP manager service
    IMEPManager*   m_mepMgr;

  public:
    /// Initializing constructor
    MEPCnvSvc(const std::string& nam, ISvcLocator* loc);
    
    /// Standard destructor      
    virtual ~MEPCnvSvc();

    /// Initialize conversion service
    virtual StatusCode initialize();

    /// Finalize conversion service
    virtual StatusCode finalize();

    /// Commit output to buffer manager
    virtual StatusCode commitDescriptors(void* ioDesc);
    
    /// Open MBM buffer manager
    void* openIO(const std::string& fname, const std::string&  mode) const;

    /// Declare event to data space
    StatusCode writeDataSpace(void* ioDesc,
                              size_t len, 
                              longlong trNumber, 
                              unsigned int trMask[4],
                              int evType, int hdrType);
  };
}

/// Initializing constructor
LHCb::MEPCnvSvc::MEPCnvSvc(const std::string& nam, ISvcLocator* loc) 
: MBMCnvSvc(nam, loc, RAWDESC_StorageType), m_mepMgr(0)
{
}
  
/// Standard destructor      
LHCb::MEPCnvSvc::~MEPCnvSvc() {
}

/// Initialize MEP conversion service
StatusCode LHCb::MEPCnvSvc::initialize()  {
  StatusCode sc = MBMCnvSvc::initialize();
  if ( sc.isSuccess() )  {
    sc = service("MEPManager",m_mepMgr);
    if ( !sc.isSuccess() )   {
      error("Failed to access MEPManager.");
    }
  }
  return sc;
}

StatusCode LHCb::MEPCnvSvc::finalize()  {
  if ( m_mepMgr )  {
    m_mepMgr->release();
    m_mepMgr = 0;
  }
  return MBMCnvSvc::finalize();
}

/// Commit output to buffer manager
StatusCode LHCb::MEPCnvSvc::commitDescriptors(void* /* ioDesc */)  {
  SmartDataPtr<DataObject> evt(dataProvider(),"/Event");
  if ( evt )  {
    IOpaqueAddress* pA = evt->registry()->address();
    if ( pA->svcType() == RAWDESC_StorageType )  {
#if 0
      const MultiEventPacket* dsc = (const MultiEventPacket*)pA->ipar()[0];
      size_t len = dsc->header()->sizeOf();
      char* data = getDataSpace(ioDesc, len);
      if ( data )  {
        unsigned int trMask[4] = { 0x103, 0, 0, 0 };
        int evType = 1;
        int hdrType = 0;
        longlong trNumber = 0;
        ::memcpy(data-sizeof(MDFHeader), dsc, len);
        return writeDataSpace(ioDesc, len, trNumber, trMask, evType, hdrType);
      }
      return error("Failed to get space from MBM buffer.");
#endif
    }
    return error("Bad IO channel type.");
  }
  return error("Cannot access \"/Event\" object");
}
  
/// Open MBM buffer manager
void* LHCb::MEPCnvSvc::openIO(const std::string& fname, const std::string&  mode) const {
  if ( strncasecmp(mode.c_str(),"N",1)==0 || strncasecmp(mode.c_str(),"REC",3)==0 )  {
    // Writing: requires producer
    if ( fname.find("mep://") == 0 )  {
      size_t id1 = fname.find(".",6);
      std::string buff = fname.substr(6, id1-6);
      MEPID mepID = m_mepMgr->mepID();
      BMID  bmID = MBM_INV_DESC;
      int offset = 0;
      switch(::toupper(buff[0]))  {
        case 'E':
          bmID   = mepID->evtBuffer;
          offset = mepID->evtStart;
          break;
        case 'R':
          bmID   = mepID->resBuffer;
          offset = mepID->resStart;
          break;
        case 'M':
          bmID   = mepID->mepBuffer;
          offset = mepID->mepStart;
          break;
        default:
          error("Unknown MEP buffer name:"+buff);
          return 0;
      }
      Producer* p = new Producer(bmID,mepID->processName,mepID->partitionID);
      return p;
    }
  }
  else if ( strncasecmp(mode.c_str(),"O",1)==0 || strncasecmp(mode.c_str(),"REA",3)==0 )  {
    // Reading: requires consumer
  }
  return 0;
}

/// Declare event to data space
StatusCode LHCb::MEPCnvSvc::writeDataSpace(void* ioDesc,
                                           size_t len, 
                                           longlong trNumber, 
                                           unsigned int trMask[4],
                                           int evType, 
                                           int hdrType)    {
  if ( ioDesc )   {
    Producer* prod = (Producer*)ioDesc;
    if ( prod )  {
      MBM::EventDesc& e = prod->event();
      makeMDFHeader(e.data, len, evType, hdrType, trNumber, trMask, 0, 0);
      e.type    = evType;
      e.mask[0] = trMask[0];
      e.mask[1] = trMask[1];
      e.mask[2] = trMask[2];
      e.mask[3] = trMask[3];
      e.len     = len+sizeof(MDFHeader);
      if ( prod->sendEvent() == MBM_NORMAL )  {
        return StatusCode::SUCCESS;
      }
    }
  }
  return StatusCode::FAILURE;
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPCnvSvc)

