// $Id: PatConfirmTool.cpp,v 1.14 2009-10-29 09:04:25 pkoppenb Exp $
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
#include "HltBase/ParabolaHypothesis.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatConfirmTool
//
// 2007-10-25 : Johannes Albrecht
//-----------------------------------------------------------------------------

using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PatConfirmTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatConfirmTool::PatConfirmTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_DataStore(0)
{
  declareInterface<ITrackConfirmTool>(this);
  //  declareInterface<ITrackView>(this);

  //variables to be defined in job options
  declareProperty("nSigmaX",m_nSigmaX=5.);
  declareProperty("nSigmaY",m_nSigmaY=5.);
  declareProperty("nSigmaTx",m_nSigmaTx=5.);
  declareProperty("nSigmaTy",m_nSigmaTy=5.);
  declareProperty("debugMode", m_debugMode = false);
  declareProperty("restrictSearch",m_restrictSearch=true);
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
  if (sc.isFailure()) return sc;
  if( msgLevel(MSG::DEBUG) )  {
    debug() << " Initialize L0 confirmation tool using PatSeeding" << endmsg;
  }
   
  m_tHitManager    = tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");

  m_patSeedingTool  = tool<IPatSeedingTool>("PatSeedingTool",this);

  if(m_debugMode){ //tool for debug information
    m_DataStore = tool<L0ConfDataStore>("L0ConfDataStore");
  }

  m_l0ConfExtrapolator = tool<IL0ConfExtrapolator>("L0ConfExtrapolator");

  if ( msgLevel(MSG::DEBUG) ) debug()<<"PatConfirmTool configured with the following search windows: "<<endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug()<<"nSigma:  X = "<<m_nSigmaX<<"  , y = "<<m_nSigmaY
         <<" ,  Tx = "<<m_nSigmaTx<<"  , Ty = "<<m_nSigmaTy<<endmsg;
  
  return sc;

}

//=============================================================================
StatusCode PatConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{
  if ( msgLevel(MSG::DEBUG) ) debug()<<"--> execute"<<endmsg;
  StatusCode sc;
  
  ChronoEntity tDecoding, tTracking;

  std::vector<Track*> tmpTracks ;
  
  if (m_debugMode) tDecoding.start();

  // Define track hypothesis from seed state (parabola model) and
  // decode IT and OT hits in search window
  ParabolaHypothesis tp = prepareT( seedState );  
    
  if (m_debugMode) {
    tDecoding.stop();
    if ( msgLevel(MSG::DEBUG) ) debug()<<"--> write dec time"<<endmsg;
    m_DataStore->nTHits.push_back( m_tHitManager->hits().size() ) ;
    m_DataStore->decodingTime.push_back( tDecoding.eTotalTime() );
    m_DataStore->trackHypo.push_back(new ParabolaHypothesis(tp));
    if ( msgLevel(MSG::DEBUG) ) debug() << " number of decoded hits: " << m_tHitManager->hits().size() << endmsg;
    tTracking.start();
  }

  unsigned minHits = m_minHitsInOT;
  // work out if we're likely to hit the IT - if so, we're satisfied with
  // half the number of hits
  double x = seedState.x() + seedState.tx() * (StateParameters::ZBegT - seedState.z());
  double y = seedState.y() + seedState.ty() * (StateParameters::ZBegT - seedState.z());
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
  
    if( m_restrictSearch ){
      
      // modify the covariance matrix, as PatSeedingTool will search in the one
      // sigma range per default
      LHCb::State state(seedState);
      Gaudi::TrackSymMatrix stateCov = state.covariance();
      stateCov(0,0) *= m_nSigmaX * m_nSigmaX;
      stateCov(1,1) *= m_nSigmaY * m_nSigmaY;
      stateCov(2,2) *= m_nSigmaTx * m_nSigmaTx;
      stateCov(3,3) *= m_nSigmaTy * m_nSigmaTy;
      state.setCovariance(stateCov);
      sc=m_patSeedingTool->performTracking(tmpTracks, &state);
    }
    else{
      sc=m_patSeedingTool->performTracking(tmpTracks );
    }
    
    if(sc.isFailure())
      if (msgLevel(MSG::DEBUG) ) debug() << "seeding failed!!"<<endmsg;
  }
    
  if (m_debugMode) {
    tTracking.stop();
    m_DataStore->trackingTime.push_back( tTracking.eTotalTime() );    
    if (msgLevel(MSG::DEBUG) ) {
      debug() << "tTracking.eTotalTime() "
              << tTracking.eTotalTime() + tDecoding.eTotalTime() << endmsg;
      debug() << "tracks found sofar in PatSearch Tool: " << tmpTracks.size() << endmsg;
    }
  }
  
  for( std::vector<LHCb::Track*>::iterator it1 = tmpTracks.begin(); 
       it1 != tmpTracks.end() ; 
       ++it1 ) {
    
    bool saveTrack = true;
    
    for( LHCb::Tracks::iterator it2=outputTracks.begin();
         it2!=outputTracks.end(); ++it2 ){
      
      if( isClone(*(*it1) , *(*it2) ) ) saveTrack = false;
    }
    if( saveTrack ){
      outputTracks.push_back( (*it1)->clone() );
      if(msgLevel(MSG::DEBUG))
        debug()<<"keep track with pt(T): "<<(*it1)->pt() <<endmsg;
    }
    delete  *it1;
  }
  tmpTracks.clear();
  
  m_tHitManager->clearHits();
  
  return sc;
}

ParabolaHypothesis PatConfirmTool::prepareT( const LHCb::State& seedState )
{
  ParabolaHypothesis tp = m_l0ConfExtrapolator->getParabolaHypothesis( seedState, m_nSigmaX , m_nSigmaY );
  m_tHitManager->prepareHitsInWindow(tp);
  return tp;
}


ParabolaHypothesis PatConfirmTool::prepareT( const LHCb::State& seedState ,std::vector<LHCb::LHCbID>& ids)
{

  //prepare hits in T-stations
  ParabolaHypothesis tp = prepareT( seedState );
  Tf::TStationHitManager<PatForwardHit>::HitRange hits = m_tHitManager->hits();
  
  //get lhcbids from hits and fill output
  Tf::TStationHitManager<PatForwardHit>::HitRange::const_iterator it;
  for ( it = hits.begin();it != hits.end();++it){
    PatForwardHit* myHit = (*it);
    LHCb::LHCbID id = myHit->hit()->lhcbID();
    ids.push_back(id);
  }
  return tp;
}

/*
 *  This small helper seaches for a track found twice
 *  Faster than comparin id's is to check position in 
 *  x to agree within 10 mu in T1 and T3
 */
bool PatConfirmTool::isClone(const LHCb::Track& t1 , const LHCb::Track& t2)
{
  if( fabs( t1.closestState(7800.).x()-t2.closestState(7800.).x() ) > 0.01 ) return false;
  if( fabs( t1.closestState(9300.).x()-t2.closestState(9300.).x() ) > 0.01 ) return false;
  return true;
}

