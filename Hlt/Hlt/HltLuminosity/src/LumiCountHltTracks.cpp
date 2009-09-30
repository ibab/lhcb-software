// $Id: LumiCountHltTracks.cpp,v 1.9 2009-09-30 07:24:56 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/Track.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "HltBase/HltSelection.h"

// local
#include "LumiCountHltTracks.h"

#include "boost/format.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountHltTracks
//
// 2008-07-22 : Jaap panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiCountHltTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiCountHltTracks::LumiCountHltTracks( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  declareProperty( "InputSelection" ,      m_InputSelectionName.property());
  declareProperty( "CounterName"    ,      m_CounterName);
  declareProperty( "OutputContainer",      m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );

};
//=============================================================================
// Destructor
//=============================================================================
LumiCountHltTracks::~LumiCountHltTracks() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode LumiCountHltTracks::initialize() {

  StatusCode sc = HltBaseAlg::initialize();
  if (sc.isFailure()) return sc;

  debug() <<  "InputSelection: " << boost::format("%20s")%m_InputSelectionName  << " "
         <<     "CounterName: " << boost::format("%20s")%m_CounterName         << " "
         << "OutputContainer: " << boost::format("%20s")%m_OutputContainerName << endmsg;

  // ------------------------------------------
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    debug() << m_CounterName << " key value: " << m_Counter << endmsg;
  }
  // ------------------------------------------


  if (sc.isFailure()) return sc;

  return sc;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiCountHltTracks::execute() {

  // load the track objects
  int nCounter =  dataSvc().selection(m_InputSelectionName, this)->size();
  debug() << "There are " << nCounter << " tracks in " << m_InputSelectionName <<  endreq ;

  // get container
  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  // add track counter
  sums->addInfo( m_Counter, nCounter);

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}
