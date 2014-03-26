// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "PatForward.h"
#include "Event/ProcStatus.h"

#include "Event/STLiteCluster.h"
#include "OTDAQ/IOTRawBankDecoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatForward
//
// 2005-04-01 : Olivier Callot
//-----------------------------------------------------------------------------
std::array<unsigned short,5> count(const LHCb::Track& lhs, const LHCb::Track& rhs) {
  // adapted from Track::nCommonLhcbIDs
  std::array<unsigned short,5> rc{{0,0,0,0,0}}; // nbTT0, nb0, nbTT1, nb1, ncommon
  auto i1 = std::begin(lhs.lhcbIDs());
  auto last1  = std::end(lhs.lhcbIDs());
  auto i2 = std::begin(rhs.lhcbIDs());
  auto last2  = std::end(rhs.lhcbIDs());
  auto f1 = [&](const LHCb::LHCbID& id) { if (!id.isVelo()) ++rc[ id.isTT() ? 0 : 1 ]; };
  auto f2 = [&](const LHCb::LHCbID& id) { if (!id.isVelo()) ++rc[ id.isTT() ? 2 : 3 ]; };
  while (i1 != last1 && i2 != last2) {
    if ( *i1 < *i2 ) {
      f1(*i1); ++i1;
    } else if ( *i2 < *i1 ) {
      f2(*i2); ++i2;
    } else {
      if (!i1->isVelo()) {
            if (i1->isTT()) { ++rc[0]; ++rc[2]; 
            } else { ++rc[1]; ++rc[3]; ++rc[4]; 
            }
      }
      ++i1;
      ++i2;
    }
  }
  std::for_each(i1,last1,f1);
  std::for_each(i2,last2,f2);
  return rc ; // rely on NRVO
}

DECLARE_ALGORITHM_FACTORY( PatForward )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForward::PatForward( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_fwdTime(0)
  , m_rawBankDecoder(nullptr)
  , m_tHitManager(nullptr)
  , m_trackSelector(nullptr)
  , m_forwardTool(nullptr)
  , m_timerTool(nullptr)
{ 
  declareProperty( "InputTracksName",    m_inputTracksName    = LHCb::TrackLocation::Velo );
  declareProperty( "OutputTracksName",   m_outputTracksName   = LHCb::TrackLocation::Forward);
  declareProperty( "VeloVetoTracksName", m_veloVetoTracksName = LHCb::TrackLocation::Forward);
  declareProperty( "TrackSelectorName",  m_trackSelectorName   = "None");
  declareProperty( "ForwardToolName",    m_forwardToolName    = "PatForwardTool" );

  if ( "Hlt" == context() ) {
    m_inputTracksName  =  "";
    m_outputTracksName =  "";
  }
  declareProperty( "MaxNVelo",   m_maxNVelo = 1000 );  
  declareProperty( "maxITHits" ,  m_maxNumberITHits = 3000);  
  declareProperty( "maxOTHits" , m_maxNumberOTHits = 10000 );
  declareProperty( "DeltaNumberInT",   m_deltaNumberInT  = 3 );
  declareProperty( "DeltaNumberInTT",  m_deltaNumberInTT = 1 );
  declareProperty( "DoCleanUp", m_doClean = true); 
  declareProperty( "UnusedVeloSeeds", m_unusedVeloSeeds = false); 
  declareProperty( "TimingMeasurement", m_doTiming = false);
  // switch on or off NN var. writing
  declareProperty( "writeNNVariables", m_writeNNVariables = true);
}
//=============================================================================
// Destructor
//=============================================================================
PatForward::~PatForward() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatForward::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_trackSelector = nullptr;
  if (m_trackSelectorName != "None") {
    m_trackSelector = tool<ITrackSelector>(m_trackSelectorName, this);
  }

  m_forwardTool = tool<IPatForwardTool>( m_forwardToolName, this );
  m_forwardTool->setNNSwitch( m_writeNNVariables); // pass the NN switch to PatForwardTool

  if ( msgLevel( MSG::DEBUG ) || m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_fwdTime = m_timerTool->addTimer( "Internal PatForward" );
    m_timerTool->decreaseIndent();
  }

  // tool handle to the ot raw bank decoder
  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");

  m_tHitManager    = tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Check if a track should be processed. Default flags, and selector if defined
//=========================================================================
bool PatForward::acceptTrack(const LHCb::Track& track) {
  bool ok = !(track.checkFlag( LHCb::Track::Invalid) );
  ok = ok && (!(track.checkFlag( LHCb::Track::Backward) ));

  if (m_trackSelector) ok = ok && m_trackSelector->accept(track);

  if (m_unusedVeloSeeds && ok){
    for (auto it : *get<LHCb::Tracks>( m_veloVetoTracksName)) { 
      auto ancestors = it->ancestors();
      ok = std::none_of( std::begin( ancestors ), std::end( ancestors ), 
                         [&](const LHCb::Track* t) { return t == &track; } ) ;
      if (!ok) break;
    } 
  }
  
  if ( msgLevel( MSG::VERBOSE )) verbose() << "For track " << track.key() << " accept flag =" << ok << endmsg;
  return ok;
}


template <typename T> 
bool overlaps_( const T& N ) { return  N[4] >=  0.7 * std::max(N[1],N[3])  ; }

int
PatForward::overlaps(const LHCb::Track* lhs, const LHCb::Track* rhs ) const
{
      auto N = count( *lhs , *rhs );
      if ( !overlaps_(N) ) return 0;;
      if ( UNLIKELY( msgLevel ( MSG::DEBUG ) ) ) {
        debug() << format( "Track %3d and %3d nT0 %2d nT1 %2d nCommon %2d nbTT : %2d, %2d  chi20 %8.3f 1 %8.3f",
                           lhs->key(), rhs->key(), N[1], N[3], N[4], N[0], N[2],
                           lhs->chi2PerDoF(), rhs->chi2PerDoF() ) << endmsg;
      }
      auto dnbTT = N[0] - N[2];
      auto dnb   = N[1] - N[3];
      if (  dnb   > m_deltaNumberInT ||  dnbTT > m_deltaNumberInTT ) return +1;
      if ( -dnb   > m_deltaNumberInT || -dnbTT > m_deltaNumberInTT ) return -1;
      return 0;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatForward::execute() {

  m_tHitManager->prepareHits(); 
        
  //== Prepare tracks
 
  LHCb::Tracks* inputTracks   = get<LHCb::Tracks>( m_inputTracksName ); 

  LHCb::Tracks* outputTracks  = 
    getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_outputTracksName);

  if (inputTracks->size() > (unsigned int) m_maxNVelo) {
    LHCb::ProcStatus* procStat =
      getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
		 LHCb::ProcStatusLocation::Default);
    // give some indication that we had to skip this event
    // (ProcStatus returns zero status for us in cases where we don't
    // explicitly add a status code)
    procStat->addAlgorithmStatus(name(), "Tracking", "LimitVeloTracksExceeded", -3 , true );
    return Warning("Too many velo tracks", StatusCode::SUCCESS, 0);
  }
 // reject hot events
  const LHCb::STLiteCluster::STLiteClusters* clusterCont 
    = get<LHCb::STLiteCluster::STLiteClusters>(LHCb::STLiteClusterLocation::ITClusters);
  if (clusterCont->size() > m_maxNumberITHits ){
     LHCb::ProcStatus* procStat =
	getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
	    LHCb::ProcStatusLocation::Default);
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
      procStat->addAlgorithmStatus(name(), "Tracking", "LimitOfITHitsExceeded", -3 , true );
    return Warning("Too many IT hits event rejected", StatusCode::SUCCESS, 0);
  }  

  const unsigned int nHitsInOT = m_rawBankDecoder->totalNumberOfHits();
  if (nHitsInOT > m_maxNumberOTHits){
    LHCb::ProcStatus* procStat =
	getOrCreate<LHCb::ProcStatus,LHCb::ProcStatus>(
	    LHCb::ProcStatusLocation::Default);
      // give some indication that we had to skip this event
      // (ProcStatus returns zero status for us in cases where we don't
      // explicitly add a status code)
     procStat->addAlgorithmStatus(name(), "Tracking", "LimitOfOTHitsExceeded", -3 , true );
    return Warning("Too Many OT hits event rejected", StatusCode::SUCCESS,0); 
  }

  bool dbg = msgLevel(MSG::DEBUG);
  if ( dbg ) debug() << "==> Execute" << endmsg;
  if ( dbg  || m_doTiming ) m_timerTool->start( m_fwdTime );

  int oriSize = outputTracks->size();

  for ( LHCb::Track *seed: *inputTracks ) {
    if ( acceptTrack(*seed) ) {
      auto prevSize = outputTracks->size();
      m_forwardTool->forwardTrack(seed, outputTracks );
      if ( dbg ) debug()  << " track " << seed->key()
                          << " position " << seed->position()
                          << " slopes " << seed->slopes()  
                          << " cov \n" << seed->firstState().covariance() << "\n"
                          << " produced " << outputTracks->size() - prevSize
                          << " tracks " << endmsg;

    }
  }
  // added for NNTools -- check how many tracks have common hits
  if( m_writeNNVariables){
    for( auto tr : *outputTracks ) {
      auto x0 = tr->stateAt( LHCb::State::AtT )->x();
      auto match = [=](const LHCb::Track* t) {
            return fabs( x0 - t->stateAt( LHCb::State::AtT )->x() ) < 5.  && 
                   overlaps_( count( *tr , *t ) );
      };
      tr->addInfo(LHCb::Track::NCandCommonHits,
                  std::count_if( std::begin(*outputTracks), std::end(*outputTracks), match ));
    }
  }
  // end of NNTools loop

  if (m_doClean) {
      //== Try to filter from T station part
      for ( auto itT = std::begin(*outputTracks); itT != std::end(*outputTracks) ;  ++itT ) {
        auto x0 = (*itT)->stateAt( LHCb::State::AtT )->x();
        auto overlaps0 = [=]( const LHCb::Track* t) {
          auto x1  = t->stateAt( LHCb::State::AtT )->x();
          return  ( fabs( x0 - x1 ) < 5. ) ? this->overlaps( *itT, t ) : 0;
        };

        for ( auto itT1 = itT+1; itT1 != std::end(*outputTracks) ;  ++itT1 ) {
          auto overlap = overlaps0( *itT1 );
          if (overlap) {
              auto i = ( overlap < 0 ? itT : itT1 );
              if( UNLIKELY( dbg  ) ) debug() << "    erase " << (*i)->key() << " and restart " << endmsg;
              //TODO: verify that this is not a memory leak... 
              outputTracks->erase( i );
              itT  = outputTracks->begin();
              break;
          }
        }
      }
  }

  if ( dbg  || m_doTiming ) {
    double t = m_timerTool->stop( m_fwdTime );
    debug() << "=== In total, produced " <<  outputTracks->size() - oriSize  << " tracks from "
            << inputTracks->size() << " Velo tracks in " << t << " ms"
            << endmsg;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
