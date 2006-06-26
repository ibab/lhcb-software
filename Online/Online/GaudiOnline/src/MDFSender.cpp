// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/src/MDFSender.cpp,v 1.1 2006-06-26 08:45:15 frankb Exp $
//	====================================================================
//  MDFSender.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "GaudiOnline/MDFSender.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "AMS/amsdef.h"
#include "WT/wtdef.h"

#include <cmath>

#include "GaudiKernel/AlgFactory.h"
DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MDFSender);

/// Standard algorithm constructor
LHCb::MDFSender::MDFSender(const std::string& nam, ISvcLocator* pSvc)
:	MDFWriter(MDFIO::MDF_BANKS, nam, pSvc), m_evtSent(0), m_bytesSent(0)
{
  declareProperty("PrintFreq",   m_freq = 0.);
  declareProperty("Recipients",  m_recipients);
  m_current = m_recipients.end();
}

/// Standard Destructor
LHCb::MDFSender::~MDFSender()      {
}

/// Initialize
StatusCode LHCb::MDFSender::initialize()   {
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
StatusCode LHCb::MDFSender::finalize()     {    
  return StatusCode::FAILURE;
}

/// Write MDF record from serialization buffer
StatusCode LHCb::MDFSender::writeBuffer(void* const /* ioDesc */, const void* data, size_t len)  {
  longlong prtCount = fabs(m_freq) > 1./LONGLONG_MAX ? longlong(1./m_freq) : LONGLONG_MAX;
  /// Now send the data ....
  if ( m_current == m_recipients.end() )  {
    m_current = m_recipients.begin();
  }
  if ( m_current == m_recipients.end() )  {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to send MDF record [No known recipient]." << endmsg;
    return StatusCode::FAILURE;
  }
  const std::string& tar = *m_current;
  int sc = amsc_send_message( data, len, tar.c_str(), WT_FACILITY_DAQ_EVENT, 0);
  if ( AMS_SUCCESS != sc )   {
    MsgStream log(msgSvc(),name());
    log << MSG::ERROR << "Failed to send MDF to " << tar << ". status:" << sc << ". " << endmsg;
    return StatusCode::FAILURE;
  }
  m_bytesSent += len;
  ++m_evtSent;
  ++m_current;
  if ( 0 == (m_evtSent%prtCount) )  {
    MsgStream log(msgSvc(),name());
    log << MSG::INFO << "Sent " << m_evtSent 
        << " Events with a total of " << m_bytesSent << " Bytes." << endmsg;
  }
  return StatusCode::SUCCESS;
}
