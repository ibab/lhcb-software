// $Id: EvtCounter.cpp,v 1.1 2007-10-11 08:47:29 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "EvtCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtCounter
//
// 2004-06-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EvtCounter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtCounter::EvtCounter( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : base_class ( type, name , parent ),
  m_eventCounter(0){
  declareProperty("InitialCount", m_initialCount = 1,
                  "Value to be used for the first event.");
  declareProperty("UseIncident", m_useIncident = true,
                  "Whether to increment the counter at every BeginEvent incident.");
}
//=============================================================================
// Initialize method, retrieve necessary services
//=============================================================================
StatusCode EvtCounter::initialize() {

  // Mandatory initialization of GaudiTool
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return sc; }

  m_eventCounter = m_initialCount;
  if (m_useIncident) {
    // Register to the Incident service to be notified at the begin of one event
    incSvc()->addListener(this,"BeginEvent",101);
    // take into account that the increment will be called once before the first event
    --m_eventCounter;
  }

  return sc;
}

//=============================================================================
