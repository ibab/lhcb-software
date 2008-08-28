//  ====================================================================
//  MEPInjector.cpp
//  --------------------------------------------------------------------
//
//  Author    : David Svantesson
//
//  ====================================================================

// Include files from Gaudi
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiOnline/MEPRxSys.h"
#include "GaudiOnline/MEPHdr.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "MDF/OnlineRunInfo.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPWriter.h"
#include "MDF/MEPEvent.h"
#include "Event/RawEvent.h"
#include "GaudiKernel/AlgFactory.h"
//#include "MDF/MEPMultiFragment.h"
#include <MDF/MEPFragment.h>
#include "GaudiOnline/MEPRxSvc.h"

//#include "GaudiUtils/IIODataManager.h"
//#include "MDF/RawDataConnection.h"
#include <arpa/inet.h>


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "GaudiOnline/MEPInjector.h"

#ifdef _WIN32
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __FUNCDNAME__ << ":" << __LINE__ << endmsg;} while(0);
#else
#define ERRMSG(a,x) do {  \
  a << MSG::ERROR << x << " " << MEPRxSys::sys_err_msg() << " in " << __PRETTY_FUNCTION__<< ":"  << __FILE__<<  ":(" << __LINE__ << ")" << endmsg;} while(0);
#endif

DECLARE_NAMESPACE_ALGORITHM_FACTORY(LHCb,MEPInjector)

using namespace LHCb;

static void* extendBuffer(void* p, size_t len)   {
  StreamBuffer* s = (StreamBuffer*)p;
  s->reserve(len);
  return s->data();
}

MEPInjector::MEPInjector( const std::string& name, ISvcLocator* pSvcLocator )
  : Algorithm(name, pSvcLocator)
{
  declareProperty("RemoveDAQStatus",  m_removeDAQStatus = true);    // To remove bank header (we want this)
  declareProperty("PackingFactor",    m_packingFactor = 2);        // How many events should be packed in one MEP? 
  
  declareProperty("IPProtoIn",        m_IPProtoIn = 0xF2);
  
  declareProperty("IPAddrTo",         m_IPAddrTo = "0.0.0.0");
  declareProperty("IPAddrFrom",       m_IPAddrFrom);
  
  declareProperty("sockBuf",          m_sockBuf = 0x100000);
  declareProperty("ethInterface",     m_ethInterface);
  
  /*
  m_probEventID = 0;//2147483647;
  m_probPartID = 2147483647;
  m_probSize = 0;//2147483647;
  std::srand ( time(NULL) );
  */
    
}

/// Initialize
StatusCode MEPInjector::initialize() { 
  MsgStream log(msgSvc(),name()); //Message stream for output
  
  if (m_IPProtoIn < 0 || m_IPProtoIn > 255)
    return error("IPProtoIn is an unsigned 8 bit quantity");
  
  std::srand ( std::time(NULL) );
  
  std::string errmsg;  
  //if ((m_mepSock = MEPRxSys::open_sock(m_IPProtoIn, m_sockBuf, m_ethInterface, "0.0.0.0", 0, errmsg))<0) 
  //  return error("Failed to open socket:" + errmsg);
  //log << MSG::DEBUG << "Get mep socket: " << m_mepSock << endmsg;
 	
  if ((m_mepSock = MEPRxSys::open_sock_arb_source(m_IPProtoIn, m_sockBuf, errmsg)) < 0)
    return error("Failed to open socket:" + errmsg);

  m_evID = 1;
  return StatusCode::SUCCESS;
}
    
/// Main execution
StatusCode MEPInjector::execute() {
  MsgStream log(msgSvc(),name()); //Message stream for output
  size_t nevts = abs(m_packingFactor)>15 ? 15 : abs(m_packingFactor); // Max packingfactor is 15s
  SmartDataPtr<RawEvent> raw(eventSvc(),RawEventLocation::Default); // Open the file
  if ( raw )  {
    //This If-statement adds one RawEvent to the vector and if the packing factor is reached, it is sent.
  
    log << MSG::DEBUG << "NEW MDF" << endmsg;
    //return StatusCode::FAILURE;

    // Need to remove all MDF header banks first
    typedef std::vector<RawBank*> _V;
    if ( m_removeDAQStatus ) {
      _V bnks = raw->banks(RawBank::DAQ);
      for(_V::iterator ib=bnks.begin(); ib != bnks.end(); ++ib)  {
        log << MSG::DEBUG << "bank, TELL1 board: " << (*ib)->sourceID() << endmsg;
        if ( (*ib)->version() == DAQ_STATUS_BANK )
          raw->removeBank(*ib);
      }
    }      
        
    raw->addRef(); //add reference to object
    log << MSG::INFO << "Add event number: " << m_evID << endmsg;
    m_events[m_evID] = raw; //Add raw event to map
    if ( m_events.size() == nevts )  {
      //Have reached packing factor
      sendMEP(); //Create and send the MEP
      std::map<unsigned int, RawEvent*>::iterator it;
      for(it = m_events.begin(); it != m_events.end(); ++it)  {
         it->second->release(); //delete reference to object
      }
      m_events.clear();
    }
    ++m_evID;
    return StatusCode::SUCCESS;
  }
  log << MSG::ERROR << "Failed to access " << RawEventLocation::Default << endmsg;
  return StatusCode::FAILURE;
}

StatusCode MEPInjector::sendMEP() {
  MsgStream log(msgSvc(),name());  
  
  MEPEvent *me = NULL;
  struct MEPHdr *mh = NULL;
    
  StatusCode encM = encodeMEP(m_events, 0x1234, &m_data, extendBuffer, &me);
  
  if ( encM == StatusCode::FAILURE )
    return error("Error while creating MEP");
  if (false == checkMEPEvent(me, true, true )) 
    return error("Consistency check for MEP failed");
  
  mh = (struct MEPHdr *) &(((u_int8_t *) me->start())[20]);  // The MEP-header starts after the 20 byte IP-header
  int mepsize = mh->m_totLen;
  
  //printMEP(me, mepsize);
  log << MSG::INFO << "MEP of size " << mepsize << " successfully created. Sending." << endmsg;
    
  const char* addrfrom = m_IPAddrFrom[(rand() % (int)m_IPAddrFrom.size())].c_str();
        // rand() is not very random, but it is enough for us.
  
  int n = MEPRxSys::send_msg_arb_source(m_mepSock,m_IPProtoIn, inet_addr(addrfrom) ,inet_addr(m_IPAddrTo.c_str()),mh, mepsize );
  if (n == (int) mh->m_totLen)   {
    log << MSG::INFO << "MEP successfully sent!" << endmsg;
    return StatusCode::SUCCESS;
  }
  if (n == -1)  {
    ERRMSG(log,"MEP sending failed: ");
    return StatusCode::FAILURE;
  }
  return error("MEP corrupted on send! Sent length:" + n);
    
}

/** This function prints out the contents of a MEP.
 *
 *  ( The output is printed in hexadecimal numbers with 16 bytes on each row )
 */
void MEPInjector::printMEP(MEPEvent *me, int size) {
  //int size = me->size();
  const char* data = me->start();
  
  for (int i=0; i< (size+20); i++) {
    if ((i % 16)==0) {
      std::cout << std::endl;
      std::printf("%08X :",i);
      
    }
    std::printf(" %02X",(unsigned char) data[i]);
  }
  std::cout << std::endl;
}

StatusCode MEPInjector::error(const std::string& msg) {
  MsgStream log(msgSvc(),name());
  log << MSG::ERROR << msg << endmsg;
  return StatusCode::FAILURE;
}
