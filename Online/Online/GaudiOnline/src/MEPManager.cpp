// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPManager.cpp,v 1.20 2008-08-11 12:02:21 frankb Exp $
//  ====================================================================
//  MEPManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
#include "GaudiOnline/MEPManager.h"
#include "RTL/rtl.h"
#include "MBM/bmdef.h"
#include "MBM/Producer.h"
#include <stdexcept>
#include <cctype>
#include <cstdio>

extern "C" int mep_install(int argc , char** argv);

/// Standard service constructor
LHCb::MEPManager::MEPManager(const std::string& nam, ISvcLocator* loc)    
: Service(nam, loc), m_partitionID(0x103), m_mepID(MEP_INV_DESC)
{
  m_procName = RTL::processName();
  declareProperty("Buffers",          m_buffers);
  declareProperty("ProcessName",      m_procName);
  declareProperty("PartitionID",      m_partitionID);
  declareProperty("PartitionName",    m_partitionName="");
  declareProperty("InitFlags",        m_initFlags);
  declareProperty("PartitionBuffers", m_partitionBuffers=false);
  declareProperty("MapUnusedBuffers", m_mapUnused=true);
}

/// Default destructor
LHCb::MEPManager::~MEPManager()    {
}

StatusCode LHCb::MEPManager::error(const std::string& msg)   const {
  MsgStream err(msgSvc(), name());
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
    char *p, *items[64], txt[32];
    std::string tmp = m_initFlags;
    for(char* tok=::strtok((char*)tmp.c_str()," "); tok; tok=::strtok(NULL," ")) {
      if ( m_partitionBuffers && ::toupper(tok[1]) == 'I' )  {
        if(strcmp(tok+3,"MEP") && strcmp(tok+3,"EVENT") && strcmp(tok+3,"RESULT")) {
          std::string bm_name = tok;
          bm_name += "_";
          if ( m_partitionName.empty() )
	    bm_name += _itoa(m_partitionID,txt,16);
	  else
	    bm_name += m_partitionName;
          items[ikey++] = strcpy(p=new char[bm_name.length()+1],bm_name.c_str());
          continue;
        }
      }
      items[ikey++] = strcpy(p=new char[strlen(tok)+1],tok);
    }
    for(size_t i=0; i<ikey; ++i)  {
      if ( ::strchr(items[i],' ') != 0 ) {
        *strchr(items[i],' ') = 0;
      }
    }
    StatusCode sc = mep_install(ikey, items);
    for(size_t j=0; j<ikey; ++j)  {
      delete [] items[j];
    }
    return sc;
  }
  return StatusCode::SUCCESS;
}

/// Connect to optional MBM buffer
StatusCode LHCb::MEPManager::connectBuffer(const std::string& nam)  {
  char txt[32];
  std::string bm_name = nam;
  if ( m_partitionBuffers ) {
    bm_name += "_";
    if ( m_partitionName.empty() )
      bm_name += _itoa(m_partitionID,txt,16);
    else
      bm_name += m_partitionName;
  }
  if( m_buffMap.find(bm_name) == m_buffMap.end() ) {
    BMID bmid = ::mbm_include(bm_name.c_str(),m_procName.c_str(),m_partitionID);
    if ( bmid == MBM_INV_DESC )  {
      return error("Failed to connect to buffer "+bm_name+" as "+m_procName);
    }
    m_bmIDs.push_back(bmid);
    m_buffMap[bm_name] = bmid;
    m_buffMap[nam] = bmid;
  }
  return StatusCode::SUCCESS;
}

/// Connect to specified buffers
StatusCode LHCb::MEPManager::connectBuffers()  {
  if ( m_buffers.size() > 0 )  {
    MsgStream log(msgSvc(), "MEPManager");
    typedef std::vector<std::string> _V;
    int flags = 0;
    for(_V::const_iterator i=m_buffers.begin(); i != m_buffers.end(); ++i )  {
      const std::string& nam = *i;
      if ( nam == "EVENT" )
        flags |= USE_EVT_BUFFER;
      else if ( nam == "RESULT" )
        flags |= USE_RES_BUFFER;
      else if ( nam == "MEP" )
        flags |= USE_MEP_BUFFER;
      else if ( !connectBuffer(nam).isSuccess() )
        return error("Failed to connect to MBM buffer:"+nam);
      else 
        log << MSG::DEBUG << "Included in MBM buffer:" << nam << endmsg;
    }
    if ( flags != 0 )  {
      log << MSG::DEBUG << "Including in MEP buffers" << endmsg;
      m_mepID = ::mep_include(m_procName.c_str(), m_partitionID, flags);
      if ( m_mepID == MEP_INV_DESC )  {
        return error("Failed to include into MEP buffers!");
      }
      log << MSG::DEBUG << " MEP    buffer start: " << (void*)m_mepID->mepStart << endmsg;
      log << MSG::DEBUG << " EVENT  buffer start: " << (void*)m_mepID->evtStart << endmsg;
      log << MSG::DEBUG << " RESULT buffer start: " << (void*)m_mepID->resStart << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MEPManager::initialize()  {
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class Service.");
  }
  m_bmIDs.clear();
  m_buffMap.clear();
  mep_map_unused_buffers(m_mapUnused);
  if ( !initializeBuffers().isSuccess() )  {
    return error("Failed to initialize MEP buffers!");
  }
  if ( !connectBuffers().isSuccess() )  {
    return error("Failed to connect to MEP buffers!");
  }
  return StatusCode::SUCCESS;
}

StatusCode LHCb::MEPManager::finalize()  {
  MsgStream log(msgSvc(), "MEPManager");
  log << MSG::INFO << "Excluding from buffers. No more buffer access possible." << endmsg;
  m_buffMap.clear();
  if ( m_mepID != MEP_INV_DESC )  {
    mep_exclude(m_mepID);
    m_mepID = MEP_INV_DESC;
  }
  for(std::vector<BMID>::iterator i=m_bmIDs.begin(); i != m_bmIDs.end(); ++i)  {
    if ( *i != MBM_INV_DESC ) ::mbm_exclude(*i);
  }
  m_bmIDs.clear();
  return Service::finalize();
}

/// Cancel connection to specified buffers
StatusCode LHCb::MEPManager::cancel()  {
  if ( m_mepID != MEP_INV_DESC ) {
    ::mep_cancel_request(m_mepID);
  }
  for(std::vector<BMID>::iterator i=m_bmIDs.begin(); i != m_bmIDs.end(); ++i)  {
    if ( *i != MBM_INV_DESC ) ::mbm_cancel_request(*i);
  }
  return StatusCode::SUCCESS;
}

/// Create producer
MBM::Producer* 
LHCb::MEPManager::createProducer(const std::string& buffer,const std::string& instance) {
  std::map<std::string,BMID>::iterator i=m_buffMap.find(buffer);
  if ( i != m_buffMap.end() ) {
    BMID bmid = (*i).second;
    return new MBM::Producer(bmid,instance,partitionID());
  }
  return 0;
}

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb,MEPManager)
