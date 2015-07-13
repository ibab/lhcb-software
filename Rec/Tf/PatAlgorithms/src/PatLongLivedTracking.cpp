// Include files 

#include <algorithm>
#include <cmath>


// from ROOT
#include <Math/CholeskyDecomp.h>
using ROOT::Math::CholeskyDecomp;

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

#include "STDet/DeSTDetector.h"
#include "STDet/DeTTSector.h"

#include "Event/Track.h"

#include "PatKernel/IPatDebugTTTool.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/ITTHitCreator.h"
#include "Event/StateParameters.h"
// local
#include "PatDownTrack.h"
#include "PatLongLivedTracking.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatLongLivedTracking
//
// 2015-06-09: Michel De Cian
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatLongLivedTracking )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatLongLivedTracking::PatLongLivedTracking( const std::string& name,
                                ISvcLocator* pSvcLocator)
: GaudiAlgorithm ( name , pSvcLocator ),
  m_downTime( 0 ),
  m_preselTime( 0 ),
  m_findMatchingHitsTime( 0 ),
  m_fitXProjectionTime( 0 ),
  m_fitAndRemoveTime( 0 ),
  m_xFitTime( 0 ),
  m_addUHitsTime( 0 ),
  m_addVHitsTime( 0 ),
  m_acceptCandidateTime( 0 ),
  m_storeTrackTime( 0 ),
  m_overlapTime( 0 ),
  m_printing( false ),
  m_magnetOff( false ),
  m_firstEvent(true),
  m_magFieldSvc(nullptr),
  m_ttHitManager(nullptr),
  m_debugTool(nullptr),
  m_timerTool(nullptr)
{
  declareProperty( "InputLocation" ,                 m_inputLocation        = ""                                          );
  declareProperty( "OutputLocation",                 m_outputLocation       = LHCb::TrackLocation::Downstream             );
  declareProperty( "ForwardLocation",                m_forwardLocation      = LHCb::TrackLocation::Forward                );
  declareProperty( "MatchLocation",                  m_matchLocation        = LHCb::TrackLocation::Match                  );
  declareProperty( "XPredTolConst",                  m_xPredTolConst        = 200. * Gaudi::Units::mm * Gaudi::Units::GeV );
  declareProperty( "XPredTolOffset",                 m_xPredTolOffset       = 6. * Gaudi::Units::mm                       );
  declareProperty( "TolMatchConst",                  m_tolMatchConst        = 20000.                                      );
  declareProperty( "TolMatchOffset",                 m_tolMatchOffset       = 1.5 *  Gaudi::Units::mm                     );
  declareProperty( "TolUConst",                      m_tolUConst            = 20000.0                                     );
  declareProperty( "TolUOffset",                     m_tolUOffset           = 2.5                                         );
  declareProperty( "TolVConst",                      m_tolVConst            = 2000.0                                      );
  declareProperty( "TolVOffset",                     m_tolVOffset           = 0.5                                         );
  
  declareProperty( "MaxWindowSize",                  m_maxWindow            = 10.0 * Gaudi::Units::mm                     );
  declareProperty( "MaxChi2",                        m_maxChi2              = 20.                                         );
  declareProperty( "MaxChi2ThreeHits",               m_maxChi2ThreeHits     = 10.0                                        );
  declareProperty( "MinTTx",                         m_minTTx               = 33. *  Gaudi::Units::mm                     );
  declareProperty( "MinTTy",                         m_minTTy               = 33. *  Gaudi::Units::mm                     );
  // Define parameters for MC09 field, zState = 9410
  declareProperty( "ZMagnetParams",                  m_zMagnetParams        = {5372.13, -3702.66, 401.378, 109033, 
        -0.0119398, -0.158909, 1668.42});
  declareProperty( "MomentumParams",                 m_momentumParams       = {1148.65,961.786,5326.81}                   );
  declareProperty( "YParams",                        m_yParams              = {5.,2000.}                                  );
  declareProperty( "ZTT",                            m_zTT                  = 2485.* Gaudi::Units::mm                     );
  declareProperty( "ZTTa",                           m_zTTa                 = 2350.* Gaudi::Units::mm                     );

  declareProperty( "StateErrorX2",                   m_stateErrorX2         = 4.0                                         );
  declareProperty( "StateErrorY2",                   m_stateErrorY2         = 400.                                        );
  declareProperty( "StateErrorTX2",                  m_stateErrorTX2        = 6.e-5                                       );
  declareProperty( "StateErrorTY2",                  m_stateErrorTY2        = 1.e-4                                       );
  declareProperty( "StateErrorP",                    m_stateErrorP          = 0.15                                        );
  declareProperty( "MinPt",                          m_minPt                = 0. * Gaudi::Units::MeV                      );
  declareProperty( "MinMomentum",                    m_minMomentum          = 0. * Gaudi::Units::GeV                      );

  // -- Parameter to reject seed track which are likely ghosts
  declareProperty( "FisherCut",                      m_seedCut              = -1.0                                        );
  
  // -- Parameters for the cut on deltaP (momentum estimate from Seeding and Downstream kink)
  declareProperty( "MaxDeltaPConst",                 m_maxDeltaPConst       = 750                                         );
  declareProperty( "MaxDeltaPOffset",                m_maxDeltaPOffset      = 0.25                                        );
  
  // -- Parameters for correcting the predicted position
  declareProperty( "XCorrectionConst",               m_xCorrectionConst     = 23605.0                                     );
  declareProperty( "XCorrestionOffset",              m_xCorrectionOffset    = 0.4                                         );

  
  declareProperty( "MaxXTracks",                     m_maxXTracks           = 2                                           );
  declareProperty( "MaxChi2DistXTracks",             m_maxChi2DistXTracks   = 0.2                                         );
  declareProperty( "MaxXUTracks",                    m_maxXUTracks          = 2                                           );
  declareProperty( "MinChi2XProjection",             m_maxChi2XProjection   = 200                                         );
  
  // -- Tolerance for adding overlap hits
  declareProperty( "OverlapTol",                     m_overlapTol           = 2.0*Gaudi::Units::mm                        );
  declareProperty( "YTol",                           m_yTol                 = 2.0*Gaudi::Units::mm                        );
  // Change this in order to remove hits and T-tracks used for longtracks.
  // RemoveAll configures that everything is removed.
  // If false only hits and T-tracks from good longtracks are removed.
  // The criterion for this is the Chi2 of the longtracks from the fit.
  declareProperty( "RemoveUsed",                     m_removeUsed           = false                                       );
  declareProperty( "RemoveAll",                      m_removeAll            = false                                       );
  declareProperty( "LongChi2",                       m_longChi2             = 1.5                                         );

  //== debugging options
  declareProperty( "SeedKey",                        m_seedKey              = -1                                          );
  declareProperty( "WithDebugTool",                  m_withDebugTool        = false                                       );
  declareProperty( "DebugTool",                      m_debugToolName        = "PatDebugTTTruthTool"                       );
  declareProperty( "PrintTracks",                    m_printTracks          = false                                       );
  declareProperty( "TimingMeasurement",              m_doTiming             = false                                       );
}
//=============================================================================
// Destructor
//=============================================================================
PatLongLivedTracking::~PatLongLivedTracking() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatLongLivedTracking::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");
  
  if ( m_withDebugTool ) {
    m_debugTool = tool<IPatDebugTTTool>( m_debugToolName );
  }  

  info() << "========================================================= "                        << endmsg
         << " XPredTol           = " << m_xPredTolConst       << "/p + " << m_xPredTolOffset    << endmsg
         << " TolMatchConst      = " << m_tolMatchConst       << "/p + " << m_tolMatchOffset    << endmsg
         << " TolUConst          = " << m_tolUConst           << "/p + " << m_tolUOffset        << endmsg
         << " TolVConst          = " << m_tolVConst           << "/p + " << m_tolVOffset        << endmsg
         << " MaxDeltaPConst     = " << m_maxDeltaPConst      << "/p + " << m_maxDeltaPOffset   << endmsg
         << " XCorrectionConst   = " << m_xCorrectionConst    << "/p + " << m_xCorrectionOffset << endmsg
         << " MaxXTracks         = " << m_maxXTracks          << endmsg
         << " MaxChi2DistXTracks = " << m_maxChi2DistXTracks  << endmsg
         << " MaxXUTracks        = " << m_maxXUTracks         << endmsg
         << " MaxChi2XProjection = " << m_maxChi2XProjection  << endmsg
         << " OverlapTol         = " << m_overlapTol          << endmsg
         << " SeedCut            = " << m_seedCut             << endmsg
         << " MaxChi2            = " << m_maxChi2             << endmsg
         << " MaxChi2ThreeHits   = " << m_maxChi2ThreeHits    << endmsg
         << " MaxWindowSize      = " << m_maxWindow           << endmsg
         << " RemoveUsed         = " << m_removeUsed          << endmsg
         << " RemoveAll          = " << m_removeAll           << endmsg
         << " LongChi2           = " << m_longChi2            << endmsg     
         << " TimingMeasurements = " << m_doTiming            << endmsg     
         <<  "========================================================= "   << endmsg;
  
  info() << "zMagnetParams ";
  for ( unsigned int kk = 0; m_zMagnetParams.size() > kk ; kk++) {
    info() << m_zMagnetParams[kk] << " ";
  }
  info() << endmsg << "momentumParams ";
  for ( unsigned int kk = 0; m_momentumParams.size() > kk ; kk++) {
    info() << m_momentumParams[kk] << " ";
  }
  info() << endmsg ;
  if ( 3 > m_zMagnetParams.size() ) {
    return Warning( "Not enough zMagnetParams" );
  }
  if ( 3 > m_momentumParams.size() ) {
    return Warning( "Not enough momentumParams" );
  }

  m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  
  if ( m_doTiming) {
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool/Timer", this );
    m_timerTool->increaseIndent();
    m_downTime = m_timerTool->addTimer( "Execute" );
    m_preselTime = m_timerTool->addTimer( "getPreSelection" );
    m_findMatchingHitsTime = m_timerTool->addTimer( "FindMatchingHits" );
    m_fitXProjectionTime = m_timerTool->addTimer( "FitXProjection" );
    m_fitAndRemoveTime = m_timerTool->addTimer( "FitAndRemove" );
    m_xFitTime = m_timerTool->addTimer( "xFit" );
    m_addUHitsTime = m_timerTool->addTimer( "AddUHits" );
    m_addVHitsTime = m_timerTool->addTimer( "AddVHits" );
    m_acceptCandidateTime = m_timerTool->addTimer( "acceptCandidate" );
    m_overlapTime = m_timerTool->addTimer( "addOverlap" );
    m_storeTrackTime = m_timerTool->addTimer( "storeTrack" );
    m_timerTool->decreaseIndent();
  }

  // -- fill some helper variables
  m_addIsStereoHelper = { 0, 1, 1, 0 };

  // -- values for Fisher discriminant
  m_fishConst = -1.69860581797;
  m_fishCoefficients = { -0.241020410138, 3.03197732663e-07, -1.14400162824e-05,  0.126857153245, 0.122359738469 };

  m_matchingXHits.reserve(64);
  m_trackCandidates.reserve(16);
  m_goodXTracks.reserve(8);
  m_goodXUTracks.reserve(8);
  m_uHitsTemp.reserve(64);
  

  // -- that's a little ugly and should be improved in the future
  m_magPars = { m_zMagnetParams[0], m_zMagnetParams[1], m_zMagnetParams[2], m_zMagnetParams[3], 
                m_zMagnetParams[4], m_zMagnetParams[5], m_zMagnetParams[6] };
  m_momPars = { m_momentumParams[0], m_momentumParams[1], m_momentumParams[2] };


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatLongLivedTracking::execute() {
  
  if ( m_doTiming ) m_timerTool->start( m_downTime );

  m_printing = msgLevel( MSG::DEBUG );  
  if (m_printing) debug() << "==> Execute" << endmsg;

  // -- If no specified input location, get seed if it exists, else Tsa.
  if ( m_inputLocation.empty() ) {
    if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Seed ) ) {
      m_inputLocation = LHCb::TrackLocation::Seed;
    } else {
      if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Tsa ) ) {
        m_inputLocation = LHCb::TrackLocation::Tsa;
      } else {
        error() << "Could not find input location: " << LHCb::TrackLocation::Seed 
                << " or " << LHCb::TrackLocation::Tsa << endmsg;
      }
    }
  }
  
  //==========================================================================
  // Prepare hits in TT, optional: remove used in PatForward.
  //========================================================================== 
  ttCoordCleanup();
  
  //==========================================================================
  // Prepare the tracks, optional: removing the seends already used in Match.
  //==========================================================================
  LHCb::Tracks* inTracks   = get<LHCb::Tracks>( m_inputLocation );

  //== Local container of good track.
  std::vector<LHCb::Track*> myInTracks;
  myInTracks.reserve(inTracks->size());  

  //=== Optional: Remove the seeds already used in Match, check by ancestor of Match tracks
  //=== Prepare T-Seeds
  prepareSeeds( inTracks, myInTracks );
  
  // -- put the hits in a container per layer and sort them according to x at y=0
  initEvent();
  

  //==========================================================================
  // Get the output container 
  //==========================================================================
  LHCb::Tracks* finalTracks = new LHCb::Tracks();
  finalTracks->reserve(100);
  put( finalTracks, m_outputLocation);

  const double magScaleFactor = m_magFieldSvc->signedRelativeCurrent() ;
  
  if( std::abs(magScaleFactor) > 1e-6 ){
    m_magnetOff = false;
  } else m_magnetOff = true;
  
  //==========================================================================
  // Main loop on tracks
  //==========================================================================
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords;
  if ( UNLIKELY( m_printing ) )  ttCoords = m_ttHitManager->hits();
  
  for ( LHCb::Track* tr : myInTracks ) {
  
    // -- simple Fisher discriminant to reject bad seed tracks
    const double fisher = evaluateFisher( tr );
    if( fisher < m_seedCut ) continue;
    
    if ( 0 <= m_seedKey && m_seedKey == tr->key() ) m_printing = true;
    
    PatDownTrack track( tr, m_zTT, m_magPars, m_momPars, m_yParams, magScaleFactor*(-1) );
    
    if( std::abs( track.momentum()) < 1400 ) continue;
    
    // -- Veto particles coming from the beam pipe.
    if( insideBeampipe( track ) ) continue;
    
    const double deltaP = track.momentum() * track.state()->qOverP() - 1.;
  
    // --
    if ( UNLIKELY( m_printing ) ) {
      for ( PatTTHit* hit : ttCoords ){
        if (hit->hit()->ignore()) continue;
        const double yTrack = track.yAtZ( 0. );
        const double tyTr   = track.slopeY();
        updateTTHitForTrackFast( hit, yTrack, tyTr );
      }
      
      info() << "Track " << tr->key() 
             << format( " [%7.2f %7.2f GeV] x,y(TTa)%7.1f%7.1f dp/p%7.3f errXMag%6.1f YTT%6.1f",
                        .001/track.state()->qOverP(), .001*track.momentum(),
                        track.xAtZ( m_zTTa ), track.yAtZ( m_zTTa ), deltaP,
                        track.errXMag(), track.errYMag() )
             << endmsg;
      info() << format( " Y slope %8.6f computed %8.6f", track.state()->ty(), track.slopeY() ) 
             << endmsg;
      
      if ( m_debugTool ) m_debugTool->debugTTClusterOnTrack( tr, ttCoords.begin(), ttCoords.end() );      
    }
    // --
    
    // -- check for compatible momentum
    if ( maxDeltaP( track ) < fabs(deltaP) ) {
      if ( UNLIKELY( m_printing )) info() << "   --- deltaP " << deltaP << " -- rejected" << endmsg;
      if ( !m_magnetOff ) continue;
    }
    
    // -- Get hits in TT around a first track estimate
    getPreSelection( track );
   
    // -- Need at least 3 hits and at least 1 stereo and 1 x hit
    if( 3 > m_preSelHits[0].size() +  m_preSelHits[1].size() + m_preSelHits[2].size() + m_preSelHits[3].size() ) continue;
    if( 1 > m_preSelHits[0].size() +  m_preSelHits[3].size() ) continue;
    if( 1 > m_preSelHits[1].size() +  m_preSelHits[2].size() ) continue;
 
    int nbOK = 0;
    int maxPoints = 0;
    m_trackCandidates.clear();
    m_matchingXHits.clear();

    //==============================================================
    // Try to find a candidate: X first, then UV.
    //==============================================================
    for( PatTTHit* myHit : m_xHits ){
      
      if ( myHit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream ) ) continue;
      const double meanZ = myHit->z();
      const double posX  = myHit->x( );
      const int myPlane  = myHit->planeCode();
      
      track.startNewCandidate();
            
  
      // -- Create track estimate with one x hit
      const double slopeX = (track.xMagnet() - posX + track.sagitta( meanZ)) / (track.zMagnet() - meanZ);
      track.setSlopeX( slopeX );
      // -- now we know the slope better and can adapt the curvature
      // -- this effect is very small, but seems to be beneficial
      const double curvature =  1.6e-5 * ( track.state()->tx() - slopeX );
      track.setCurvature( curvature );
      
      // -----------------
      if ( UNLIKELY( m_printing )) {
        const double tolMatch = (std::abs(track.state()->p() / m_tolMatchConst) < 1. / (m_maxWindow - m_tolMatchOffset)) ?
          m_maxWindow : (m_tolMatchOffset + m_tolMatchConst / track.state()->p());
        info() << endmsg 
               << format( "... start plane %1d x%8.2f z%8.1f slope%8.2f tolMatch%7.3f",
                          myPlane, posX, meanZ, 1000. * slopeX, tolMatch )
               << endmsg;
      }        
      // -----------------

      // -- Fit x projection
      findMatchingHits( track, myPlane);
      
      if( !m_matchingXHits.empty() ){
        fitXProjection( track, myHit );
      }else{
        m_goodXTracks.clear();
        track.hits().push_back( myHit );
        m_goodXTracks.push_back( track );
      }
      
      
      // -- Take all xTracks into account whose chi2 is close to the best
      // -- until MaxXTracks is reached
      unsigned int maxI = 1;
      
      if( m_goodXTracks.size() > 1){
        for(unsigned int i = 1; i < m_goodXTracks.size() && i < m_maxXTracks; ++i){
          if(m_goodXTracks[i].chi2() - m_maxChi2DistXTracks < m_goodXTracks[0].chi2()) maxI = i;
        }
      }

      // -- Loop over good x tracks
      for( unsigned int i = 0; i < maxI; ++i){
      
        PatDownTrack& xTrack = m_goodXTracks[i];
        
        addUHits( xTrack, m_maxXUTracks );
        unsigned int maxJ = std::min( (unsigned int)m_maxXUTracks, (unsigned int)m_goodXUTracks.size() );
        
        // -- Loop over good xu tracks
        for(unsigned int j = 0; j < maxJ; j++){
          
          PatDownTrack& xuTrack = m_goodXUTracks[j];
          
          addVHits( xuTrack );
          fitAndRemove( xuTrack, false );
          
          // -- Check if candidate is better than the old one
          if ( !acceptCandidate( xuTrack, maxPoints ) ) continue;
          m_trackCandidates.push_back( std::move(xuTrack) );
          ++nbOK;
        }
        
        // -- In case no U hits is found, search for V hit
        // -- but with larger search window
        if( xTrack.hits().size() > 1 && maxPoints < 4 && m_goodXUTracks.empty() ){
          addVHits( xTrack );
          fitAndRemove( xTrack, false );
          // -- Check if candidate is better than the old one
          if ( !acceptCandidate( xTrack, maxPoints ) ) continue;
          m_trackCandidates.push_back( std::move(xTrack) );
          ++nbOK;
        }
      }
    }
    
    // -- The 'nbOK' is just paranoia....
    if( m_trackCandidates.empty() || nbOK == 0) continue;
   
    // -- Now we have all possible candidates
    // -- Add overlap regions, fit again and flag
    // -- bad candidates
    for( PatDownTrack& track : m_trackCandidates ){
    
      addOverlapRegions( track );

      if( m_withDebugTool && m_debugTool ){
        info() << "This track is: " << m_debugTool->isTrueTrack( track.track(), track.hits() ) << endmsg;
        m_debugTool->fracGoodHits( track.track(), track.hits() );
      }
      
      if( track.chi2() > m_maxChi2 ) track.setIgnore(true);
      if( track.firedLayers() < 3  ) track.setIgnore( true );
      if( insideBeampipe( track )  ) track.setIgnore( true );
      if( maxPoints > 3 && track.hits().size() < 4 ) track.setIgnore(true);
    }
    
    std::sort( m_trackCandidates.begin(), m_trackCandidates.end(), 
               [](const PatDownTrack& a, const PatDownTrack& b){ return a.chi2() < b.chi2();});

    for( PatDownTrack& track : m_trackCandidates ){
      
      if( track.ignore() ) continue;
      
      if( m_debugTool ) m_debugTool->chi2Tuple( std::abs( track.momentum() ), track.chi2(), track.hits().size());
      storeTrack( track, finalTracks, track.track() ); 
      break;
    }
    
   
  }
  
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Found " << finalTracks->size() << " tracks." << endmsg;
  
  if ( m_doTiming ) m_timerTool->stop( m_downTime );
  
  return StatusCode::SUCCESS;
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatLongLivedTracking::finalize() {
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=========================================================================
//  Cleanup already used T-Seeds
//=========================================================================
void PatLongLivedTracking::prepareSeeds(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks){
  
  LHCb::Tracks* match = getIfExists<LHCb::Tracks>( m_matchLocation );
 
  if ( NULL != match ) {
    if (!m_removeUsed) {
      myInTracks.insert( myInTracks.end(), inTracks->begin(), inTracks->end() );
    } else {
      if (m_printing) debug()<<"Remove seeds and tt hits from Match tracks"<<endmsg;
      for ( LHCb::Tracks::const_iterator itT = inTracks->begin();
            inTracks->end() != itT; itT++ ) {
        LHCb::Track* tr = (*itT);
        bool store =true;
        if ( m_printing ) debug() << "Seed " << tr->key();
        for( LHCb::Track* matchTr: *match ) {
          const SmartRefVector<LHCb::Track>& ancestors = matchTr->ancestors();
          for ( SmartRefVector<LHCb::Track>::const_iterator itA = ancestors.begin();
                ancestors.end() != itA; ++itA ) {
            const LHCb::Track* pt = (*itA);
            if ( tr == pt ) {
              if ( m_printing ) debug() << " is used in match " << matchTr->key(); 
              if ( m_removeAll || matchTr->chi2PerDoF() < m_longChi2 ) {
                if ( m_printing ) debug() << " good longtrack " << matchTr->key()<<endmsg; 
                store = false;
                tagUsedTT( matchTr );
                break;
              }
              break;
            }
          }
          if ( !store ) break;
        }
        if (store) {
          myInTracks.push_back( tr );
          if ( m_printing ) debug() << " will be processed";  
        }
        if ( m_printing ) debug() << endmsg;
      }
    }
  } else {  //== Copy tracks without ancestor...
    for(LHCb::Track* tr: *inTracks) {
      //== Ignore tracks with ancestor = forward...
      if ( m_removeUsed && 0 < tr->ancestors().size() ) continue;
      myInTracks.push_back( tr );
    }
  }
}
//=========================================================================
//  Cleanup already used TT coord
//=========================================================================
void PatLongLivedTracking::ttCoordCleanup ( ) {

  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();
  for(PatTTHit* hit: ttCoords) hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
  
  //== Tag hit used in forward
  if ( m_removeUsed ) {
    if ( exist<LHCb::Tracks>( m_forwardLocation ) ) {
      if (m_printing) debug()<<"Remove TT hits from Forward tracks from location "
                             <<m_forwardLocation <<endmsg;
      LHCb::Tracks* tracks = get<LHCb::Tracks>( m_forwardLocation );
      for(const LHCb::Track* tr: *tracks) {
        if (m_removeAll || tr->chi2PerDoF()<m_longChi2) tagUsedTT( tr );
      }
    }
  }
}
//=========================================================================
//  Tag used TT hits on this track.
//=========================================================================
void PatLongLivedTracking::tagUsedTT( const LHCb::Track* tr ) {
  
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();
  
  for(LHCb::LHCbID id: tr->lhcbIDs()) {
    if ( !id.isTT() ) continue;
    for(const PatTTHit* hit: ttCoords) {
      if ( hit->hit()->lhcbID() == id ) {
        if (m_printing) debug()<<"tag hit as used "<<hit->hit()->lhcbID()<<endmsg;
        hit->hit()->setStatus( Tf::HitBase::UsedByPatMatch, true );
        break;
      }
    }
  }
}
//=========================================================================
//  Get the PreSelection
//=========================================================================
void PatLongLivedTracking::getPreSelection( PatDownTrack& track ) {

  if ( m_doTiming ) m_timerTool->start( m_preselTime );

  // - Max Pt around 100 MeV for strange particle decay -> maximum displacement is in 1/p.
  double xPredTol = m_xPredTolOffset;

  if (std::abs(track.momentum()) >  1e-6) xPredTol = m_xPredTolConst / std::abs( track.momentum() ) + m_xPredTolOffset;  // P dependance + overal tol.
  
  const double yTol = xPredTol;
  
  // -- a correction turns out to be beneficial
  // -- maybe to compensate tracks not coming from 0/0 (?)
  const double correction = xPosCorrection( track );
  
  for(int i = 0; i < 4; ++i) m_preSelHits[i].clear();
  
  m_xHits.clear();
  
  //== Collect all hits compatible with the extrapolation, region by region.
  if ( UNLIKELY( m_printing )) info() << "-- collect hits with tolerance " << xPredTol << endmsg;

  const double yTrack = track.yAtZ( 0. );
  const double tyTr   = track.slopeY();

  for(int iLayer = 0; iLayer < 4; ++iLayer){
  
    if( m_hitsLayers[iLayer].empty() ) continue;
    
    const double zLayer = m_hitsLayers[iLayer].front()->z();
    const double yPredLay   = track.yAtZ( zLayer );
    const double xPredLay = track.xAtZ( zLayer );
    const double dxDy = m_hitsLayers[iLayer].front()->hit()->dxDy();
    
    // -- this should sort of take the stereo angle and some tolerance into account.
    const double lowerBoundX = xPredLay - xPredTol - dxDy*yPredLay - 2.0;
    
    PatTTHits::iterator itHit = std::lower_bound( m_hitsLayers[iLayer].begin(), 
                                                  m_hitsLayers[iLayer].end(), 
                                                  lowerBoundX, 
                                                  Tf::compByX_LB< PatTTHit >() );
    
    for( ; itHit != m_hitsLayers[iLayer].end(); ++itHit){

      PatTTHit* hit = *itHit;
      
      if( m_debugTool && !m_debugTool->isTrueHit( track.track(), hit ) ) continue;
      

      if (hit->hit()->ignore()) continue;
      
      const double yPos   = track.yAtZ( hit->z() );
      if ( !hit->hit()->isYCompatible( yPos, yTol ) ) continue;
      
     
      updateTTHitForTrackFast( hit, yTrack, tyTr);
      const double pos    = track.xAtZ( hit->z() ) - correction;
      
      // -- go from -x to +x
      // -- can break if we go out of the right bound
      if( xPredTol < pos - hit->x() ) continue;
      if( xPredTol < hit->x() - pos ) break;
      
      hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
      hit->setProjection( fabs( hit->x()-pos ) );
    
      m_preSelHits[iLayer].push_back( hit );
      
      if ( UNLIKELY( m_printing )) {
        info() << format( "  plane%2d z %8.2f x %8.2f pos %8.2f High%2d dist %8.2f", 
                          hit->planeCode(), hit->z(), hit->x(), pos, 
                          hit->hit()->sthit()->cluster().highThreshold(), hit->x() - pos);
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endmsg;
      }
    }

    if( iLayer == 1 && m_preSelHits[0].empty() && m_preSelHits[1].empty() ) break;
    

  }


  std::sort(m_preSelHits[1].begin(), m_preSelHits[1].end(), Tf::increasingByProjection<PatTTHit>() );
  std::sort(m_preSelHits[2].begin(), m_preSelHits[2].end(), Tf::increasingByProjection<PatTTHit>() );

  // -- this is a little weird, but having a common vector of x-hits is better
  m_xHits = m_preSelHits[0];
  m_xHits.insert( m_xHits.end(), m_preSelHits[3].begin(), m_preSelHits[3].end() );
  std::sort( m_xHits.begin(),  m_xHits.end(),  Tf::increasingByProjection<PatTTHit>() );
  
  if ( m_doTiming ) m_timerTool->stop( m_preselTime );

}
//=========================================================================
//  Fit and remove the worst hit, as long as over tolerance
//=========================================================================
void PatLongLivedTracking::fitAndRemove ( PatDownTrack& track, bool onlyFit ) {
  
  if ( m_doTiming ) m_timerTool->start( m_fitAndRemoveTime );

  if ( 2 > track.hits().size() ){
    if ( m_doTiming ) m_timerTool->stop( m_fitAndRemoveTime );
    return;  // no fit if single point !
  }
  
  bool again = false;
  do {
    again = false;
    //== Fit, using the magnet point as constraint.
    double mat[6], rhs[3];
    mat[0] = 1./( track.errXMag() * track.errXMag() );
    mat[1] = 0.;
    mat[2] = 0.;
    mat[3] = 0.;
    mat[4] = 0.;
    mat[5] = 0.;
    rhs[0] = mat[0] * track.dxMagnet();//( m_magnetSave.x() - m_magnet.x() );
    rhs[1] = 0.;
    rhs[2] = 0.;
    int nbUV = 0;

    std::array<int,4> differentPlanes = {0, 0, 0, 0 };
    unsigned int nDoF = 0;
    
    double yTrack = track.yAtZ( 0. );
    double tyTr   = track.slopeY();
  
    PatTTHits::const_iterator itEnd = track.hits().end();
    for(PatTTHits::iterator iHit = track.hits().begin() ; iHit != itEnd; ++iHit) {

      PatTTHit* hit = *iHit;
      
      if( !onlyFit ) updateTTHitForTrackFast( hit, yTrack, tyTr);
      const double dz   = hit->z() - track.zMagnet();
      const double dist = track.distance( hit );
      const double w    = hit->hit()->weight();
      const double t    = hit->hit()->sinT();          
      mat[0] += w;
      mat[1] += w * dz;
      mat[2] += w * dz * dz;
      mat[3] += w * t;
      mat[4] += w * dz * t ;
      mat[5] += w * t  * t ;
      rhs[0] += w * dist;
      rhs[1] += w * dist * dz;
      rhs[2] += w * dist * t ;
    
      // -- check how many different layers have fired
      //if ( 0 == differentPlanes[hit->planeCode()]++ ) ++nDoF;
      differentPlanes[hit->planeCode()]++;
      nbUV += addIsStereo( hit );
      
      
      if ( UNLIKELY( m_printing )) {
        info() << format( "   Plane %2d x %7.2f dist %6.3f ", 
                          hit->planeCode(), hit->x(), dist );
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endmsg;
      }
    }

    nDoF = std::count_if(differentPlanes.begin(), differentPlanes.end(), [](const int a){ return a > 0; });
    track.setFiredLayers( nDoF );
    
    // -- solve the equation and update the parameters of the track
    CholeskyDecomp<double, 3> decomp(mat);
    if (!decomp) {
      track.setChi2(1e42);
      if ( m_doTiming ) m_timerTool->stop( m_fitAndRemoveTime );
      return;
    } else {
      decomp.Solve(rhs);
    }
    
    const double dx  = rhs[0];
    const double dsl = rhs[1];
    const double dy  = rhs[2];
    
    if ( UNLIKELY( m_printing )) {
      info() << format( "  dx %7.3f dsl %7.6f dy %7.3f, displY %7.2f", 
                        dx, dsl, dy, track.displY() ) << endmsg;
    }
    
    if ( 4 > nbUV ) track.updateX( dx, dsl );
    track.setDisplY( track.displY() + dy );

    //== Remove worst hit and retry, if too far.
    double chi2 = track.initialChi2();
    
    double maxDist = -1.;
    PatTTHits::iterator worst;
    
    yTrack = track.yAtZ( 0. );
    tyTr   = track.slopeY();

    itEnd = track.hits().end();
    for (PatTTHits::iterator itH = track.hits().begin();
         itH != itEnd; ++itH){
      
      PatTTHit* hit = *itH;
      updateTTHitForTrackFast( hit, yTrack, tyTr);
      const double dist = track.distance( hit );
      
      // --
      if ( UNLIKELY( m_printing )) {
        info() << format( "   Plane %2d x %7.2f dist %6.3f ", 
                          hit->planeCode(), hit->x(), dist );
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endmsg;
      }
      // --
      chi2 += dist * dist * hit->hit()->weight();
      // -- only flag this hit as removable if it is not alone in a plane or there are 4 planes that fired
      if ( maxDist < fabs(dist) &&  (1 < differentPlanes[hit->planeCode()] || nDoF == track.hits().size() ) ) {
        maxDist = fabs( dist );
        worst = itH;
      }
    }
    
    if ( 2 < track.hits().size() ) chi2 /= (track.hits().size() - 2 );
    track.setChi2( chi2 );

    if(onlyFit){
      if ( m_doTiming ) m_timerTool->stop( m_fitAndRemoveTime );
      return;
    }
    
    // -- remove if Y is incompatible
    for (PatTTHits::iterator itH = track.hits().begin();
         itH != track.hits().end(); ++itH){
      PatTTHit* hit = *itH;
      double yTrack = track.yAtZ( hit->z() );
      if( hit->hit()->isYCompatible(yTrack, m_yTol ) ) continue;
      
      if ( UNLIKELY( m_printing )) {
        info() << "   remove Y incompatible hit measure = " << hit->x() 
               << " : y " << yTrack << " min " << hit->hit()->yMin()
               << " max " << hit->hit()->yMax() << endmsg;
      }
      track.hits().erase( itH );
      maxDist = 0.;  // avoid deleting worst, may be already gone...
      if ( 2 < track.hits().size() ) again = true;
      break;
    }
    
    if ( again ) continue;
   
    
    
    if ( m_maxChi2 < chi2 && track.hits().size() > 3) {
      track.hits().erase( worst );
      again = true;
      if ( UNLIKELY( m_printing )) info() << "   remove worst and retry" << endmsg;
      
    } 
    
    if ( UNLIKELY( m_printing )) {
      info() << format( "  ---> chi2 %7.2f maxDist %7.3f", chi2, maxDist) << endmsg;
    }    
  } while (again);

  if ( m_doTiming ) m_timerTool->stop( m_fitAndRemoveTime );

}
//=========================================================================
//  Fit hits in x layers
//=========================================================================
void PatLongLivedTracking::xFit( PatDownTrack& track ) {
  
  if ( m_doTiming ) m_timerTool->start( m_xFitTime );

  if ( 2 > track.hits().size() ){
    if ( m_doTiming ) m_timerTool->stop( m_xFitTime );
    return;  // no fit if single point !
  }
  
  //== Fit, using the magnet point as constraint.
  double mat[6], rhs[3];
  mat[0] = 1./( track.errXMag() * track.errXMag() );
  mat[1] = 0.;
  mat[2] = 0.;
  rhs[0] = mat[0] * track.dxMagnet();//( m_magnetSave.x() - m_magnet.x() );
  rhs[1] = 0.;
  
  const PatTTHit* hit1 = track.hits()[0];
  const PatTTHit* hit2 = track.hits()[1];
  const double w1 = hit1->hit()->weight();
  const double w2 = hit2->hit()->weight();


  mat[0] = w1 + w2;
  mat[1] = w1 * (hit1->z() - track.zMagnet()) +  w2 * (hit2->z() - track.zMagnet());
  mat[2] = w1 * (hit1->z() - track.zMagnet())*(hit1->z() - track.zMagnet()) 
    + w2 * (hit2->z() - track.zMagnet())*(hit2->z() - track.zMagnet());
  
  rhs[0] = w1 *  track.distance( hit1 ) + w2 *  track.distance( hit2 );
  rhs[1] = w1 *  track.distance( hit1 ) * (hit1->z() - track.zMagnet()) 
    +  w2 *  track.distance( hit2 ) * (hit2->z() - track.zMagnet());
  
  CholeskyDecomp<double, 2> decomp(mat);
  if (!decomp) {
    track.setChi2(1e42);
    if ( m_doTiming ) m_timerTool->stop( m_xFitTime );
    return;
  } else {
    decomp.Solve(rhs);
  }
        
  const double dx  = rhs[0];
  const double dsl = rhs[1];
  
  track.updateX( dx, dsl );
  
  const double chi2 = track.initialChi2() + w1* track.distance( hit1 )* track.distance( hit1 ) 
    +  w2* track.distance( hit2 )* track.distance( hit2 );
  
  track.setChi2( chi2 );
    
  if ( m_doTiming ) m_timerTool->stop( m_xFitTime );
  
}
//=========================================================================
//  Collect the hits in the other x layer
//=========================================================================
void PatLongLivedTracking::findMatchingHits( const PatDownTrack& track, const int plane) {
  
  if ( m_doTiming ) m_timerTool->start( m_findMatchingHitsTime );

  m_matchingXHits.clear();
  //search window = const1/momentum + const2
  double tol = (std::abs(track.state()->p() / m_tolMatchConst) < 1. / (m_maxWindow - m_tolMatchOffset)) ?
    m_maxWindow : (m_tolMatchOffset + m_tolMatchConst / track.state()->p());
  
  int planeToConsider = 0;
  if( plane == 0 ) planeToConsider = 3;
  const double xPred = track.xAtZ( m_preSelHits[planeToConsider].front()->z() );
  
  PatTTHits::iterator it = std::lower_bound( m_preSelHits[planeToConsider].begin(), 
                                             m_preSelHits[planeToConsider].end(), xPred - tol, 
                                             [](const PatTTHit* hit, const double uVal){ return hit->x() < uVal; });
  
  for( ; it != m_preSelHits[planeToConsider].end(); ++it ){
    PatTTHit* hit = *it;
    
    const double adist = std::abs( hit->x() - xPred );
    if ( adist > tol ) break;
    m_matchingXHits.push_back( hit );
  }
  
  std::sort( m_matchingXHits.begin(), m_matchingXHits.end(), 
             [xPred](const PatTTHit* lhs, const PatTTHit* rhs){ return std::abs(lhs->x() - xPred) < std::abs(rhs->x() - xPred);} );
  
  if ( m_doTiming ) m_timerTool->stop( m_findMatchingHitsTime );

}
//=========================================================================
//  Add the U hits.
//=========================================================================
void PatLongLivedTracking::addUHits ( const PatDownTrack& track, const unsigned int maxNumTracks ) {

  if ( m_doTiming ) m_timerTool->start( m_addUHitsTime );

  m_goodXUTracks.clear();

  const double tol = m_tolUOffset + m_tolUConst / std::abs(track.momentum());
  
  // -- these numbers are a little arbitrary
  double minChi2 = 300;
  if( track.hits().size() == 1 ) minChi2 = 800;
  
  const double yTrack = track.yAtZ( 0. );
  const double tyTr   = track.slopeY();
 
  m_uHitsTemp.clear();
  
  // -- first select all hits, and then
  // -- accept until over a tolerance
  for(PatTTHit* hit: m_preSelHits[1]) {
    if( m_preSelHits[1].empty() ) break;
    updateTTHitForTrackFast( hit, yTrack, tyTr);
   
    const double dist = std::abs( track.distance( hit ) );
    if ( dist > tol ) continue;
    hit->setProjection( dist );
    m_uHitsTemp.push_back( hit );
  }
  
  if( m_uHitsTemp.empty() ) return;
  

  std::sort( m_uHitsTemp.begin(), m_uHitsTemp.end(), Tf::increasingByProjection<PatTTHit>());

  const double slopeX = track.slopeX();
  const double displX = track.displX();
  const double magnetX = track.xMagnet();

  PatDownTrack greatTrack( track );

  for( PatTTHit* hit : m_uHitsTemp){
    
    greatTrack.startNewXUCandidate(slopeX, displX, magnetX);
  
    greatTrack.hits().push_back( hit );
    fitAndRemove( greatTrack, true );
  
    // -- it's sorted
    if ( greatTrack.chi2() > minChi2 ) break;
    if ( m_goodXUTracks.size() < maxNumTracks-1){
      m_goodXUTracks.push_back( greatTrack );
      greatTrack.hits().pop_back();
    }else{
      m_goodXUTracks.push_back( std::move(greatTrack) );
      break;
    }
    
  }
  
  if ( m_doTiming ) m_timerTool->stop( m_addUHitsTime );
  
}
//=========================================================================
//  Add the V hits. Take the one which has the best chi2
//=========================================================================
void PatLongLivedTracking::addVHits ( PatDownTrack& track ) {

  if ( m_doTiming ) m_timerTool->start( m_addVHitsTime );

  if( m_preSelHits[2].empty() ){
    if ( m_doTiming ) m_timerTool->stop( m_addVHitsTime );
    return;
  }
  
  double tol = m_tolVOffset + m_tolVConst/std::abs( track.momentum() );
  if( track.hits().size() == 2 )  tol = m_tolUOffset + m_tolUConst / std::abs(track.momentum());

  const double yTrack = track.yAtZ( 0. );
  const double tyTr   = track.slopeY();

  double minChi2 = 10000;
  
  PatTTHit* bestHit = nullptr;
  for(PatTTHit* hit: m_preSelHits[2]) {
    
    updateTTHitForTrackFast( hit, yTrack, tyTr);
    const double adist = std::abs( track.distance( hit ) );
    
    if( adist < tol ){
      track.hits().push_back( hit );
      fitAndRemove( track, true );
      track.hits().pop_back();
      
      if( track.chi2() < minChi2){
        bestHit = hit;
        minChi2 = track.chi2();
      }
    }
  }

  if( bestHit != nullptr) track.hits().push_back( bestHit );
  
  track.sortFinalHits();
  
  if ( m_doTiming ) m_timerTool->stop( m_addVHitsTime );

}
//=========================================================================
//  Check if the new candidate is better than the old one
//=========================================================================
bool PatLongLivedTracking::acceptCandidate( PatDownTrack& track, int& maxPoints ){

  if ( m_doTiming ) m_timerTool->start( m_acceptCandidateTime );

  const int nbMeasureOK = track.hits().size();
  
  // -- use a tighter chi2 for 3 hit tracks
  // -- as they are more likely to be ghosts
  double maxChi2 = m_maxChi2;
  if( track.hits().size() == 3 ) maxChi2 = m_maxChi2ThreeHits;
  

  //== Enough mesures to have Chi2/ndof.
  if ( 3 > nbMeasureOK ) {
    if ( m_printing ) info() << " === not enough points" << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }

  if( 3 > track.firedLayers() ){
    if ( m_printing ) info() << " === not enough firedLayers" << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }

  //== Good enough Chi2/ndof
  if ( maxChi2 < track.chi2() ) {
    if ( m_printing ) info() << " === Chisq too big " << track.chi2() << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
  
  const double deltaP = track.momentum() * track.state()->qOverP() - 1.;
  //== Compatible momentum
  if ( maxDeltaP( track ) < fabs(deltaP) ) {
    if ( m_printing ) info() << " === Deltap too big " << deltaP << endmsg;
    if ( !m_magnetOff ){
      if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
      return false;
    }
  }

  //== Longest -> Keeep it
  if ( maxPoints > nbMeasureOK ) {
    if ( m_printing ) info() << " === less points than previous" << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
  

  //== Count if enough with high threshold
  int nbHigh = 0;
  int nbUsed = 0;

  for(const PatTTHit* hit: track.hits()) {
    if ( hit->hit()->sthit()->cluster().highThreshold() ) ++nbHigh;
    if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream )  ) ++nbUsed;
  }
  
  if ( 2 > nbHigh ) {
    if ( m_printing ) info() << " === not enough high threshold points" << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
  if ( nbMeasureOK == nbUsed ) {
    if ( m_printing ) info() << " === is a clone" << endmsg;
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
        
  if ( m_printing ) {
    info() << format( "  *** Good candidate ***  slope%8.2f displX%8.2f Y%8.2f Chi2%8.2f", 
                      1000.*track.slopeX(), track.displX(), track.displY(), track.chi2() );
  }

  //== Better candidate. 
  maxPoints = nbMeasureOK;
  if ( maxPoints > 4 ) maxPoints = 4;
  
  //== calculate pt and p
  const double momentum = std::abs(track.momentum());
  const double tx2 = track.slopeX()*track.slopeX();
  const double ty2 = track.slopeY()*track.slopeY();
  const double sinTrack = sqrt( 1. - 1./(1.+tx2 + ty2) );
  const double pt = sinTrack*momentum;
  
  if (momentum<m_minMomentum){
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
  
  if (pt<m_minPt){
    if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
    return false;
  }
  
  track.sortFinalHits();
  
  for( PatTTHit* hit: track.hits() ) {
    hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, true );
  }
  
      
  if ( UNLIKELY( m_printing )) {
    for(PatTTHit* hit: track.hits()) {
      LHCb::STChannelID icID = hit->hit()->lhcbID().stID();
      double xCoord = hit->x() ;
      info() << "      TT Clus " 
             << format( "(S%1d,L%1d,R%2d,S%2d,S%3d) x%7.1f  dist%7.3f High %1d", 
                        icID.station(), icID.layer(), icID.detRegion(), 
                        icID.sector(), icID.strip(), xCoord,
                        track.distance( hit ), hit->hit()->sthit()->cluster().highThreshold() ) ;
      if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
      info() << endmsg;
    }
  }
  
  if ( m_doTiming ) m_timerTool->stop( m_acceptCandidateTime );
  
  return true;
}

//=========================================================================
//  Store Track
//=========================================================================
void PatLongLivedTracking::storeTrack( PatDownTrack& track, LHCb::Tracks* finalTracks, LHCb::Track* tr ){
  
  if ( m_doTiming ) m_timerTool->start( m_storeTrackTime );

  //=== Store the tracks
  //== new LHCb::Track
  LHCb::Track* dTr = new LHCb::Track();
  //== add ancestor
  dTr->addToAncestors( tr );   
  //== Adjust flags
  dTr->setType(         LHCb::Track::Downstream  );
  dTr->setHistory(      LHCb::Track::PatDownstream   );
  dTr->setPatRecStatus( LHCb::Track::PatRecIDs   );
  //== add Seed LHCbIDs
  dTr->addSortedToLhcbIDs(  tr->lhcbIDs()      );  
  //== add new LHCbIDs
  for ( const PatTTHit* hit : track.hits() ){
    dTr->addToLhcbIDs( hit->hit()->lhcbID() );
  }
  
  
  //== add states
  // S. Stahl added 3 T-States
  
  // check for FPE and magnet off
  double QOverP = 1e-5;
  if (std::abs(track.momentum()) >  1e-6){
      QOverP = 1.0 / track.momentum();
  }

  //== create a state at zTTa
  LHCb::State ttState;
  ttState.setState( track.xAtZ( m_zTTa ),
                    track.yAtZ( m_zTTa ),
                    m_zTTa,
                    track.slopeX(),
                    track.slopeY(),
                    QOverP );
  Gaudi::TrackSymMatrix cov;
  cov(0,0) = m_stateErrorX2;
  cov(1,1) = m_stateErrorY2;
  cov(2,2) = m_stateErrorTX2;
  cov(3,3) = m_stateErrorTY2;
  double errQOverP = m_stateErrorP * QOverP;
  cov(4,4) = errQOverP * errQOverP;
  
  ttState.setCovariance( cov );
  dTr->addToStates( ttState );   
  
  //== add seed states
  std::vector<LHCb::State*> newstates;
  newstates.reserve(3);
  newstates.push_back(tr->closestState(StateParameters::ZBegT).clone());
  newstates.push_back(tr->closestState(StateParameters::ZMidT).clone());
  // make sure we don't include same state twice
  if (std::abs(newstates[newstates.size() - 2]->z() -
               newstates.back()->z()) < 300.) {
    delete newstates.back();
    newstates.pop_back();
  }
  newstates.push_back(tr->closestState(StateParameters::ZEndT).clone());
  // make sure we don't include same state twice
  if (std::abs(newstates[newstates.size() - 2]->z() -
               newstates.back()->z()) < 300.) {
    delete newstates.back();
    newstates.pop_back();
  }
  
  // adjust q/p and its uncertainty
  for(LHCb::State* st: newstates) {
    st->covariance()(4,4) = errQOverP * errQOverP;
    st->setQOverP(QOverP);
  }
  dTr->addToStates(newstates);

  //== Save track
  finalTracks->insert( dTr );

  


  if ( m_doTiming ) m_timerTool->stop( m_storeTrackTime );

}

//=============================================================================
// Fit the projection in the zx plane, one hit in each x layer
//=============================================================================
void PatLongLivedTracking::fitXProjection( PatDownTrack& track, PatTTHit* firstHit){

  if ( m_doTiming ) m_timerTool->start( m_fitXProjectionTime );

  m_goodXTracks.clear();
  

  // Catch if there is no second hit in other station
  PatDownTrack newTrack( track );
  
  PatTTHits::const_iterator itEnd = m_matchingXHits.end();
  for( PatTTHits::const_iterator iHit = m_matchingXHits.begin(); iHit != itEnd; ++itEnd) {
    PatTTHit* hit = *iHit;
    
    newTrack.startNewXCandidate( firstHit );
    newTrack.hits().push_back( hit );
    xFit( newTrack );
    
    // -- It is sorted according to the projection
    // -- the chi2 will therefore only increase
    if( newTrack.chi2() > m_maxChi2XProjection) break;

    // -- We can only move the last one
    // -- as the tracks before are 'recycled'
    if( m_goodXTracks.size() < m_maxXTracks-1){
      m_goodXTracks.push_back( newTrack );
    }else{
      m_goodXTracks.push_back( std::move(newTrack) );
      break;
    }
    
  }
  
  // -- If no suitable hit has been found
  // -- we just add the first one and make
  // -- it a track.
  if( m_goodXTracks.empty() ){
    track.hits().push_back( firstHit );
    m_goodXTracks.push_back( track );
    if ( m_doTiming ) m_timerTool->stop( m_fitXProjectionTime );
    return;
  }
  
  std::sort( m_goodXTracks.begin(), m_goodXTracks.end(), 
             [](const PatDownTrack& a, const PatDownTrack& b){ return a.chi2() < b.chi2();});
  
  if ( m_doTiming ) m_timerTool->stop( m_fitXProjectionTime );

}
//=============================================================================
// This is needed for tracks which have more than one x hit in one layer
// Maybe we could make this smarter and do it for every track and add the 'second best'
// this, such that we do not need to loop over them again
//=============================================================================
void PatLongLivedTracking::addOverlapRegions( PatDownTrack& track ){
  
  if ( m_doTiming ) m_timerTool->start( m_overlapTime );

  bool hitAdded = false;
  
  const double yTrack = track.yAtZ( 0. );
  const double tyTr   = track.slopeY();
  
  for(int i = 0; i < 4; ++i){
    for( PatTTHit* hit : m_preSelHits[i] ){
     
      updateTTHitForTrackFast( hit, yTrack, tyTr );

      if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream )) continue;
      if ( m_overlapTol > std::abs( track.distance( hit ) ) ) {
        double yTrack = track.yAtZ( hit->z() );

        if ( !hit->hit()->isYCompatible( yTrack, m_yTol ) ) continue;
                
        // -- check that z-position is different
        bool addHit = true;
        for(const PatTTHit* trackHit : track.hits() ){
          if( trackHit->planeCode() != hit->planeCode() ) continue;
          // -- the displacement in z between overlap modules is larger than 1mm
          if( std::abs( hit->z() - trackHit->z() ) < 1.0 ) addHit = false;
        }
  
        // -------------------------------------
        if(addHit){
          track.hits().push_back( hit );
          hitAdded = true;
        }
        
      }
    }
  }
  
  if ( hitAdded ) {
    track.sortFinalHits();
    fitAndRemove( track, false );
  }

  if ( m_doTiming ) m_timerTool->stop( m_overlapTime );

}
//=============================================================================
// Evaluate the Fisher discriminant for the input tracks 
//=============================================================================
double PatLongLivedTracking::evaluateFisher( const LHCb::Track* track ){

  const unsigned int nbIT = std::count_if( track->lhcbIDs().begin(), track->lhcbIDs().end(), 
                                           [](const LHCb::LHCbID id){ return id.isIT();});
  double nbITD =  static_cast<double>(nbIT);
  double lhcbIDSizeD = static_cast<double>(track->lhcbIDs().size());
  std::array<double,5> vals = { track->chi2PerDoF(), track->p(), track->pt(), nbITD, lhcbIDSizeD };

  return getFisher( vals );

}
//=============================================================================
// -- Init event: Get the new hits and sort them
//=============================================================================
void PatLongLivedTracking::initEvent () {
 
  m_ttHitManager->prepareHits();
  
  
  m_hitsLayers[0].clear();
  m_hitsLayers[1].clear();
  m_hitsLayers[2].clear();
  m_hitsLayers[3].clear();

  Tf::TTStationHitManager<PatTTHit>::HitRange range = m_ttHitManager->hits(0,0);
  m_hitsLayers[0].insert(m_hitsLayers[0].end(), range.begin(), range.end() );
  range = m_ttHitManager->hits(0,1);
  m_hitsLayers[1].insert(m_hitsLayers[1].end(), range.begin(), range.end() );
  range = m_ttHitManager->hits(1,0);
  m_hitsLayers[2].insert(m_hitsLayers[2].end(), range.begin(), range.end() );
  range = m_ttHitManager->hits(1,1);
  m_hitsLayers[3].insert(m_hitsLayers[3].end(), range.begin(), range.end() );

  for(int i = 0; i < 4; ++i){
    std::sort(m_hitsLayers[i].begin(), m_hitsLayers[i].end(), [](const PatTTHit* lhs, const PatTTHit* rhs){
        return lhs->hit()->xAtYEq0() < rhs->hit()->xAtYEq0();
      });
  }

}
