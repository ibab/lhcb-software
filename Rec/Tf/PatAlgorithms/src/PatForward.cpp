// Include files

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

class count_hits { 

    enum { ibTT0, ib0, ibTT1, ib1, icommon };
    std::array<short,5> m_count;// nbTT0, nb0, nbTT1, nb1, nbcommon

public:
    count_hits(const LHCb::Track& lhs, const LHCb::Track& rhs)
    : m_count{{0,0,0,0,0}} 
    {
      // adapted from Track::nCommonLhcbIDs
      auto i1 = std::begin(lhs.lhcbIDs());
      auto last1  = std::end(lhs.lhcbIDs());
      auto i2 = std::begin(rhs.lhcbIDs());
      auto last2  = std::end(rhs.lhcbIDs());
      auto f1 = [&](const LHCb::LHCbID& id) { if (!id.isVelo()) ++m_count[ id.isTT() ? ibTT0 : ib0 ]; };
      auto f2 = [&](const LHCb::LHCbID& id) { if (!id.isVelo()) ++m_count[ id.isTT() ? ibTT1 : ib1 ]; };
      while (i1 != last1 && i2 != last2) {
        if ( *i1 < *i2 ) {
          f1(*i1); ++i1;
        } else if ( *i2 < *i1 ) {
          f2(*i2); ++i2;
        } else {
          if (!i1->isVelo()) {
                if (i1->isTT()) { ++m_count[ibTT0]; ++m_count[ibTT1]; 
                } else { ++m_count[ib0]; ++m_count[ib1]; ++m_count[icommon]; 
                }
          }
          ++i1;
          ++i2;
        }
      }
      std::for_each(i1,last1,f1);
      std::for_each(i2,last2,f2);
    }

    bool overlaps() const { return nbcommon() >= 0.7 * std::max(nb0(),nb1()); }

    short nbTT0() const { return m_count[ibTT0]; }
    short nbTT1() const { return m_count[ibTT1]; }
    short nb0() const { return m_count[ib0]; }
    short nb1() const { return m_count[ib1]; }
    short nbcommon() const { return m_count[icommon]; }
};

DECLARE_ALGORITHM_FACTORY( PatForward )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForward::PatForward( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_fwdTime(0)
  , m_rawBankDecoder(nullptr)
  , m_forwardTool(nullptr)
  , m_timerTool(nullptr)
{ 
  declareProperty( "InputTracksName",    m_inputTracksName    = LHCb::TrackLocation::Velo );
  declareProperty( "OutputTracksName",   m_outputTracksName   = LHCb::TrackLocation::Forward);
  declareProperty( "ForwardToolName",    m_forwardToolName    = "PatForwardTool" );
  declareProperty( "MaxNVelo",   m_maxNVelo = 1000 );  
  declareProperty( "maxITHits" ,  m_maxNumberITHits = 3000);  
  declareProperty( "maxOTHits" , m_maxNumberOTHits = 10000 );
  declareProperty( "DeltaNumberInT",   m_deltaNumberInT  = 3 );
  declareProperty( "DeltaNumberInTT",  m_deltaNumberInTT = 1 );
  declareProperty( "DoCleanUp", m_doClean = true); 
  declareProperty( "TimingMeasurement", m_doTiming = false);
  // switch on or off NN var. writing
  declareProperty( "writeNNVariables", m_writeNNVariables = true);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatForward::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;


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

  //m_tHitManager    = tool<Tf::TStationHitManager <PatForwardHit> >("PatTStationHitManager");

  return StatusCode::SUCCESS;
}


int
PatForward::overlaps(const LHCb::Track* lhs, const LHCb::Track* rhs ) const
{
    count_hits N( *lhs, *rhs) ;
    if ( !N.overlaps()  ) return 0;;
    if ( UNLIKELY( msgLevel ( MSG::DEBUG ) ) ) {
      debug() << format( "Track %3d and %3d nT0 %2d nT1 %2d nCommon %2d nbTT : %2d, %2d  chi20 %8.3f 1 %8.3f",
                         lhs->key(), rhs->key(), N.nb0(), N.nb1(), N.nbcommon(), N.nbTT0(), N.nbTT1(),
                         lhs->chi2PerDoF(), rhs->chi2PerDoF() ) << endmsg;
    }
    auto dnbTT = N.nbTT0() - N.nbTT1(); 
    auto dnb   = N.nb0() - N.nb1();
    if (  dnb   > m_deltaNumberInT ||  dnbTT > m_deltaNumberInTT ) return +1;
    if ( -dnb   > m_deltaNumberInT || -dnbTT > m_deltaNumberInTT ) return -1;
    return 0;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatForward::execute() {

  //== Prepare tracks
 
  LHCb::Track::Range inputTracks   = get<LHCb::Track::Range>( m_inputTracksName ); 

  LHCb::Tracks* outputTracks  = 
    getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_outputTracksName);

  // S.Stahl: Why are GEC cuts in here?
  if (inputTracks.size() > m_maxNVelo) {
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

  counter("#Seeds") += inputTracks.size();
  
  for ( const LHCb::Track *seed: inputTracks ) {
    auto prevSize = outputTracks->size();
    m_forwardTool->forwardTrack(seed, outputTracks );
    if ( dbg ) debug()  << " track " << seed->key()
                        << " position " << seed->position()
                        << " slopes " << seed->slopes()  
                        << " cov \n" << seed->firstState().covariance() << "\n"
                        << " produced " << outputTracks->size() - prevSize
                        << " tracks " << endmsg;
    
    
  }
  // added for NNTools -- check how many tracks have common hits
  if( m_writeNNVariables){
    for( auto tr : *outputTracks ) {
      auto x0 = tr->stateAt( LHCb::State::AtT )->x();
      auto matches_tr = [=](const LHCb::Track* t) {
            return fabs( x0 - t->stateAt( LHCb::State::AtT )->x() ) < 5.  && 
                   count_hits( *tr , *t ).overlaps() ;
      };
      tr->addInfo(LHCb::Track::NCandCommonHits,
                  std::count_if( std::begin(*outputTracks), std::end(*outputTracks), matches_tr ));
    }
  }
  // end of NNTools loop

  if (m_doClean) {
      //== Try to filter from T station part
      for ( auto itT = std::begin(*outputTracks); itT != std::end(*outputTracks) ;  ++itT ) {
        auto x0 = (*itT)->stateAt( LHCb::State::AtT )->x();
        auto overlaps_with_itT = [=]( const LHCb::Track* t) {
          auto x1 = t->stateAt( LHCb::State::AtT )->x();
          return ( fabs( x0 - x1 ) < 5. ) ? this->overlaps( *itT, t ) : 0;
        };

        for ( auto itT1 = itT+1; itT1 != std::end(*outputTracks) ;  ++itT1 ) {
          auto overlap = overlaps_with_itT( *itT1 );
          if (overlap) {
              auto i = ( overlap < 0 ? itT : itT1 );
              if( UNLIKELY( dbg  ) ) debug() << "    erase " << (*i)->key() << " and restart " << endmsg;
              outputTracks->erase( i );
              // break innermost loop, and restart the outermost loop
              itT  = outputTracks->begin();
              break;
          }
        }
      }
  }

  if ( dbg  || m_doTiming ) {
    double t = m_timerTool->stop( m_fwdTime );
    debug() << "=== In total, produced " <<  outputTracks->size() - oriSize  << " tracks from "
            << inputTracks.size() << " Velo tracks in " << t << " ms"
            << endmsg;
  }

  counter("#Tracks") += outputTracks->size() - oriSize;

  return StatusCode::SUCCESS;
}

//=============================================================================
