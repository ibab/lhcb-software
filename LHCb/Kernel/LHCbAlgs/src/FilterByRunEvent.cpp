// $Id: FilterByRunEvent.cpp,v 1.2 2007-11-28 13:43:20 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

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

  int lcl_runNum = odin->runNumber();
  int lcl_evtNum = odin->eventNumber();

  bool lcl_sel = false;
  std::vector< std::pair<int,int> >::const_iterator i;
  for( i = m_events.begin(); i != m_events.end(); i++ )
  {
    if( lcl_runNum == (*i).first && lcl_evtNum == (*i).second )
    {
      lcl_sel = true;
      break;
    }
  }

  bool lcl_pass = (m_passSelect && lcl_sel) || (!m_passSelect && !lcl_sel);

  setFilterPassed( lcl_pass );

  if( lcl_pass )
    info() << "Passing Run " << lcl_runNum 
    		<< " event number " << lcl_evtNum << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
