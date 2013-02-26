#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/L0MuonCandidate.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "LumiCountMuons.h"
#include "boost/format.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountMuons
// Counts number of L0 muons above threshold.
// 2011-03-01 : Vladislav Balagura
// copied from LumiCountTracks
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( LumiCountMuons );

LumiCountMuons::LumiCountMuons( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {
  declareProperty( "CounterName"    , m_CounterName = "Muon" );
  declareProperty( "InputSelection" , m_InputSelectionName = LHCb::L0MuonCandidateLocation::Default);
  declareProperty( "OutputContainer", m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty( "Threshold"      , m_Threshold = 2.0);
}
LumiCountMuons::~LumiCountMuons() {} 

StatusCode LumiCountMuons::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  debug() << "CounterName: "     << boost::format("%20s")%m_CounterName         << " "
	  << "InputSelection: "  << boost::format("%20s")%m_InputSelectionName  << " "
          << "OutputContainer: " << boost::format("%20s")%m_OutputContainerName << " "
          << "Threshold: "       << boost::format("%20g")%m_Threshold           << endmsg;
  m_Counter = LHCb::LumiCounters::counterKeyToType(m_CounterName);
  if ( m_Counter == LHCb::LumiCounters::Unknown ) {
    info() << "LumiCounter not found with name: " << m_CounterName <<  endmsg;
  } else {
    debug() << m_CounterName << " value: " << m_Counter << endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode LumiCountMuons::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  int nCand = 0;
  LHCb::L0MuonCandidates* cands = getIfExists<LHCb::L0MuonCandidates>(m_InputSelectionName);
  if ( NULL == cands ) 
  {
    if (msgLevel(MSG::DEBUG)) debug() << m_InputSelectionName << " not found" << endmsg ;
  } 
  else 
  {    
    //LHCb::L0MuonCandidates* cands = get<LHCb::L0MuonCandidates>(m_InputSelectionName);
    //if ( !cands ) 
    //{ 
    //  err() << "Could not find location " <<  m_InputSelectionName << endmsg;
    //  return StatusCode::FAILURE ;
    //}
    LHCb::L0MuonCandidates::const_iterator itcand;
    for ( itcand=cands->begin(); itcand!=cands->end(); ++itcand)
      if ((*itcand)->pt() > m_Threshold) ++nCand;
  }
  if (msgLevel(MSG::DEBUG)) debug() << "There are " << nCand << " muons with Pt>" << m_Threshold << " in " << m_InputSelectionName <<  endmsg ;

  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  sums->addInfo(m_Counter, nCand); // add track counter

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode LumiCountMuons::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}

