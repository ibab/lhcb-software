#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

#include "Event/Track.h"
#include "Event/Vertex.h"
#include "Event/RecVertex.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiCounters.h"

#include "LumiCountVeloWithZRCuts.h"
#include "boost/format.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : LumiCountVeloWithZRCuts
// Counts number of L0 muons above threshold.
// 2011-03-01 : Vladislav Balagura
// copied from LumiCountMuons
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( LumiCountVeloWithZRCuts );

LumiCountVeloWithZRCuts::LumiCountVeloWithZRCuts( const std::string& name,
									      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {
  declareProperty( "TrackCounterName"    , m_TrackCounterName = "Velo"   );
  declareProperty( "VertexCounterName"   , m_VertexCounterName = "Vertex");
  declareProperty( "TrackInputSelection" , m_TrackInputSelectionName  = "TES:Hlt/Track/Lumi" );
  declareProperty( "VertexInputSelection", m_VertexInputSelectionName = "TES:Hlt/Vertex/Lumi");
  declareProperty( "OutputContainer",      m_OutputContainerName = LHCb::HltLumiSummaryLocation::Default );
  declareProperty( "AbsZCut",              m_AbsZCut = 300); // in mm
  declareProperty( "RCut",                 m_RCut    =   4); // in mm
}
LumiCountVeloWithZRCuts::~LumiCountVeloWithZRCuts() {} 

StatusCode LumiCountVeloWithZRCuts::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "TrackCounterName: "     << boost::format("%20s")%m_TrackCounterName         << " "
	  << "VertexCounterName: "    << boost::format("%20s")%m_VertexCounterName        << " "
	  << "TrackInputSelection: "  << boost::format("%20s")%m_TrackInputSelectionName  << " "
	  << "VertexInputSelection: " << boost::format("%20s")%m_VertexInputSelectionName << " "
          << "OutputContainer: "      << boost::format("%20s")%m_OutputContainerName      << " "
          << "AbsZCut: "              << boost::format("%20g")%m_AbsZCut                  << " "
          << "RCut: "                 << boost::format("%20g")%m_RCut                     << endmsg;
  m_TrackCounter  = LHCb::LumiCounters::counterKeyToType(m_TrackCounterName );
  m_VertexCounter = LHCb::LumiCounters::counterKeyToType(m_VertexCounterName);
  
  if ( m_TrackCounter == LHCb::LumiCounters::Unknown ) 
  {
    info() << "LumiCounter not found with name: " << m_TrackCounterName <<  endmsg;
  } 
  else 
  {
    if (msgLevel(MSG::DEBUG)) debug() << m_TrackCounterName << " value: " << m_TrackCounter << endmsg;
  }
  if ( m_VertexCounter == LHCb::LumiCounters::Unknown ) 
  {
    info() << "LumiCounter not found with name: " << m_VertexCounterName <<  endmsg;
  } 
  else 
  {
    if (msgLevel(MSG::DEBUG)) debug() << m_VertexCounterName << " value: " << m_VertexCounter << endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode LumiCountVeloWithZRCuts::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  int nTr = 0;
  int nVx = 0;
  
  LHCb::Tracks*      trCands = getIfExists<LHCb::Tracks>     (m_TrackInputSelectionName);
  LHCb::RecVertices* vxCands = getIfExists<LHCb::RecVertices>(m_VertexInputSelectionName);
  
  if ( NULL==trCands || NULL==vxCands ) 
  {
    if (msgLevel(MSG::DEBUG)) debug() << m_TrackInputSelectionName << " or " << m_VertexInputSelectionName << " not found" << endmsg ;
  }
  else 
  {    
    for (LHCb::Tracks::const_iterator      itr=trCands->begin(); itr!=trCands->end(); ++itr)
      if ( abs((*itr)->position().z()) < m_AbsZCut &&
	   ((*itr)->position().x() * (*itr)->position().x() + (*itr)->position().y() * (*itr)->position().y()) < m_RCut*m_RCut ) ++nTr;
    for (LHCb::RecVertices::const_iterator ivx=vxCands->begin(); ivx!=vxCands->end(); ++ivx)
      if ( abs((*ivx)->position().z()) < m_AbsZCut &&
	   ((*ivx)->position().x() * (*ivx)->position().x() + (*ivx)->position().y() * (*ivx)->position().y()) < m_RCut*m_RCut ) ++nVx;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "There are " << nTr << " VELO tracks   inside abs(z)<" << m_AbsZCut << " and R<" << m_RCut << " in " << m_TrackInputSelectionName  <<  endmsg ;
  if (msgLevel(MSG::DEBUG)) debug() << "There are " << nVx << " VELO vertices inside abs(z)<" << m_AbsZCut << " and R<" << m_RCut << " in " << m_VertexInputSelectionName <<  endmsg ;

  LHCb::HltLumiSummary* sums = getOrCreate<HltLumiSummary,HltLumiSummary>(m_OutputContainerName);
  sums->addInfo(m_TrackCounter,  nTr); // add track  counter
  sums->addInfo(m_VertexCounter, nVx); // add vertex counter

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode LumiCountVeloWithZRCuts::finalize() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  return GaudiAlgorithm::finalize();
}

