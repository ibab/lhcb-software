// $Id: EvtCounter.cpp,v 1.1 2007-10-11 08:47:29 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/IIncidentSvc.h"

// local
#include "EvtCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtCounter
//
// 2004-06-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EvtCounter );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtCounter::EvtCounter( const std::string& type,
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
StatusCode EvtCounter::initialize() {

  // Mandatory initialization of GaudiTool
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return sc; }

  // Register to the Incident service to be notified at the begin of one event
  incSvc()->addListener(this,"BeginEvent",101);

  return sc;
}

//=============================================================================
