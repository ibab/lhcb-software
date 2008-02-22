// $Id: NodeCounter.cpp,v 1.1.1.1 2008-02-22 15:06:06 ocallot Exp $
// Include files

// local
#include "NodeCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : NodeCounter
//
// 2007-11-15 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NodeCounter::NodeCounter( std::string& name, bool isNode ) {
  m_name    = name;
  if ( isNode ) {
    std::string service;  
    service  = m_name+"_MEPRx_1/Runable/totMEPReq";
    m_mepRqd = new DimInfo( service.c_str(), 2, -1 );
    service  = m_name+"_MEPRx_1/Runable/totRxPkt";
    m_mepRcv = new DimInfo( service.c_str(), 2, -1 );
    service  = m_name+"_MEPRx_1/Runable/incEvt";
    m_incomp = new DimInfo( service.c_str(), 2, -1 );
    service  = m_name+"_EvtProd_1/Runable/EventsOut";
    m_evtPro = new DimInfo( service.c_str(), 2, -1 );
    service  = m_name+"_DiskWR_1/Runable/EvtCount";
    m_evtWri = new DimInfo( service.c_str(), 2, -1 );
  } else {
    m_mepRqd = 0;
    m_mepRcv = 0;
    m_incomp = 0;
    m_evtPro = 0;
    m_evtWri = 0;
  }
}
//=============================================================================
// Destructor
//=============================================================================
NodeCounter::~NodeCounter() {}
//=========================================================================
//  Update if the node is non virtual
//=========================================================================
void NodeCounter::update ( ) {
  if ( 0 != m_mepRqd ) {
    m_rqd = m_mepRqd->getInt();
    m_rcv = m_mepRcv->getInt();
    m_inc = m_incomp->getInt();
    m_pro = m_evtPro->getInt();
    m_wri = m_evtWri->getInt();
  }
}
//=========================================================================
//  Produce the output line
//=========================================================================
std::string NodeCounter::statusLine ( ) {
  char line[200];  
  sprintf( line, "%12s %12d %12d %12d  %12d %12d",
           m_name.c_str(), m_rqd, m_rcv, m_inc, m_pro, m_wri );
  return std::string(line);
}
//=============================================================================
