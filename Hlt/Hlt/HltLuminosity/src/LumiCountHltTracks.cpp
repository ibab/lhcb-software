// $Id: LumiCountHltTracks.cpp,v 1.6 2009-01-14 21:06:50 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/Track.h"
#include "Event/HltLumiSummary.h"

#include "HltBase/ANNSvc.h"

// local
#include "LumiCountHltTracks.h"

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

  info() << "InputSelection         " << m_InputSelectionName  << endmsg;
  info() << "CounterName            " << m_CounterName         << endmsg;
  info() << "OutputContainer        " << m_OutputContainerName << endmsg;

  // ------------------------------------------
  IANNSvc* annSvc = svc<IANNSvc>("LumiANNSvc");

  boost::optional<IANNSvc::minor_value_type> 
    x = annSvc->value("LumiCounters", m_CounterName);

  if (!x) {
    warning() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    m_Counter = x->second;
    info() << "ExtraInfo key value: " << m_Counter << endmsg;
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
