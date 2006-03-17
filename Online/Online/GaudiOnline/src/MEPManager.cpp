// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPManager.cpp,v 1.6 2006-03-17 07:33:06 frankb Exp $
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
#include "RTL/rtl.h"
#include <stdexcept>
#include <cctype>
#include <cstdio>

extern "C" int mep_install(int argc , char** argv);

/// Standard service constructor
LHCb::MEPManager::MEPManager(const std::string& nam, ISvcLocator* loc)    
: Service(nam, loc), m_mepID(MEP_INV_DESC), m_partitionID(0x103)
{
  declareProperty("Buffers",     m_buffers);
  declareProperty("ProcessName", m_procName);
  declareProperty("PartitionID", m_partitionID);
  declareProperty("InitFlags",   m_initFlags);
}

/// Default destructor
LHCb::MEPManager::~MEPManager()    {
}

StatusCode LHCb::MEPManager::error(const std::string& msg)   const {
  MsgStream err(msgSvc(), "MEPManager");
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

/// Initialize buffers for MEP usage
StatusCode LHCb::MEPManager::initializeBuffers()  {
  MsgStream log(msgSvc(), name());
  if ( !m_initFlags.empty() )  {
    size_t ikey = 0;
    char* items[64];
    std::string tmp = m_initFlags;
    for(char* tok=::strtok((char*)tmp.c_str()," "); tok; tok=::strtok(NULL," ")) {
      items[ikey++] = tok;
    }
    for(size_t i=0; i<ikey; ++i)  {
      if ( ::strchr(items[i],' ') != 0 ) {
        *strchr(items[i],' ') = 0;
      }
    }
    return mep_install(ikey, items);
  }
  return StatusCode::SUCCESS;
}

/// Connect to specified buffers
StatusCode LHCb::MEPManager::connectBuffers()  {
  MsgStream log(msgSvc(), "MEPManager");
  typedef std::vector<std::string> _V;
  if ( m_buffers.size() > 0 )  {
    int flags = 0;
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
    log << MSG::ALWAYS << "Including in MEP buffers" << endmsg;
    m_mepID = mep_include(m_procName.c_str(), m_partitionID, flags);
    if ( m_mepID == MEP_INV_DESC )  {
      return error("Failed to include into MEP buffers!");
    }
    log << MSG::DEBUG << " MEP    buffer start: " << (void*)m_mepID->mepStart << endmsg;
    log << MSG::DEBUG << " EVENT  buffer start: " << (void*)m_mepID->evtStart << endmsg;
    log << MSG::DEBUG << " RESULT buffer start: " << (void*)m_mepID->resStart << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MEPManager::initialize()  {
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class RawDataCnvSvc.");
  }
  if ( m_procName.empty() )  {
    char txt[64];
    ::lib_rtl_get_process_name(txt, sizeof(txt));
    m_procName = txt;
  }
  if ( !initializeBuffers().isSuccess() )  {
    return error("Failed to initialize MEP buffers!");
  }
  if ( !connectBuffers().isSuccess() )  {
    return error("Failed to connect to MEP buffers!");
  }
  return StatusCode::SUCCESS;
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPManager)
