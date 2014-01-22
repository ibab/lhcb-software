#include <algorithm>
#include "GaudiKernel/AlgFactory.h" 

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

  setFilterPassed(true);

  LHCb::L0MuonCandidates* cands = getIfExists<LHCb::L0MuonCandidates>(m_InputSelectionName);
  // FIXME/TODO: is L0MuonCandidate signed or unsigned????
  int   nCand = cands ? std::count_if( cands->begin(), cands->end()
                                     , [&](const LHCb::L0MuonCandidate* c) 
                                       { return c->pt() > m_Threshold; } )
                      : -1 ;
  if ( msgLevel(MSG::DEBUG)) {
    //TODO: count how often we failed, and print in finalize...
    if ( nCand < 0 ) debug() << m_InputSelectionName << " not found" << endmsg ;
    debug() << "There are " << nCand << " muons with Pt>" << m_Threshold << " in " << m_InputSelectionName <<  endmsg ;
  }

  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  sums->addInfo(m_Counter, nCand); // add track counter


  return StatusCode::SUCCESS;
}
