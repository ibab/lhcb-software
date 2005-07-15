// $Id: EventNodeKiller.cpp,v 1.1 2005-07-15 16:41:41 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "EventNodeKiller.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventNodeKiller
//
// 2005-07-14 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<EventNodeKiller>          s_factory ;
const        IAlgFactory& EventNodeKillerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventNodeKiller::EventNodeKiller( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Nodes", m_nodes );  
}
//=============================================================================
// Destructor
//=============================================================================
EventNodeKiller::~EventNodeKiller() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventNodeKiller::execute() {

  debug() << "==> Execute" << endmsg;

  std::vector<std::string>::iterator itS;
  for( itS=m_nodes.begin(); itS != m_nodes.end(); itS++ ) {
    debug() << "Killing node " << *itS << endmsg;
    eventSvc()->unlinkObject( *itS );
  }
  
  return StatusCode::SUCCESS;
};
//=============================================================================
