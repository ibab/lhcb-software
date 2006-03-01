// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFNetworkSendAlg.cpp,v 1.1 2006-03-01 19:20:33 frankb Exp $
//	====================================================================
//  MDFNetworkSendAlg.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================

#include "GaudiKernel/Algorithm.h"
#include "RTL/rtl.h"
#include <vector>

namespace MBM { 
  class Consumer;
  class EventDesc;
}

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MDFNetworkSendAlg MDFNetworkSendAlg.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFNetworkSendAlg : public Algorithm   {
    typedef std::vector<std::string> Requests;
    typedef std::vector<std::string> Recipients;
    MBM::Consumer*   m_cons;
    int              m_partitionID;
    std::string      m_buffer;
    std::string      m_procName;
    Recipients       m_recipients;
    Requests         m_requests;
    Recipients::iterator m_current;
    lib_rtl_thread_t m_threadHandle;
  public:
    /// Standard algorithm constructor
    MDFNetworkSendAlg(const std::string& name, ISvcLocator* pSvcLocator);
    /// Standard Destructor
    virtual ~MDFNetworkSendAlg();
    /// Initialize
    virtual StatusCode initialize();
    /// Finalize
    virtual StatusCode finalize();
    /// Execute procedure
    virtual StatusCode execute();
    /// Handle event interrupt from buffer manager
    virtual int handleEvent(const MBM::EventDesc& evt);
  };
}

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFNetworkSendAlg.cpp,v 1.1 2006-03-01 19:20:33 frankb Exp $
//	====================================================================
//  MDFNetworkSendAlg.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================

//#include "GaudiKernel/NetworkSendAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "MBM/Requirement.h"
#include "MBM/Consumer.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"
#include "RTL/rtl.h"

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFNetworkSendAlg);

using namespace MBM;


namespace {
  struct Cons : public Consumer  {
    LHCb::MDFNetworkSendAlg* m_algorithm;
    Cons(const std::string& buff,const std::string& nam, int pid, LHCb::MDFNetworkSendAlg* alg) 
    : Consumer(buff,nam,pid), m_algorithm(alg)
    {
      //setNonBlocking(WT_FACILITY_DAQ_EVENT, true);
    }
    int eventAction() {
      m_algorithm->handleEvent(event());
      return Consumer::eventAction();
    }
    static int runThread(void* param)  {
      Cons* c = (Cons*)param;
      while( c->getEvent() )  {
        c->eventAction();
      }
      return 1;
    }
  };
}

/// Standard algorithm constructor
LHCb::MDFNetworkSendAlg::MDFNetworkSendAlg(const std::string& name, ISvcLocator* pSvcLocator)
:	Algorithm(name, pSvcLocator),  m_cons(0)
{
  declareProperty("Buffer",       m_buffer = "EVENT");
  declareProperty("ProcessName",  m_procName);
  declareProperty("PartitionID",  m_partitionID = 0x103);
  declareProperty("Requirements", m_requests);
  declareProperty("Recipients",   m_recipients);
  m_current = m_recipients.end();
}

/// Standard Destructor
LHCb::MDFNetworkSendAlg::~MDFNetworkSendAlg()      {
}

/// Initialize
StatusCode LHCb::MDFNetworkSendAlg::initialize()   {
  if ( m_procName.empty() )  {
    char txt[64];
    ::lib_rtl_get_process_name(txt, sizeof(txt));
    m_procName = txt;
  }
  m_cons = new Cons(m_buffer,m_procName,m_partitionID, this);
  for(Requests::const_iterator i=m_requests.begin(); i != m_requests.end(); ++i)  {
    MBM::Requirement r;
    r.parse(*i);
    m_cons->addRequest(r);
  }
  if ( m_recipients.empty() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::FATAL << "You need to set the option recipients "
        << "to send event over the network!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_current = m_recipients.end();
  int sc = lib_rtl_start_thread(Cons::runThread,m_cons,&m_threadHandle);
  if ( !lib_rtl_is_success(sc) )  {
    MsgStream log(msgSvc(),name());
    log << MSG::FATAL << "Failed to start consumer thread." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// Finalize
StatusCode LHCb::MDFNetworkSendAlg::finalize()     {    
  if ( m_cons ) delete m_cons;
  m_cons = 0;
  m_procName = "";
  return StatusCode::SUCCESS;
}

/// Execute procedure
StatusCode LHCb::MDFNetworkSendAlg::execute()  {
  return StatusCode::SUCCESS;
}

/// Execute procedure
int LHCb::MDFNetworkSendAlg::handleEvent(const MBM::EventDesc& e)  {
  if ( m_current == m_recipients.end() )  {
    m_current = m_recipients.begin();
  }
  int sc = amsc_send_message(e.data,e.len,(*m_current).c_str(),WT_FACILITY_DAQ_EVENT,0);
  if ( AMS_SUCCESS != sc )   {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to send MDF record to " << (*m_current)
        << ". status:" << sc << ". " << endmsg;
    ++m_current;
    return handleEvent(e);
  }
  ++m_current;
  return WT_SUCCESS;
}
