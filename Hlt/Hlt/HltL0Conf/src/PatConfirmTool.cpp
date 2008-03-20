// $Id: PatConfirmTool.cpp,v 1.5 2008-03-20 14:22:46 albrecht Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ChronoEntity.h"

// event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"

// local
#include "PatConfirmTool.h"
#include "TrackParabel.h"
#include "ParabolaHypothesis.h"
//-----------------------------------------------------------------------------
// Implementation file for class : PatConfirmTool
//
// 2007-10-25 : Johannes Albrecht
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatConfirmTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatConfirmTool::PatConfirmTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackConfirmTool>(this);
 
  //variables to be defined in job options
  declareProperty("nSigma",m_nsigma=5.);
  declareProperty("debugMode", m_debugMode = false);
  declareProperty("minHitsInOT", m_minHitsInOT = 14 );
}
//=============================================================================
// Destructor
//=============================================================================
PatConfirmTool::~PatConfirmTool() {} 

//=============================================================================

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatConfirmTool::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return sc;
  }
  if( msgLevel(MSG::DEBUG) ) 
    debug() << " Initialize L0 confirmation tool using PatSeeding" << endmsg;
   
  m_tHitManager    = tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");

  m_patSeedingTool  = tool<IPatSeedingTool>("PatSeedingTool");

  if(m_debugMode){
    //tool for debug information
    m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  }

  m_l0ConfExtrapolator = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");
  
  return StatusCode::SUCCESS;

}

//=============================================================================
StatusCode PatConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{
  if ( msgLevel(MSG::DEBUG) ) debug()<<"--> execute"<<endmsg;
  ChronoEntity tDecoding, tTracking;
  if (m_debugMode) tDecoding.start();

  // Define track hypothesis from seed state (parabola model) and
  // decode IT and OT hits in search window
  ParabolaHypothesis tp = m_l0ConfExtrapolator->getParabolaHypothesis( seedState, m_nsigma );
  m_tHitManager->prepareHitsInWindow(tp);

  if (m_debugMode) {
    tDecoding.stop();
    debug()<<"--> write dec time"<<endmsg;
    m_DataStore->nTHits.push_back( m_tHitManager->hits().size() ) ;
    m_DataStore->decodingTime.push_back( tDecoding.eTotalTime() );
    m_DataStore->trackHypo.push_back(new ParabolaHypothesis(tp));
    debug() << " number of decoded hits: " << m_tHitManager->hits().size() << endreq;
    tTracking.start();
  }

  unsigned minHits = m_minHitsInOT;
  // work out if we're likely to hit the IT - if so, we're satisfied with
  // half the number of hits
  double x = seedState.x() + seedState.tx() *
	  (StateParameters::ZBegT - seedState.z());
  double y = seedState.y() + seedState.ty() *
	  (StateParameters::ZBegT - seedState.z());
  if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
  else {
    x += seedState.tx() * (StateParameters::ZMidT - StateParameters::ZBegT);
    y += seedState.ty() * (StateParameters::ZMidT - StateParameters::ZBegT);
    if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
    else {
      x += seedState.tx() * (StateParameters::ZEndT - StateParameters::ZMidT);
      y += seedState.ty() * (StateParameters::ZEndT - StateParameters::ZMidT);
      if (std::abs(x) < 700. && std::abs(y) < 250.) minHits /= 2;
    }
  }

  if ( m_tHitManager->hits().size() >= minHits ) {
    m_patSeedingTool->prepareHits();
    // modify the covariance matrix, as PatSeedingTool will search in the one
    // sigma range per default
    LHCb::State state(seedState);
    state.covariance() *= m_nsigma * m_nsigma;
    m_patSeedingTool->performTracking(outputTracks, &state);
  }

  if (m_debugMode) {
    tTracking.stop();
    m_DataStore->trackingTime.push_back( tTracking.eTotalTime() );    
    if (msgLevel(MSG::DEBUG) ) {
      debug() << "tTracking.eTotalTime() "
              << tTracking.eTotalTime() + tDecoding.eTotalTime() << endreq;
      debug() << "tracks found sofar in PatSearch Tool: " << outputTracks.size() << endreq;
    }
  }

  m_tHitManager->clearHits();
  
  return StatusCode::SUCCESS;
}

