// $Id: StoreCounter.cpp,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
// Include files 



// local
#include "StoreCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StoreCounter
//
// 2007-12-05 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StoreCounter::StoreCounter(  std::string& name, std::string& address  ) {
  m_name    = name;
  m_address = address;
  if ( "" != name ) {
    std::string service;  
    service  = "LHCb_"+m_name+"_RCV"+address+"/Receiver/EventsIn";
    m_evtRcv = new DimInfo( service.c_str(), 2, -1 );
    service  = "LHCb_"+m_name+"_WRT"+address+"/Runable/EvtCount";
    m_evtWri = new DimInfo( service.c_str(), 2, -1 );
  } else {
    m_evtRcv = 0;
    m_evtWri = 0;
  }
  std::string serviceName =  "StoreMonitor/" + address + "/Status";
  char* format="I:2";
  m_service = new DimService( serviceName.c_str(), format, (void*) &m_dimBuffer, 40 );
}
//=============================================================================
// Destructor
//=============================================================================
StoreCounter::~StoreCounter() {
  delete m_evtRcv;
  delete m_evtWri;
  delete m_service;
}
//=========================================================================
//  
//=========================================================================
void StoreCounter::update ( ) {
  if ( 0 != m_evtRcv ) {
    m_rcv = m_evtRcv->getInt();
    m_wri = m_evtWri->getInt();
  }
  
  //== update the Dim service
  bool mustUpdate = false;
  if ( m_dimBuffer.rcv != m_rcv ) {
    mustUpdate = true;
    m_dimBuffer.rcv = m_rcv;
  }
  if ( m_dimBuffer.wri != m_wri ) {
    mustUpdate = true;
    m_dimBuffer.wri = m_wri;
  }
  
  if ( mustUpdate ) m_service->updateService(); 
}
//=========================================================================
//  Produce the output line
//=========================================================================
std::string StoreCounter::statusLine ( ) {
  char line[200];  
  sprintf( line, "%12s %12d %12d",
           m_address.c_str(), m_rcv, m_wri );
  return std::string(line);
}
//=============================================================================
