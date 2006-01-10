// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPManager.cpp,v 1.2 2006-01-10 13:56:32 frankb Exp $
//	====================================================================
//  MEPManager.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiOnline/MEPManager.h"
#include <stdexcept>

LHCb::MEPManager* s_manager = 0;
LHCb::MEPManager* manager()  {
  return s_manager;
}

LHCb::MEPManager::MEPManager(const std::string& nam, ISvcLocator* loc)    
: Service(nam, loc), m_mepID(0), m_partitionID(0x103)
{
  s_manager = this;
  declareProperty("Buffers",     m_buffers);
  declareProperty("ProcessName", m_procName);
  declareProperty("PartitionID", m_partitionID);
}

LHCb::MEPManager::~MEPManager()    {
}

StatusCode LHCb::MEPManager::error(const std::string& msg)   const {
  MsgStream err(msgSvc(), "OnlineConverter");
  err << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

/// Query interfaces of Interface
StatusCode LHCb::MEPManager::queryInterface(const InterfaceID& riid,
                                            void** ppvInterface) 
{
  if ( IID_IMEPManager.versionMatch(riid) )   {
    *ppvInterface = (IMEPManager*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvInterface);
}

StatusCode LHCb::MEPManager::initialize()  {
  typedef std::vector<std::string> _V;
  StatusCode sc = Service::initialize();
  int flags = 0;
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class RawDataCnvSvc.");
  }
  for(_V::const_iterator i=m_buffers.begin(); i != m_buffers.end(); ++i )  {
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

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPManager)
