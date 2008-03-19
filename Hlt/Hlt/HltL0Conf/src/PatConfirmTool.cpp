// $Id: PatConfirmTool.cpp,v 1.3 2008-03-19 10:00:24 smenzeme Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ChronoEntity.h"

// event
#include "Event/Track.h"
#include "Event/State.h"

// local
#include "PatConfirmTool.h"
#include "TrackParabel.h"

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
  declareProperty("nSigma",m_nsigma=5);
  declareProperty("debugMode", m_debugMode = false);
  
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
  
  return StatusCode::SUCCESS;

}


//=============================================================================
StatusCode PatConfirmTool::tracks(const LHCb::State& seedState, std::vector<Track*>& outputTracks ) 
{
  ChronoEntity tDecoding, tTracking;
  if(m_debugMode) tDecoding.start();
  
  /*
   *    Define track hypothesis from seed state (parabola model) and 
   *    decode IT and OT hits in search window
   */
  TrackParabel tp(seedState, m_nsigma); 
  m_tHitManager->prepareHitsInWindow(tp);

  if( msgLevel(MSG::DEBUG) ) 
    debug()<<" number of decoded hits: "<<m_tHitManager->hits().size()<<endmsg;

  /*
   *    Prepare (sort) hits for PatSeeding tool and perform track search on
   *    seelcted hits
   */
  if(m_debugMode) {
    tDecoding.stop();
    tTracking.start();
  }
 
  m_patSeedingTool->prepareHits();
  m_patSeedingTool->performTracking(outputTracks);

  if( msgLevel(MSG::DEBUG) ) 
    debug()<<"tracks found sofar in PatSearch Tool: "<<outputTracks.size()<<endmsg;
  
  if(m_debugMode) {
    tTracking.stop();
    info()<<"Pat: #decoded hits"<<m_tHitManager->hits().size() <<endmsg;
    
    m_DataStore->nTHits.push_back( m_tHitManager->hits().size() ) ;
    m_DataStore->decodingTime.push_back( tDecoding.eTotalTime() );
    m_DataStore->trackingTime.push_back( tTracking.eTotalTime() );    
    info()<<"tTracking.eTotalTime() "<< tTracking.eTotalTime()+tDecoding.eTotalTime()<<endmsg;
    
  }

  m_tHitManager->clearHits();
 
  return StatusCode::SUCCESS;
  
}

