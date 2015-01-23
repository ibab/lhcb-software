// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "PatVeloTTHybrid.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatVeloTTHybrid
//
// 2007-05-08 : Mariusz Witek
//-----------------------------------------------------------------------------


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatVeloTTHybrid )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatVeloTTHybrid::PatVeloTTHybrid( const std::string& name,
                      ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_ttHitManager(nullptr),
  m_tracksFitter(nullptr),
  m_veloTTTool(nullptr),
  m_timerTool(nullptr),
  m_veloTTTime(0)
{
  if ( "Hlt" == context() ) {
    m_inputTracksName =  "";
    m_outputTracksName = "";
  }else {
    m_inputTracksName =  LHCb::TrackLocation::Velo;
    m_outputTracksName = LHCb::TrackLocation::VeloTT;
  } 

  declareProperty("InputTracksName"       , m_inputTracksName                   );
  declareProperty("OutputTracksName"      , m_outputTracksName                  );
  declareProperty("fitTracks"             , m_fitTracks = false                 );
  declareProperty("Fitter"                , m_fitterName = "TrackMasterFitter"  );
  declareProperty("maxChi2"               , m_maxChi2          = 1280.          ); 
  declareProperty("TimingMeasurement"     , m_doTiming = false                  );
    
}
//=============================================================================
// Destructor
//=============================================================================
PatVeloTTHybrid::~PatVeloTTHybrid() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatVeloTTHybrid::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_veloTTTool = tool<ITracksFromTrack>("PatVeloTTHybridTool", "PatVeloTTHybridTool");

  m_tracksFitter = tool<ITrackFitter>( m_fitterName, "Fitter", this );

  m_ttHitManager   = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  info() << " InputTracksName    = " << m_inputTracksName            << endmsg;
  info() << " OutputTracksName   = " << m_outputTracksName           << endmsg;

  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_veloTTTime = m_timerTool->addTimer( "Internal VeloTTHybrid Tracking" );
    m_timerTool->decreaseIndent();
  }

         
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatVeloTTHybrid::execute() {

  if ( m_doTiming ) m_timerTool->start( m_veloTTTime );
  
  m_ttHitManager->prepareHits();

  LHCb::Tracks* outputTracks  = new LHCb::Tracks();
  outputTracks->reserve(200);
  put(outputTracks, m_outputTracksName);

  LHCb::Track::Range inputTracks   = get<LHCb::Track::Range>( m_inputTracksName ); 
  if( inputTracks.empty() ){
    if(msgLevel(MSG::DEBUG)) debug() << " Input Tracks container: " <<  m_inputTracksName << " is empty! Skipping" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  std::vector<LHCb::Track*> tmpTracks;
  tmpTracks.reserve(5);
  
  counter("#seeds") += inputTracks.size();

  for(const LHCb::Track* veloTr: inputTracks){
    
    m_veloTTTool->tracksFromTrack(*veloTr, tmpTracks).ignore();
    
    LHCb::Track* bestTrack = nullptr;
    
    float maxChi2 = m_maxChi2;

    for ( LHCb::Track* fitTrack : tmpTracks ) {

      if (m_fitTracks) {  
        StatusCode sc = m_tracksFitter -> fit( *fitTrack );
        if(msgLevel(MSG::DEBUG)) debug() << "chi2 after KF " << fitTrack->chi2PerDoF() << endmsg;

        if( !sc ){
          warning() << "Could not fit track" << endmsg;
          delete fitTrack;
          continue;
        }
        
      }
      
      if( fitTrack->chi2PerDoF() < maxChi2){
        maxChi2 = fitTrack->chi2PerDoF();
        bestTrack = fitTrack;
      }
      
    }

    tmpTracks.clear();
    
    if( bestTrack != nullptr){
      outputTracks->insert(bestTrack);
    }
  }
  

  counter("#tracks") += outputTracks->size();
  if ( m_doTiming ) m_timerTool->stop( m_veloTTTime );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatVeloTTHybrid::finalize() {

  return GaudiAlgorithm::finalize();  // must be called after all other actions
  
}
//=============================================================================
