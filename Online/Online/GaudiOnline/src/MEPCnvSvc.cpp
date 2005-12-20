// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPCnvSvc.cpp,v 1.1 2005-12-20 16:38:18 frankb Exp $
//	====================================================================
//  RawBufferCreator.cpp
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
#include "MDF/RawDataCnvSvc.h"
#include "MDF/StorageTypes.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventHelpers.h"
#include "MBM/Producer.h"
#include "MBM/mepdef.h"
#include "Event/RawEvent.h"
#include <stdexcept>

using LHCb::RawEvent;
using LHCb::MDFHeader;
using LHCb::MultiEventPacket;

namespace LHCb {
class MepManager : public Service  {
  MEPID                     m_mepID;
  std::vector<std::string>  m_buffers;
  std::string               m_procName;
  int                       m_partitionID;
public:
  MepManager(const std::string& nam, ISvcLocator* loc);
  virtual ~MepManager();
  MEPID  mepID() const  {  return m_mepID;   }
  StatusCode initialize();
  StatusCode error(const std::string& msg)   const;
};
}

LHCb::MepManager* s_manager = 0;
LHCb::MepManager* manager()  {
  return s_manager;
}

LHCb::MepManager::MepManager(const std::string& nam, ISvcLocator* loc)    
: Service(nam, loc), m_mepID(0), m_partitionID(0x103)
{
  s_manager = this;
  declareProperty("Buffers",     m_buffers);
  declareProperty("ProcessName", m_procName);
  declareProperty("PartitionID", m_partitionID);
}

LHCb::MepManager::~MepManager()    {
}

StatusCode LHCb::MepManager::error(const std::string& msg)   const {
  MsgStream err(msgSvc(), "OnlineConverter");
  err << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

StatusCode LHCb::MepManager::initialize()  {
  typedef std::vector<std::string> _V;
  StatusCode sc = Service::initialize();
  int flags = 0;
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class RawDataCnvSvc.");
  }
  for(_V::const_iterator i=m_buffers.begin(); i != m_buffers.end(); ++i )  {
    const std::string& b = *i;
    switch(::toupper((*i)[0]))  {
      case 'E':
        flags |= USE_EVT_BUFFER;
        break;
      case 'R':
        flags |= USE_RES_BUFFER;
        break;
      case 'M':
        flags |= USE_MEP_BUFFER;
        break;
      default:
        return error("Unknown buffer name:"+(*i));
    }
  }
  m_mepID = mep_include(m_procName.c_str(), m_partitionID, flags);
  if ( m_mepID == MEP_INV_DESC )  {
    return error("Failed to include into MEP buffers!");
  }
  return StatusCode::SUCCESS;
}

namespace LHCb {
  /** @class OnlineCnvSvc OnlineCnvSvc.cpp  GaudiOnline/OnlineCnvSvc.cpp
    *
    * Conversion service for the online data.
    * 
    * @author  M.Frank
    * @version 1.0
    * @date    01/01/2005
    */
  class MEPCnvSvc : public MBMCnvSvc  {
  public:
    /// Initializing constructor
    MEPCnvSvc(const std::string& nam, ISvcLocator* loc);
    
    /// Standard destructor      
    virtual ~MEPCnvSvc();

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
: MBMCnvSvc(nam, loc, MBMDESC_StorageType)
{
}
  
/// Standard destructor      
LHCb::MEPCnvSvc::~MEPCnvSvc() {
}

/// Commit output to buffer manager
StatusCode LHCb::MEPCnvSvc::commitDescriptors(void* ioDesc)  {
  SmartDataPtr<DataObject> evt(dataProvider(),"/Event");
  if ( evt )  {
    IOpaqueAddress* pA = evt->registry()->address();
    if ( pA->svcType() == MBMDESC_StorageType )  {
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
      int partID;
      size_t id1 = fname.find(".",6);
      size_t id2 = fname.find(".0x",id1+1);
      std::string buff = fname.substr(6, id1-6);
      std::string proc = fname.substr(6, id2-id1);
      ::sscanf(fname.c_str()+id2+1,"0x%X",&partID);
      MEPID mepID = manager()->mepID();
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
      MBM::Producer* p = new MBM::Producer(bmID,mepID->processName,mepID->partitionID);
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
    MBM::Producer* prod = (MBM::Producer*)ioDesc;
    if ( prod )  {
      MBM::EventDesc& e = prod->event();
      MDFHeader* h = (MDFHeader*)e.data;
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
DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MepManager)
