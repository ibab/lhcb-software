// $Id: LumiCountHltTracks.cpp,v 1.3 2008-08-13 16:03:52 panmanj Exp $
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
  declareProperty( "InputSelection" ,      m_InputSelectionName);
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

  //m_inputContainer  = &(retrieveTSelection<LHCb::Track>(m_inputSelectionName));  
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

  // ------------------------------------------
  // load the track objects
  stringKey name(m_InputSelectionName);
  Hlt::Selection& m_sel = dataSvc().selection(name, this);
  int nCounter = m_sel.ncandidates();
  debug() << "There are " << nCounter << " tracks in " << m_InputSelectionName <<  endreq ;

  // ------------------------------------------
  // get (existing) container
  LHCb::HltLumiSummary* hltLS;
  if ( !exist<LHCb::HltLumiSummarys>(m_OutputContainerName) ){
    // create output container on the TES
    m_HltLumiSummarys = new LHCb::HltLumiSummarys();
    // locate them in the TES
    put(m_HltLumiSummarys, m_OutputContainerName); 
    // initialize output class
    hltLS = new LHCb::HltLumiSummary();
    m_HltLumiSummarys->insert( hltLS );
    debug() << m_OutputContainerName << " not found, made a new one" << endmsg ;
  }
  else {
    m_HltLumiSummarys = get<LHCb::HltLumiSummarys>(m_OutputContainerName);
    hltLS = *m_HltLumiSummarys->begin();
    debug() << m_OutputContainerName << " found, used the old one" << endmsg ;
  }


  // ------------------------------------------
  // add track counter
  hltLS->addInfo( m_Counter, nCounter);

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiCountHltTracks::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltBaseAlg::finalize();  // must be called after all other actions
}
