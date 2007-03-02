// $Id: EventCounter.cpp,v 1.1 2007-03-02 08:49:26 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "EventCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventCounter
//
// 2004-06-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EventCounter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventCounter::EventCounter( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent ),
  m_eventCounter(0){
  declareInterface<IEventCounter>(this);
  return;
  
}
//=============================================================================
// Initialize method, retrieve necessary services 
//=============================================================================
StatusCode EventCounter::initialize() {

  // Mandatory initialization of GaudiTool
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return sc; }

  // Register to the Incident service to be notified at the begin of one event
  incSvc()->addListener(this,"BeginEvent",101);

  return sc;
}

//=============================================================================
