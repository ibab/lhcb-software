// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPManager.cpp,v 1.25 2010-09-21 14:28:25 frankb Exp $
//  ====================================================================
//  MEPManager.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/xtoa.h"
#include "GaudiOnline/MEPManager.h"
#include "RTL/rtl.h"
#include "MBM/bmdef.h"
#include "MBM/Producer.h"
#include "MBM/Consumer.h"
#include "MBM/Requirement.h"
#include <stdexcept>
#include <cctype>
#include <cstdio>

using namespace std;
using namespace LHCb;

/// Standard service constructor
MEPManager::MEPManager(const string& nam, ISvcLocator* loc)
  : Service(nam, loc), m_partitionID(0x103)
{
  m_procName = RTL::processName();
  declareProperty("Buffers",              m_buffers);
  declareProperty("PartitionID",          m_partitionID);
  declareProperty("PartitionName",        m_partitionName="");
  declareProperty("InitFlags",            m_initFlags);
  declareProperty("PartitionBuffers",     m_partitionBuffers=false);
  declareProperty("MapUnusedBuffers",     m_mapUnused=true);
  declareProperty("HandleSignals",        m_handleSignals=false);
  declareProperty("ConnectWhen",          m_connectWhen="initialize");
  declareProperty("ConsumerRequirements", m_consRequirements);
  declareProperty("Communication",        m_communication="FIFO");
}

/// Default destructor
MEPManager::~MEPManager()    {
}

StatusCode MEPManager::error(const string& msg)   const {
  MsgStream err(msgSvc(), name());
  err << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}

/// Query interfaces of Interface
StatusCode MEPManager::queryInterface(const InterfaceID& riid,
                                      void** ppvInterface) 
{
  if ( IMEPManager::interfaceID().versionMatch(riid) )   {
    *ppvInterface = (IMEPManager*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvInterface);
}

/// Initialize buffers for MEP usage
StatusCode MEPManager::initializeBuffers()  {
  MsgStream log(msgSvc(), name());
  if ( !m_initFlags.empty() )  {
    size_t ikey = 0;
    char *items[64], txt[32];
    string tmp = m_initFlags;
    for(char* tok=::strtok((char*)tmp.c_str()," "); tok; tok=::strtok(NULL," ")) {
      if ( m_partitionBuffers && ::toupper(tok[1]) == 'I' )  {
        string bm_name = tok;
        bm_name += "_";
        if ( m_partitionName.empty() )
          bm_name += _itoa(m_partitionID,txt,16);
        else
          bm_name += m_partitionName;
        items[ikey++] = strcpy(new char[bm_name.length()+1],bm_name.c_str());
        continue;
      }
      items[ikey++] = strcpy(new char[strlen(tok)+1],tok);
    }
    for(size_t i=0; i<ikey; ++i)  {
      if ( ::strchr(items[i],' ') != 0 ) {
        *strchr(items[i],' ') = 0;
      }
    }
    m_srvBMIDs = mbm_multi_install(ikey, items);
    for(size_t j=0; j<ikey; ++j)
      delete [] items[j];
    if ( m_srvBMIDs.empty() ) {
      return error("Failed to initialize MBM buffers.");
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::SUCCESS;
}

/// Apply single consumer reuirement to buffer
StatusCode MEPManager::setConsumerRequirement(ServerBMID srvBM, const string& task, const MBM::Requirement& r)  {
  int status = mbmsrv_require_consumer(srvBM, task.c_str(), m_partitionID, r.evtype, r.trmask);
  if ( status != MBM_NORMAL )
    return error("Failed to set BM consumer requirements");
  return StatusCode::SUCCESS;
}

/// Apply consumer requirements to MBM buffers
StatusCode MEPManager::setConsumerRequirements()   {
  MsgStream log(msgSvc(), name());
  for(auto i=begin(m_consRequirements); i!=end(m_consRequirements);++i)  {    
    const string& buf = bufferName((*i).first);
    ServedBuffers::const_iterator j=m_srvBMIDs.find(buf);
    if ( j == m_srvBMIDs.end() )  {
      return error("Buffer "+buf+" does not exist. Failed to set consumer requirement.");
    }
    else if ( (*i).second.size() < 2 )  {
      return error("Insufficient information from job-option. Failed to set consumer requirement.");
    }
    else if ( (((*i).second.size())%2) != 0 )  {
      return error("Inconsistent job-option. Failed to set consumer requirement.");
    }
    for(size_t k=0; k<(*i).second.size(); k += 2)  {
      const string& tsk = (*i).second[k];
      const string& req = (*i).second[k+1];
      MBM::Requirement r;
      r.parse(req);
      log << MSG::INFO << "Setting consumer requirement "
          << "[" << buf << "] Task:" << tsk << " REQ:" << req << endmsg;
      StatusCode sc = setConsumerRequirement((*j).second, tsk, r);
      if ( !sc.isSuccess() )   {
        return error("Failed to set consumer requirement: "+req);
      }
    }
  }
  return StatusCode::SUCCESS;
}

/// Create proper buffer name depending on partitioning
std::string MEPManager::bufferName(const std::string& nam)  const   {
  string bm_name = nam;
  if ( m_partitionBuffers ) {
    char txt[64];
    bm_name += "_";
    if ( m_partitionName.empty() )
      bm_name += _itoa(m_partitionID,txt,16);
    else
      bm_name += m_partitionName;
  }
  return bm_name;
}

/// Connect to optional MBM buffer
StatusCode MEPManager::connectBuffer(const string& nam)  {
  string bm_name = bufferName(nam);
  if( m_buffMap.find(bm_name) == m_buffMap.end() ) {
    BMID bmid = MBM_INV_DESC;
    if ( m_communication == "FIFO" )
      bmid = ::mbm_include(bm_name.c_str(),m_procName.c_str(),m_partitionID,BM_COM_FIFO);
    else if ( m_communication == "ASIO" )
      bmid = ::mbm_include(bm_name.c_str(),m_procName.c_str(),m_partitionID,BM_COM_ASIO);

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
StatusCode MEPManager::connectBuffers()  {
  if ( m_buffers.size() > 0 )  {
    MsgStream log(msgSvc(), name());
    typedef vector<string> _V;
    for(_V::const_iterator i=m_buffers.begin(); i != m_buffers.end(); ++i )  {
      const string& nam = *i;
      if ( !connectBuffer(nam).isSuccess() )
        return error("Failed to connect to MBM buffer:"+nam);
      else 
        log << MSG::DEBUG << "Included in MBM buffer:" << nam << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MEPManager::initialize()  {
  StatusCode sc = Service::initialize();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class Service.");
  }
  return (m_connectWhen == "initialize") ? i_init() : sc;
}

StatusCode MEPManager::start()  {
  StatusCode sc = Service::start();
  if ( !sc.isSuccess() )  {
    return error("Failed to initialize base class Service.");
  }
  return (m_connectWhen == "start") ? i_init() : sc;
}

StatusCode MEPManager::restart()  {
  return StatusCode::SUCCESS;
}

StatusCode MEPManager::i_init()  {
  MsgStream log(msgSvc(), name());
  setProperties();
  m_procName = RTL::processName();
  log << MSG::INFO << m_connectWhen << "> Process name:" << m_procName
      << " Connecting to MBM Buffers." << endmsg;
  m_bmIDs.clear();
  m_buffMap.clear();
  if ( !initializeBuffers().isSuccess() )  {
    return error("Failed to initialize MBM buffers!");
  }
  if ( !setConsumerRequirements().isSuccess() )  {
    return error("Failed to apply consumer requirements to MBM buffers!");
  }
  if ( !connectBuffers().isSuccess() )  {
    return error("Failed to connect to MBM buffers!");
  }
  return StatusCode::SUCCESS;
}

StatusCode MEPManager::finalize()  {
  if (m_connectWhen == "initialize") i_fini();
  return Service::finalize();
}

StatusCode MEPManager::stop()  {
  if (m_connectWhen == "start") i_fini();
  return Service::finalize();
}

StatusCode MEPManager::i_fini()  {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "Excluding from buffers. No more buffer access possible." << endmsg;
  m_buffMap.clear();
  for(vector<BMID>::iterator i=m_bmIDs.begin(); i != m_bmIDs.end(); ++i)  {
    if ( *i != MBM_INV_DESC ) ::mbm_exclude(*i);
  }
  m_bmIDs.clear();
  return StatusCode::SUCCESS;
}

/// Cancel connection to specified buffers
StatusCode MEPManager::cancel()  {
  for(vector<BMID>::iterator i=m_bmIDs.begin(); i != m_bmIDs.end(); ++i)  {
    if ( *i != MBM_INV_DESC ) ::mbm_cancel_request(*i);
  }
  return StatusCode::SUCCESS;
}

/// Create producer
MBM::Producer* MEPManager::createProducer(const string& buffer,const string& instance) {
  map<string,BMID>::iterator i=m_buffMap.find(buffer);
  if ( i == m_buffMap.end() ) {
    string bm_name = bufferName(buffer);
    i = m_buffMap.find(bm_name);
  }
  if ( i != m_buffMap.end() ) {
    BMID bmid = (*i).second;
    return new MBM::Producer(bmid,instance,partitionID());
  }
  return 0;
}

/// Create consumer attached to a specified buffer
MBM::Consumer* MEPManager::createConsumer(const string& buffer,const string& instance) {
  map<string,BMID>::iterator i=m_buffMap.find(buffer);
  if ( i == m_buffMap.end() ) {
    string bm_name = bufferName(buffer);
    i = m_buffMap.find(bm_name);
  }
  if ( i != m_buffMap.end() ) {
    BMID bmid = (*i).second;
    return new MBM::Consumer(bmid,instance,partitionID());
  }
  return 0;
}
