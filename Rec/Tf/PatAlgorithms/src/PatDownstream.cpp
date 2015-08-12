// Include files 

#include <algorithm>
#include <cmath>


// from ROOT
#include <Math/CholeskyDecomp.h>
using ROOT::Math::CholeskyDecomp;

// from Gaudi
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
#include "PatDownstream.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDownstream
//
// 2005-11-18 : Olivier Callot
// 2009-10-07 : Sascha Stahl
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatDownstream )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDownstream::PatDownstream( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_downTime( 0 ),
    m_printing( false ),
    m_magnetOff( false ),
    m_magFieldSvc(NULL),
    m_ttHitManager(NULL),
    m_debugTool(NULL),
    m_timerTool(NULL)    
{
  declareProperty( "InputLocation" , m_inputLocation  = ""    );
  declareProperty( "OutputLocation", m_outputLocation = LHCb::TrackLocation::Downstream );
  declareProperty( "ForwardLocation", m_forwardLocation = LHCb::TrackLocation::Forward );
  declareProperty( "MatchLocation", m_matchLocation = LHCb::TrackLocation::Match );
  declareProperty( "PrintTracks"   , m_printTracks    = false );
  declareProperty( "TimingMeasurement", m_doTiming = false);
  declareProperty( "deltaP"        , m_deltaP        = 2.0  );
  declareProperty( "xPredTol"      , m_xPredTol      = 150. * Gaudi::Units::mm * Gaudi::Units::GeV  );
  declareProperty( "xPredTol2"      , m_xPredTol2      = 20. * Gaudi::Units::mm );
  declareProperty( "TolMatch"      , m_tolX          = 1.5 *  Gaudi::Units::mm );
  declareProperty( "TolUV"         , m_tolUV         = 2.0 *  Gaudi::Units::mm );
  declareProperty( "TolMomentum"   , m_tolMomentum   = 20000.    );
  declareProperty( "maxWindowSize" , m_maxWindow     = 10.0 * Gaudi::Units::mm );
  declareProperty( "MaxDistance"   , m_maxDistance   = 0.30 * Gaudi::Units::mm );
  declareProperty( "MaxChi2"       , m_maxChi2       = 20.        );
  declareProperty( "minTTx"        , m_minTTx        = 35. *  Gaudi::Units::mm );
  declareProperty( "minTTy"        , m_minTTy        = 35. *  Gaudi::Units::mm );
  // Define parameters for MC09 field, zState = 9410
  declareProperty( "zMagnetParams" , m_zMagnetParams  = {5372.13, -3702.66, 401.378, 109033, -0.0119398, -0.158909, 1668.42} );
  declareProperty( "momentumParams", m_momentumParams = {1148.65,961.786,5326.81});
  declareProperty( "yParams"       , m_yParams        = {5.,2000.});

  declareProperty( "zTT"           , m_zTT           = 2485.* Gaudi::Units::mm );
  declareProperty( "zTTa"          , m_zTTa          = 2350.* Gaudi::Units::mm );

  declareProperty( "StateErrorX2"  , m_stateErrorX2  =   4.0);
  declareProperty( "StateErrorY2"  , m_stateErrorY2  = 400.);
  declareProperty( "StateErrorTX2" , m_stateErrorTX2 =  6.e-5);
  declareProperty( "StateErrorTY2" , m_stateErrorTY2 =  1.e-4);
  declareProperty( "StateErrorP"   , m_stateErrorP   =  0.15);
  declareProperty( "MinPt"                 , m_minPt                 =  0. * Gaudi::Units::MeV );
  declareProperty( "MinMomentum"           , m_minMomentum           =  0. * Gaudi::Units::GeV );

  // Change this in order to remove hits and T-tracks used for longtracks.
  // RemoveAll configures that everything is removed.
  // If false only hits and T-tracks from good longtracks are removed.
  // The criterion for this is the Chi2 of the longtracks from the fit.
  declareProperty( "RemoveUsed"    , m_removeUsed    = false     );
  declareProperty( "RemoveAll"    ,  m_removeAll = false      );
  declareProperty( "LongChi2"      , m_longChi2     = 1.5       );
  
  //== debugging options
  declareProperty( "SeedKey"       , m_seedKey       = -1    );
  declareProperty( "WithDebugTool" , m_withDebugTool = false );
  declareProperty( "DebugTool"     , m_debugToolName = "PatDebugTTTruthTool" );
}
//=============================================================================
// Destructor
//=============================================================================
PatDownstream::~PatDownstream() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatDownstream::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  m_debugTool = 0;
  if ( m_withDebugTool ) {
    m_debugTool = tool<IPatDebugTTTool>( m_debugToolName );
  }  

  info() << "========================================"  << endmsg
         << " deltaP             = " << m_deltaP        << endmsg
         << " xPredTol           = " << m_xPredTol      << endmsg
         << " TolMomentum        = " << m_tolMomentum   << endmsg
         << " TolMatch           = " << m_tolX          << endmsg
         << " TolUV              = " << m_tolUV         << endmsg
         << " MaxDistance        = " << m_maxDistance   << endmsg
         << " MaxChi2            = " << m_maxChi2      << endmsg
         << " MaxWindowSize      = " << m_maxWindow     << endmsg
         << " RemoveUsed         = " << m_removeUsed    << endmsg
         << " RemoveAll          = " << m_removeAll     << endmsg
         << " LongChi2           = " << m_longChi2      << endmsg     
         << "========================================"  << endmsg;
  
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
    m_timerTool = tool<ISequencerTimerTool>( "SequencerTimerTool" );
    m_timerTool->increaseIndent();
    m_downTime = m_timerTool->addTimer( "Internal PatDownstream" );
    m_timerTool->decreaseIndent();
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatDownstream::execute() {
      
  if ( m_doTiming ) m_timerTool->start( m_downTime );

  m_printing = msgLevel( MSG::DEBUG );  
  if (m_printing) debug() << "==> Execute" << endmsg;
  
  //== if no specified input location, get seed if it exists, else Tsa.
  if ( m_inputLocation.empty() ) {
    if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Seed ) ) {
      m_inputLocation = LHCb::TrackLocation::Seed;
    } else {
      m_inputLocation = LHCb::TrackLocation::Tsa;
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
  

  if ( m_printing ) debug() << "-- Started from " << inTracks->size() << " Seed, left with " 
                    << myInTracks.size() << " tracks after Matched removal." << endmsg;

  //==========================================================================
  // Get the output container ( created by PatDataStore )
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
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();

  std::array<double,7> magPars = { m_zMagnetParams[0], m_zMagnetParams[1], m_zMagnetParams[2], m_zMagnetParams[3], 
                                   m_zMagnetParams[4], m_zMagnetParams[5], m_zMagnetParams[6] };
  std::array<double,3> momPars = { m_momentumParams[0], m_momentumParams[1], m_momentumParams[2] };


  
  for ( LHCb::Track* tr : myInTracks ) {
    
    if ( 0 <= m_seedKey && m_seedKey == tr->key() ) m_printing = true;
    
    PatDownTrack track( tr, m_zTT, magPars, momPars, m_yParams, magScaleFactor*(-1) );
    
    // -- Veto particles coming from the beam pipe.
    if( insideBeampipe( track ) ) continue;
    

    const double deltaP = track.momentum() * track.state()->qOverP() - 1.;
    
    if ( m_printing ) {
      for ( PatTTHit* hit : ttCoords ){
        if (hit->hit()->ignore()) continue;
        const double yTrack = track.yAtZ( 0. );
        const double tyTr   = track.slopeY();
        updateTTHitForTrack( hit, yTrack, tyTr );
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
    
    // check for compatible momentum
    if ( m_deltaP < fabs(deltaP) ) {
      if ( m_printing ) info() << "   --- deltaP " << deltaP << " -- rejected" << endmsg;
      if ( !m_magnetOff ) continue;
      //continue;
    }
    
    // Get hits in TT around a first track estimate
    getPreSelection( track );
    
    int nbOK = 0;
    PatDownTrack bestTrack( track );
    PatTTHits MatchingXHits;
    MatchingXHits.reserve(16);
    int maxPoints = 0;
    double minChi2 = m_maxChi2 + 10.;
    
    if ( 3 > ( m_xHits.size() +  m_uvHits.size() ) ) continue;
    
    //==============================================================
    // Try to find a candidate: X first, then UV.
    //==============================================================
    for( PatTTHit* myHit : m_xHits ){
      
      if ( myHit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream ) ) continue;
      double meanZ = myHit->z();
      double posX  = myHit->x( );
      int myPlane  = myHit->planeCode();
      
      track.startNewCandidate();
      
      // Create track estimate with one x hit
      double slopeX = (track.xMagnet() - posX + track.sagitta( meanZ)) / (track.zMagnet() - meanZ);
      track.setSlopeX( slopeX );
      
      if ( m_printing ) {
        const double tolMatch = (std::abs(track.state()->p() / m_tolMomentum) < 1. / (m_maxWindow - m_tolX)) ?
          m_maxWindow : (m_tolX + m_tolMomentum / track.state()->p());
        info() << endmsg 
               << format( "... start plane %1d x%8.2f z%8.1f slope%8.2f tolMatch%7.3f",
                          myPlane, posX, meanZ, 1000. * slopeX, tolMatch )
               << endmsg;
      }        
      
      // Fit x projection
      findMatchingHits( MatchingXHits, track, myPlane );
      fitXProjection( MatchingXHits, track, myHit );
      
      // Fit 3D track
      addUVHits( track );
      if ( 3 > track.hits().size() ) {
        if ( m_printing ) info() << " === not enough hits" << endmsg;
        continue;
      }
      fitAndRemove( track );
        
      // Check if candidate is better than the old one
      if ( !acceptCandidate( track, bestTrack, maxPoints, minChi2 ) ) continue;
      
      // New best track
      bestTrack = track;
      nbOK++;
    }
    
    
    //== Debug the track.
    if ( 0 == nbOK ) {
      if ( m_printing ) {
        info() << format( "Track %3d P=%7.2f GeV --- discarded, not enough planes",
                          tr->key(),  .001*track.momentum() ) << endmsg;
      }
    } else {
      
      // Best one  
      track = bestTrack;
      
      // add hits in x layers in overlap regions of TT
      addOverlapRegions( track );     
      
      
      //=== Store the track
      storeTrack( track, finalTracks, tr );
      
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
StatusCode PatDownstream::finalize() {
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Cleanup already used T-Seeds
//=========================================================================
void PatDownstream::prepareSeeds(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks){
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
void PatDownstream::ttCoordCleanup ( ) {
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();
  for(PatTTHit* hit: ttCoords)
    hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
  
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
void PatDownstream::tagUsedTT( const LHCb::Track* tr ) {
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
void PatDownstream::getPreSelection( PatDownTrack& track ) {
  // Max Pt around 100 MeV for strange particle decay -> maximum displacement is in 1/p.
  double xPredTol = m_xPredTol2;
  if (std::abs(track.momentum()) >  1e-6)  
      xPredTol = m_xPredTol / fabs( track.momentum() ) + m_xPredTol2;  // P dependance + overal tol.
  double yTol = xPredTol;
  
  m_xHits.clear();
  m_uvHits.clear();
  
  //== Collect all hits compatible with the extrapolation, region by region.
  if ( m_printing ) info() << "-- collect hits with tolerance " << xPredTol << endmsg;
  
  for ( int kSta = 0; m_ttHitManager->maxStations() > kSta; ++kSta ) {
    for ( int kLay = 0; m_ttHitManager->maxLayers() > kLay; ++kLay ) {
      for ( int kReg = 0; m_ttHitManager->maxRegions() > kReg; ++kReg ) {
        const Tf::ITTHitCreator::STRegion* reg = m_ttHitManager->region( kSta, kLay, kReg );
        const double yReg = track.yAtZ( reg->z() );
        if ( !reg->isYCompatible( yReg, yTol ) ) continue;
        const double xReg = track.xAtZ( reg->z() );
        if ( !reg->isXCompatible( xReg, xPredTol ) ) continue;
        Tf::TTStationHitManager<PatTTHit>::HitRange coords = m_ttHitManager->hits( kSta, kLay, kReg );
        for(PatTTHit* hit: coords) {
          if (hit->hit()->ignore()) continue;
          if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatMatch ) ) {
            if (m_printing) debug()<<"Skip hit "<<hit->hit()->lhcbID()<<endmsg;
            continue;
          }
          
          const double yPos   = track.yAtZ( hit->z() );
          if ( !hit->hit()->isYCompatible( yPos, yTol ) ) continue;
          
          const double yTrack = track.yAtZ( 0. );
          const double tyTr   = track.slopeY();
          updateTTHitForTrack( hit, yTrack, tyTr );
          const double pos    = track.xAtZ( hit->z() );
          if ( xPredTol < fabs( pos - hit->x() ) ) continue;
          hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
          hit->setProjection( fabs( hit->x()-pos ) );
          
          // check if x or stereo hit
          if( isStereo(hit) ){
            m_uvHits.push_back( hit );
          }else{
            m_xHits.push_back( hit );
          }
          
          if ( m_printing ) {
            info() << format( "  plane%2d z %8.2f x %8.2f pos %8.2f High%2d dist %8.2f", 
                              hit->planeCode(), hit->z(), hit->x(), pos, 
                              hit->hit()->sthit()->cluster().highThreshold(), hit->x() - pos);
            if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
            info() << endmsg;
          }
        }
      }
    }
  }
  std::sort( m_xHits.begin(),  m_xHits.end(),  Tf::increasingByProjection<PatTTHit>() );
  std::sort( m_uvHits.begin(), m_uvHits.end(), Tf::increasingByProjection<PatTTHit>() );
}


//=========================================================================
//  Fit and remove the worst hit, as long as over tolerance
//=========================================================================
void PatDownstream::fitAndRemove ( PatDownTrack& track ) {
  
  if ( 2 > track.hits().size() ) return;  // no fit if single point !
  
  bool again;
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
    

    for(PatTTHit* hit: track.hits()) {
      double yTrack = track.yAtZ( 0. );
      double tyTr   = track.slopeY();
      updateTTHitForTrack( hit, yTrack, tyTr );
      double dz   = hit->z() - track.zMagnet();
      double dist = track.distance( hit );
      double w    = hit->hit()->weight();
      double t    = hit->hit()->sinT();          
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
      if ( 0 == differentPlanes[hit->planeCode()]++ ) ++nDoF;
      if( isStereo( hit ) ) ++nbUV;

      if ( m_printing ) {
        info() << format( "   Plane %2d x %7.2f dist %6.3f ", 
                          hit->planeCode(), hit->x(), dist );
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endmsg;
      }
      
    }

    if (0 != nbUV) {
      CholeskyDecomp<double, 3> decomp(mat);
      if (!decomp) {
        track.setChi2(1e42);
        return;
      } else {
        decomp.Solve(rhs);
      }
    } else {
      CholeskyDecomp<double, 2> decomp(mat);
      if (!decomp) {
        track.setChi2(1e42);
        return;
      } else {
        decomp.Solve(rhs);
      }
      rhs[2] = 0.;
    }
    
    const double dx = rhs[0], dsl = rhs[1], dy = rhs[2];
    
    if ( m_printing ) {
      info() << format( "  dx %7.3f dsl %7.6f dy %7.3f, displY %7.2f", 
                        dx, dsl, dy, track.displY() ) << endmsg;
    }
    
    if ( 4 > nbUV ) track.updateX( dx, dsl );
    track.setDisplY( track.displY() + dy );

    //== Remove worst hit and retry, if too far.

    double chi2 = track.initialChi2();
    
    double maxDist = -1.;
    PatTTHits::iterator worst;
    for (PatTTHits::iterator itH = track.hits().begin();
         itH != track.hits().end(); itH++){
      
      PatTTHit* hit = *itH;
      double yTrack = track.yAtZ( 0. );
      double tyTr   = track.slopeY();
      updateTTHitForTrack( hit, yTrack, tyTr );
      double dist = track.distance( hit );
      
      // --
      if ( m_printing ) {
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
    
    
    if ( 5. * m_maxDistance > maxDist && 0 < nbUV ) {
      // remove if Y is incompatible
      for (PatTTHits::iterator itH = track.hits().begin();
           itH != track.hits().end(); itH++){
        PatTTHit* hit = *itH;
        double yTrack = track.yAtZ( hit->z() );
        if ( yTrack > hit->hit()->yMin() && yTrack < hit->hit()->yMax() ) continue;

        if ( m_printing ) {
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
    }
    

    if ( m_maxDistance < maxDist && track.hits().size() > 3) {
      
      track.hits().erase( worst );
      again = true;
      if ( m_printing ) info() << "   remove worst and retry" << endmsg;
      
    } 
    

    if ( m_printing ) {
      info() << format( "  ---> chi2 %7.2f maxDist %7.3f tol %7.3f", chi2, maxDist, m_maxDistance ) << endmsg;
    }    
  } while (again);
}


//=========================================================================
//  Collect the hits in the other x layer
//=========================================================================
void PatDownstream::findMatchingHits( PatTTHits& MatchingXHits, PatDownTrack& track, int plane ) {
  MatchingXHits.clear();
  //search window = const1/momentum + const2
  const double tol = (std::abs(track.state()->p() / m_tolMomentum) < 1. / (m_maxWindow - m_tolX)) ?
    m_maxWindow : (m_tolX + m_tolMomentum / track.state()->p());
  for(PatTTHit* hit: m_xHits) {
    if ( plane != hit->planeCode() ) {
      const double adist = std::abs(track.distance( hit ));
      if ( adist < tol ) MatchingXHits.push_back( hit );
    }
  }
  std::sort( MatchingXHits.begin(), MatchingXHits.end(), Tf::increasingByZ<PatTTHit>() );
}


//=========================================================================
//  Add the UV hits.
//=========================================================================
void PatDownstream::addUVHits ( PatDownTrack& track ) {
  //search window = const1/momentum + const2
  const double tol = (std::abs(track.state()->p() / m_tolMomentum) < 1. / (m_maxWindow - m_tolUV)) ?
    m_maxWindow : (m_tolUV + m_tolMomentum / track.state()->p());
  for(PatTTHit* hit: m_uvHits) {
    const double yTrack = track.yAtZ( 0. );
    const double tyTr   = track.slopeY();
    updateTTHitForTrack( hit, yTrack, tyTr );
    const double adist = std::abs( track.distance( hit ) );
    if ( adist < tol ) track.hits().push_back( hit );
  }
  track.sortFinalHits();
}


//=========================================================================
//  Check if the new candidate is better than the old one
//=========================================================================
bool PatDownstream::acceptCandidate( PatDownTrack& track, PatDownTrack&  bestTrack, 
                                     int& maxPoints, double& minChi2 ){

  const int nbMeasureOK = track.hits().size();

  //== Enough mesures to have Chi2/ndof.
  if ( 3 > nbMeasureOK ) {
    if ( m_printing ) info() << " === not enough points" << endmsg;
    return false;
  }

  //== Good enough Chi2/ndof
  if ( m_maxChi2 < track.chi2() ) {
    if ( m_printing ) info() << " === Chi2 too big " << track.chi2() << endmsg;
    return false;
  }

  const double deltaP = track.momentum() * track.state()->qOverP() - 1.;
  //== Compatible momentum
  if ( m_deltaP < fabs(deltaP) ) {
    if ( m_printing ) info() << " === Deltap too big " << deltaP << endmsg;
    if ( !m_magnetOff )return false;
    //return false;
  }

  //== Longest -> Keeep it
  if ( maxPoints <= nbMeasureOK ) {
    //== Same : Keep if better Chi2
    if ( minChi2 < track.chi2() ) {
      if ( m_printing ) info() << " === Chi2 bigger than previous " << track.chi2() << endmsg;
      return false;
    }
  } else {
    if ( m_printing ) info() << " === less points than previous" << endmsg;
    return false;
  }

  //== Count if enough with high threshold
  int nbHigh = 0;
  int nbUsed = 0;

  // Counter to check how many planes have fired
  std::array<int,4> differentPlanes = { 0, 0, 0, 0 };
  unsigned int nDoF = 0;
  
  for(PatTTHit* hit: track.hits()) {
    if ( hit->hit()->sthit()->cluster().highThreshold() ) ++nbHigh;
    if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream )  ) ++nbUsed;
     // -- check how many different layers have fired
    if ( 0 == differentPlanes[hit->planeCode()]++ ) ++nDoF;
    
  }

  if( nDoF < 3 ){
    if ( m_printing ) always() << " === not enough planes involved" << endmsg;
    return false;
  }

  if ( 2 > nbHigh ) {
    if ( m_printing ) info() << " === not enough high threshold points" << endmsg;
    return false;
  }
  if ( nbMeasureOK == nbUsed ) {
    if ( m_printing ) info() << " === is a clone" << endmsg;
    return false;
  }
        
  if ( m_printing ) {
    info() << format( "  *** Good candidate ***  slope%8.2f displX%8.2f Y%8.2f Chi2%8.2f", 
                      1000.*track.slopeX(), track.displX(), track.displY(), track.chi2() );
    if ( 0 == bestTrack.hits().size() ) {
      info() << " OK"  << endmsg;
    } else {
      info() << " Better" << endmsg;
    }
  }

  //== Better candidate. 
  minChi2 = track.chi2();
  maxPoints = nbMeasureOK;
  if ( maxPoints > 4 ) maxPoints = 4;
  
  //== calculate pt and p
 
  const double momentum = std::abs(track.momentum());
  const double tx2 = track.slopeX()*track.slopeX();
  const double ty2 = track.slopeY()*track.slopeY();
  const double sinTrack = sqrt( 1. - 1./(1.+tx2 + ty2) );
  const double pt = sinTrack*momentum;
  
  if (momentum<m_minMomentum) return false;
  if (pt<m_minPt) return false;
  
  

  for( PatTTHit* hit: bestTrack.hits() ) {
    hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
  }
 
  track.sortFinalHits();
  
  for( PatTTHit* hit: track.hits() ) {
    hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, true );
  }
  
      
  if ( m_printing ) {
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
  return true;
}

//=========================================================================
//  Store Track
//=========================================================================
void PatDownstream::storeTrack( PatDownTrack& track, LHCb::Tracks* finalTracks, LHCb::Track* tr ){
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
  for ( PatTTHits::iterator itH = track.hits().begin(); 
        track.hits().end() != itH; ++itH ){
    dTr->addToLhcbIDs( (*itH)->hit()->lhcbID() );
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
}



//=============================================================================
// Fit the projection in the zx plane, one hit in each x layer
//=============================================================================
void PatDownstream::fitXProjection( PatTTHits& xHits, PatDownTrack& track, PatTTHit* firstHit ){
  // Catch if there is no second hit in other station
  track.hits().push_back( firstHit );
  PatDownTrack bestTrack( track );
  PatDownTrack newTrack( track );
  double minChi2 = 1000000;
  for( PatTTHit* hit: xHits) {
    newTrack.startNewXCandidate( firstHit );
    newTrack.hits().push_back( hit );
    fitAndRemove( newTrack );
    if ( newTrack.chi2() < minChi2 ) {
      minChi2 = newTrack.chi2();
      bestTrack = newTrack;
    }
  }
  track = bestTrack;
}

//=============================================================================
// This is needed for tracks which have more than one x hit in one layer
//=============================================================================
void PatDownstream::addOverlapRegions( PatDownTrack& track ){
  bool hitAdded = false;

  for( PatTTHit* hit: m_xHits ) {
    if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream )) continue;
    if ( m_maxDistance > std::abs( track.distance( hit ) ) ) {
      double yTrack = track.yAtZ( hit->z() );
      if ( yTrack < hit->hit()->yMin() || yTrack > hit->hit()->yMax() ) continue;

      track.hits().push_back( hit );
      hitAdded = true;
    }
  }
  if ( hitAdded ) {
      track.sortFinalHits();
      fitAndRemove( track );
  }
}


