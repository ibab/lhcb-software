// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPFragmentSender.cpp,v 1.3 2006-04-24 14:45:05 frankb Exp $
//	====================================================================
//  MEPFragmentSender.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StreamBuffer.h"
#include <vector>

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /**@class MEPFragmentSender MEPFragmentSender.cpp
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MEPFragmentSender : public Algorithm  {
    typedef std::vector<std::string> Recipients;
    /// Property: Send target names
    Recipients             m_recipients;
    /// Property: printout frequence
    float                  m_freq;
    /// Round robing sending: current send target
    Recipients::iterator   m_current;
    /// Streambuffer to hold uncompressed data
    StreamBuffer           m_data;
    /// Counter of number of events sent
    unsigned long long int m_evtSent;
    /// Counter of number of bytes sent
    unsigned long long int m_bytesSent;
  public:
    /// Standard algorithm constructor
    MEPFragmentSender(const std::string& name, ISvcLocator* pSvcLocator);
    /// Standard Destructor
    virtual ~MEPFragmentSender();
    /// Initialize
    virtual StatusCode initialize();
    /// Finalize
    virtual StatusCode finalize();
    /// Execute procedure
    virtual StatusCode execute();
  };
}

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MEPFragmentSender.cpp,v 1.3 2006-04-24 14:45:05 frankb Exp $
//	====================================================================
//  MEPFragmentSender.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================

//#include "GaudiKernel/MEPFragmentSender.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/MsgStream.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/RawDataAddress.h"
#include "MDF/MDFHeader.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

#include <cmath>

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MEPFragmentSender);

/// Standard algorithm constructor
LHCb::MEPFragmentSender::MEPFragmentSender(const std::string& name, ISvcLocator* pSvcLocator)
:	Algorithm(name, pSvcLocator), m_evtSent(0), m_bytesSent(0)
{
  declareProperty("PrintFreq",   m_freq = 0.);
  declareProperty("Recipients",  m_recipients);
  m_current = m_recipients.end();
  m_data.reserve(1024*64);        // 64 kB initial space (for 35 kB average event sized)
}

/// Standard Destructor
LHCb::MEPFragmentSender::~MEPFragmentSender()      {
}

/// Initialize
StatusCode LHCb::MEPFragmentSender::initialize()   {
  if ( m_recipients.empty() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::FATAL << "You need to set the option recipients "
        << "to send event over the network!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_current = m_recipients.end();
  return StatusCode::SUCCESS;
}

/// Finalize
StatusCode LHCb::MEPFragmentSender::finalize()     {    
  return StatusCode::SUCCESS;
}

/// Execute procedure
StatusCode LHCb::MEPFragmentSender::execute()  {
  SmartDataPtr<DataObject> evt(eventSvc(),"/Event");
  if ( evt )  {
    IRegistry* reg = evt->registry();
    if ( reg )   {
      RawDataAddress* addr = dynamic_cast<RawDataAddress*>(reg->address());
      if ( addr )  {
        int hdrType = 0;
        // unsigned int  partID = addr->partitionID();
        unsigned char evtTyp = addr->eventType();
        unsigned int  trNumb = 0;
        const unsigned int* trMask = addr->triggerMask();
        const std::vector<RawBank*>* banks = addr->banks();
        size_t total_len, len = rawEventLength(*banks)+sizeof(MDFHeader);
        ulonglong prtCount = fabs(m_freq) > 1./ULONGLONG_MAX ? ulonglong(1./m_freq) : ULONGLONG_MAX;

        m_data.reserve(len);
        encodeRawBanks(*banks, m_data.data()+sizeof(MDFHeader),len, &total_len);
        makeMDFHeader(m_data.data(), total_len, evtTyp, hdrType, trNumb, trMask, 0, 0);
        /// Now send the data ....
        if ( m_current == m_recipients.end() )  {
          m_current = m_recipients.begin();
        }
        const std::string& target = *m_current;
        int sc = amsc_send_message( m_data.data(),
                                    total_len+sizeof(MDFHeader),
                                    target.c_str(),
                                    WT_FACILITY_DAQ_EVENT,0);
        if ( AMS_SUCCESS != sc )   {
          MsgStream log(msgSvc(),name());
          log << MSG::ERROR << "Failed to send MDF record to " << (*m_current)
              << ". status:" << sc << ". " << endmsg;
          return StatusCode::FAILURE;
        }
        m_bytesSent += total_len + sizeof(MDFHeader);
        ++m_evtSent;
        ++m_current;
        if ( 0 == (m_evtSent%prtCount) )  {
          MsgStream log(msgSvc(),name());
          log << MSG::INFO << "Sent " << m_evtSent 
              << " Events with a total of " << m_bytesSent << " Bytes." << endmsg;
        }
        return StatusCode::SUCCESS;
      }
    }
    MsgStream log0(msgSvc(),name());
    log0 << MSG::ERROR << "Invalid data address of /Event" << endmsg;
    return StatusCode::FAILURE;
  }
  MsgStream log1(msgSvc(),name());
  log1 << MSG::ERROR << "Cannot access top level event object" << endmsg;
  return StatusCode::FAILURE;
}
