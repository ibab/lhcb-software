// $Id: FilterByRunEvent.cpp,v 1.1 2007-11-23 23:28:03 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

#include "Event/RecHeader.h"

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
  : DVAlgorithm ( name , pSvcLocator )
{
  declareProperty( "RunNumList",   m_runnum );
  declareProperty( "EventNumList", m_evnum );
  declareProperty( "PassSelectedEvents", m_passSelect = true );
}
//=============================================================================
// Destructor
//=============================================================================
FilterByRunEvent::~FilterByRunEvent() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterByRunEvent::initialize()
{
  //=== The following two lines should be commented for DC04 algorithms ! ===
  // StatusCode sc = DVAlgorithm::initialize(); 
  // if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  if( m_runnum.size() != m_evnum.size() )
  {
    err() << "Vector of run and event numbers different lengths: "
    	  << m_runnum.size() << "/" << m_evnum.size() << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterByRunEvent::execute()
{

  debug() << "==> Execute" << endmsg;

  // code goes here  
  setFilterPassed(!m_passSelect);

  // Get the EventHeader
  LHCb::RecHeader* evthdr = get<LHCb::RecHeader>(LHCb::RecHeaderLocation::Default);
  //SmartDataPtr<RecHeader> evthdr( eventSvc(), EventHeaderLocation::Default );
  if ( !evthdr )
  {
    err() << "Not able to retrieve event header" << endreq;
    return StatusCode::FAILURE;
  }
  long lcl_runNum = evthdr->runNumber();
  long lcl_evtNum = evthdr->evtNumber();

  bool lcl_sel = false;
  std::vector<long>::const_iterator i;
  for( i = m_runnum.begin(); i != m_runnum.end(); i++ )
  {
    std::vector<long>::const_iterator j = m_evnum.begin() 
    						+ (i - m_runnum.begin());
    if( lcl_runNum == *i && lcl_evtNum == *j )
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
//  Finalize
//=============================================================================
StatusCode FilterByRunEvent::finalize()
{

  debug() << "==> Finalize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
