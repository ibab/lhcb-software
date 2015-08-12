// $Id$
// Include files 

#include "Event/ODIN.h" // event & run number
#include "GaudiKernel/IRegistry.h" // IOpaqueAddress
// local

#include "FilterEventList.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterEventList
//
// 2004-11-18 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory 
DECLARE_ALGORITHM_FACTORY( FilterEventList )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterEventList::FilterEventList( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_list(0)
  , m_pass(true)
{
  declareProperty( "EventRunList", m_list=EventRunList(0) , "Set a vector of pairs of [EventNumber, RunNumber]");
  declareProperty( "SetFilterPassed", m_pass=true , "What to do with events in this list? Pass=true? or Fail=false?");
}
//=============================================================================
// Destructor
//=============================================================================
FilterEventList::~FilterEventList() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FilterEventList::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  info() <<  "Looking for " << m_list.size() << " Events "
	 << "starting with (" << m_list[0].first << "," << m_list[0].second 
	 << ")" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FilterEventList::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  
  int run = -1;
  int ev = -1;

  const LHCb::ODIN * odin = getIfExists<LHCb::ODIN>( LHCb::ODINLocation::Default );

  if ( odin )
  {
    run = odin->runNumber();
    ev = odin->eventNumber();
    if (msgLevel(MSG::VERBOSE)) verbose() << "Event,run (" << ev << "," << run << ")" << endmsg;
  }

  if( run <0 || ev < 0)
  {
    Error("Can't get LHCb::ODINLocation::Default (" +
          LHCb::ODINLocation::Default + ")" );
  }
  
  EventRunPair eR(ev,run);
  
  if(std::find(m_list.begin(), m_list.end(), eR) != m_list.end())
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Event,run *is* in list (" << ev << "," << run << ")" << endmsg;
    ++counter("EventsInList");
    setFilterPassed(m_pass);
    counter("EventsSelected")+=m_pass;
    
  }
  else
  {
    counter("EventsInList")+=0;
    setFilterPassed(!m_pass);
    counter("EventsSelected")+=!m_pass;
  }
  
  //setFilterPassed(std::find(m_list.begin(), m_list.end(), eR) != m_list.end());
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FilterEventList::finalize() 
{

  debug() << "==> Finalize" << endmsg;
  debug() << counter("EventsInList").flag() << " Events found " 
	 << "from the list of " << m_list.size() << " Events, having read " 
	 << counter("EventsInList").nEntries() << "Events" << endmsg;
  

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
