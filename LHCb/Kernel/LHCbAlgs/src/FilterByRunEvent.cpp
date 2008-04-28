// $Id: FilterByRunEvent.cpp,v 1.4 2008-04-28 11:43:47 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/ODIN.h"

// local
#include "FilterByRunEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterByRunEvent
//
// 2007-01-18 : Patrick Spradlin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FilterByRunEvent );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterByRunEvent::FilterByRunEvent( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "RunEventNumList", m_events );
  declareProperty( "PassSelectedEvents", m_passSelect = true );
}
//=============================================================================
// Destructor
//=============================================================================
FilterByRunEvent::~FilterByRunEvent() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterByRunEvent::execute()
{

  debug() << "==> Execute" << endmsg;

  // code goes here  
  setFilterPassed(!m_passSelect);

  // Get the run and event number from the ODIN bank
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );

  // TODO: it might be advantageous to sort m_events (and add a call-back for when
  //       it is updated!), and use a binary search instead...
  bool lcl_sel = ( std::find(m_events.begin(),
                             m_events.end(), 
                             std::make_pair<int,int>(odin->runNumber(),odin->eventNumber())) 
                             // FIXME: delete the <int,int> part in the above... once m_events is of the right type...
                   != m_events.end() );

  bool lcl_pass = ( m_passSelect ? lcl_sel : !lcl_sel );

  setFilterPassed( lcl_pass );

  if( lcl_pass )
    info() << "Passing Run "   << odin->runNumber() 
           << " event number " << odin->eventNumber() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
