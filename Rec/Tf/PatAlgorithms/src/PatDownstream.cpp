// $Id: PatDownstream.cpp,v 1.5 2008-07-13 22:05:47 mschille Exp $
// Include files 

// from boost
#include <boost/assign/list_of.hpp>
#include <boost/array.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"

#include "STDet/DeTTSector.h"

#include "TfKernel/RecoFuncs.h"
#include "TfKernel/ITTHitCreator.h"
// local
#include "PatDownTrack.h"
#include "PatDownstream.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatDownstream
//
// 2005-11-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatDownstream );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatDownstream::PatDownstream( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputLocation" , m_inputLocation  = ""    );
  declareProperty( "OutputLocation", m_outputLocation = LHCb::TrackLocation::Downstream );
  declareProperty( "UseForward"    , m_useForward     = false );
  declareProperty( "PrintTracks"   , m_printTracks    = false );

  declareProperty( "deltaP"        , m_deltaP        = 2.0  );
  declareProperty( "xPredTol"      , m_xPredTol      = 150. * Gaudi::Units::mm * Gaudi::Units::GeV  );
  declareProperty( "TolMatch"      , m_tolMatch      = 2.5 *  Gaudi::Units::mm );
  declareProperty( "TolUV"         , m_tolUV         = 3.0 *  Gaudi::Units::mm );
  declareProperty( "MaxDistance"   , m_maxDistance   = 0.10 * Gaudi::Units::mm );
  declareProperty( "MaxChisq"      , m_maxChisq      = 10.        );
  declareProperty( "SeedFilter"    , m_seedFilter    = false      );
  declareProperty( "errorZMagnet"  , m_errZMag       = 30. *  Gaudi::Units::mm );
  declareProperty( "minTTx"        , m_minTTx        = 35. *  Gaudi::Units::mm );
  declareProperty( "minTTy"        , m_minTTy        = 35. *  Gaudi::Units::mm );
  // Define parameters for DC06 field, zState = 9410
  declareProperty( "zMagnetParams" , m_zMagnetParams =
		  boost::assign::list_of(5372.42)(-3372.25)(370.484) );
  declareProperty( "momentumParams", m_momentumParams=
		  boost::assign::list_of(1226.32)(479.859)(2859.75) );
  declareProperty( "yParams"       , m_yParams       =
		  boost::assign::list_of(5.)(2000.) );

  declareProperty( "zTT"           , m_zTT           = 2485.* Gaudi::Units::mm );
  declareProperty( "zTTa"          , m_zTTa          = 2350.* Gaudi::Units::mm );

  declareProperty( "StateErrorX2"  , m_stateErrorX2  =   4.0);
  declareProperty( "StateErrorY2"  , m_stateErrorY2  = 400.);
  declareProperty( "StateErrorTX2" , m_stateErrorTX2 =  6.e-5);
  declareProperty( "StateErrorTY2" , m_stateErrorTY2 =  1.e-4);
  declareProperty( "StateErrorP"   , m_stateErrorP   =  0.15);

  //== debugging options
  declareProperty( "SeedKey"       , m_seedKey       = -1    );
  declareProperty( "WithDebugTool" , m_withDebugTool = false );
  declareProperty( "DebugTool"     , m_debugToolName = "PatDebugTTTruthTool" );
}
//=============================================================================
// Destructor
//=============================================================================
PatDownstream::~PatDownstream() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatDownstream::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  m_debugTool = 0;
  if ( m_withDebugTool ) {
    m_debugTool = tool<IPatDebugTTTool>( m_debugToolName );
  }  

  info() << "========================================"  << endreq
         << " deltaP             = " << m_deltaP        << endreq
         << " xPredTol           = " << m_xPredTol      << endreq
         << " TolMatch           = " << m_tolMatch      << endreq
         << " TolUV              = " << m_tolUV         << endreq
         << " MaxDistance        = " << m_maxDistance   << endreq
         << " MaxChisq           = " << m_maxChisq      << endreq
         << "========================================"  << endreq;
  
  info() << "zMagnetParams ";
  for ( unsigned int kk = 0; m_zMagnetParams.size() > kk ; kk++) {
    info() << m_zMagnetParams[kk] << " ";
  }
  info() << endreq << "momentumParams ";
  for ( unsigned int kk = 0; m_momentumParams.size() > kk ; kk++) {
    info() << m_momentumParams[kk] << " ";
  }
  info() << endreq ;
  if ( 3 > m_zMagnetParams.size() ) {
    warning() << "Not enough zMagnetParams" << endreq;
    return StatusCode::FAILURE;
  }
  if ( 3 > m_momentumParams.size() ) {
    warning() << "Not enough momentumParams" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatDownstream::execute() {

  debug() << "==> Execute" << endmsg;

  //== if no specified input location, get seed if it exists, else Tsa.
  if ( m_inputLocation.empty() ) {
    if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Seed ) ) {
      m_inputLocation = LHCb::TrackLocation::Seed;
    } else {
      m_inputLocation = LHCb::TrackLocation::Tsa;
    }
  }
  
  ttCoordCleanup();
  //==========================================================================
  // Prepare the tracks, removing the seends already used in Match.
  //==========================================================================
   LHCb::Tracks* inTracks   = get<LHCb::Tracks>( m_inputLocation );

   LHCb::Tracks myInTracks;  //== Local container of good track.
   myInTracks.reserve(inTracks->size());

  //=== Remove the seeds already used in Match, check by ancestor of Match tracks
  
  if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Match ) ) {
    LHCb::Tracks* match = get<LHCb::Tracks>( LHCb::TrackLocation::Match );

    for ( LHCb::Tracks::const_iterator itT = inTracks->begin();
          inTracks->end() != itT; itT++ ) {
      LHCb::Track* tr = (*itT);
      bool store = true;
      debug() << "Seed " << tr->key();
      for ( LHCb::Tracks::const_iterator itTM = match->begin();
            match->end() != itTM; itTM++ ) {
        const SmartRefVector<LHCb::Track>& ancestors = (*itTM)->ancestors();
        for ( SmartRefVector<LHCb::Track>::const_iterator itA = ancestors.begin();
              ancestors.end() != itA; ++itA ) {
          const LHCb::Track* pt = (*itA);
          if ( tr == pt ) {
            debug() << " is used in match " << (*itTM)->key(); 
            store = !m_seedFilter;
            if ( m_seedFilter ) tagUsedTT( *itTM );

          }        
        }
      }
      if ( store ) {
        myInTracks.insert( tr );
        debug() << " will be processed";
      }
      debug() << endreq;
    }
  } else {  //== Copy tracks without ancestor...
    for ( LHCb::Tracks::const_iterator itT = inTracks->begin();
          inTracks->end() != itT; itT++ ) {
      LHCb::Track* tr = (*itT);
      //== Ignore tracks with ancestor = forward...
      if ( m_useForward && 0 < tr->ancestors().size() ) continue;
      myInTracks.insert( tr );
    }
  }

  debug() << "-- Started from " << inTracks->size() << " Seed, left with " 
          << myInTracks.size() << " tracks after Matched removal." << endreq;

  //==========================================================================
  // Get the output container ( created by PatDataStore )
  //==========================================================================
  LHCb::Tracks* finalTracks = new LHCb::Tracks();
  finalTracks->reserve(100);
  put( finalTracks, m_outputLocation);
  //==========================================================================
  // Main loop on tracks
  //==========================================================================
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();
  
  for ( LHCb::Tracks::const_iterator itT = myInTracks.begin();
        myInTracks.end() != itT; ++itT ) {
    LHCb::Track* tr = (*itT);

    m_printing = msgLevel( MSG::DEBUG );
    if ( 0 <= m_seedKey && m_seedKey == tr->key() ) m_printing = true;

    PatDownTrack track( tr, m_zTT, m_zMagnetParams, m_momentumParams, m_yParams, m_errZMag );

    //Y. Xie get rid or particles from beampipe 
    double xAtTT = track.xAtZ( m_zTTa );
    double yAtTT = track.yAtZ( m_zTTa );
    if( m_minTTx > fabs(xAtTT) && 
        m_minTTy > fabs(yAtTT)       ) continue;
    
    double deltaP = track.moment() * track.state()->qOverP() - 1.;

    if ( m_printing ) {
      for ( PatTTHits::const_iterator itH = ttCoords.begin(); ttCoords.end() != itH; ++itH ){
        PatTTHit* hit = (*itH);
        double yTrack = track.yAtZ( 0. );
        double tyTr   = track.slopeY();
        updateTTHitForTrack( hit, yTrack, tyTr );
      }
      
      info() << "Track " << tr->key() 
             << format( " [%7.2f %7.2f GeV] x,y(TTa)%7.1f%7.1f dp/p%7.3f errXMag%6.1f YTT%6.1f",
                        .001/track.state()->qOverP(), .001*track.moment(),
                        track.xAtZ( m_zTTa ), track.yAtZ( m_zTTa ), deltaP,
                        track.errXMag(), track.errYMag() )
             << endreq;
      info() << format( " Y slope %8.6f computed %8.6f", track.state()->ty(), track.slopeY() ) 
             << endreq;

      if ( m_debugTool ) m_debugTool->debugTTClusterOnTrack( tr, ttCoords.begin(), ttCoords.end() );      
    }
    
    
    if ( m_deltaP < fabs(deltaP) ) {
      if ( m_printing ) info() << "   --- deltaP " << deltaP << " -- rejected" << endreq;
      continue;
    }

    // Max Pt around 100 MeV for strange particle decay -> maximum displacement is in 1/p.
    double xPredTol = m_xPredTol / fabs( track.moment() ) + 10.;  // P dependance + overal tol.
    double yTol = xPredTol;

    m_xHits.clear();
    m_uvHits.clear();
    
    //== Collect all hits compatible with the extrapolation, region by region.
    if ( m_printing ) info() << "-- collect hits with tolerance " << xPredTol << endreq;
    
    for ( int kSta = 0; m_ttHitManager->maxStations() > kSta; ++kSta ) {
      for ( int kLay = 0; m_ttHitManager->maxLayers() > kLay; ++kLay ) {
        for ( int kReg = 0; m_ttHitManager->maxRegions() > kReg; ++kReg ) {
          const Tf::ITTHitCreator::STRegion* reg = m_ttHitManager->region( kSta, kLay, kReg );
          double yReg = track.yAtZ( reg->z() );
          if ( !reg->isYCompatible( yReg, yTol ) ) continue;
          double xReg = track.xAtZ( reg->z() );
          if ( !reg->isXCompatible( xReg, xPredTol ) ) continue; 
          Tf::TTStationHitManager<PatTTHit>::HitRange coords = m_ttHitManager->hits( kSta, kLay, kReg );
          for ( PatTTHits::const_iterator itH = coords.begin(); coords.end() != itH; ++itH ){
            
            PatTTHit* hit = (*itH);
            if ( hit->hit()->testStatus( Tf::HitBase::UsedByPatMatch ) ) continue;
            double yPos   = track.yAtZ( hit->z() );
            if ( !hit->hit()->isYCompatible( yPos, yTol ) ) continue;
            
            double yTrack = track.yAtZ( 0. );
            double tyTr   = track.slopeY();
            updateTTHitForTrack( hit, yTrack, tyTr );
            double pos    = track.xAtZ( hit->z() );
            if ( xPredTol < fabs( pos - hit->x() ) ) continue;
            hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
            hit->setProjection( fabs( hit->x()-pos ) );
            if ( hit->hit()->isX() ) {
              m_xHits.push_back( hit );
            } else {
              m_uvHits.push_back( hit );
            }
            if ( m_printing ) {
              info() << format( "  plane%2d z %8.2f x %8.2f pos %8.2f High%2d dist %8.2f", 
                                hit->planeCode(), hit->z(), hit->x(), pos, 
                                hit->hit()->sthit()->cluster().highThreshold(), hit->x() - pos);
              if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
              info() << endreq;
            }
          }
        }
      }
    }
  

    std::sort( m_xHits.begin(),  m_xHits.end(),  Tf::increasingByProjection<PatTTHit>() );
    std::sort( m_uvHits.begin(), m_uvHits.end(), Tf::increasingByProjection<PatTTHit>());

    PatTTHits::const_iterator itH, itH1;

    int nbOK = 0;
    PatTTHits bestHits;
    PatDownTrack bestTrack( track );
    
    int maxPoints = 0;
    double minChisq = m_maxChisq + 10.;
    
    if ( 2 < ( m_xHits.size() +  m_uvHits.size() ) ) {
      //==============================================================
      // Try to find a candidate: X first, then UV.
      //==============================================================
      
      for ( itH = m_xHits.begin(); m_xHits.end() != itH; itH++ ) {
        const PatTTHit* myHit = *itH;
        if ( myHit->hit()->testStatus( Tf::HitBase::UsedByPatDownstream ) ) continue;
        double meanZ = myHit->z();
        double posX  = myHit->x( );
        int myPlane  = myHit->planeCode();

        track.startNewCandidate();
        
        double slopeX = (track.xMagnet() - posX + track.sagitta( meanZ)) / (track.zMagnet() - meanZ);
        track.setSlopeX( slopeX );

        double tolMatch = m_tolMatch + 5. * fabs( track.slopeY() );

        if ( m_printing ) {
          info() << endreq 
                 << format( "... start plane %1d x%8.2f z%8.1f slope%8.2f tolMatch%7.3f",
                            myPlane, posX, meanZ, 1000. * slopeX, tolMatch )
                 << endreq;
        }        
        
        findMatchingHits( track, myPlane, tolMatch );

        fitAndRemove( track );

        addUVHits( track ); // Make it momentum dependent ?
        if ( 3 > track.hits().size() ) {
          if ( m_printing ) info() << " === not enough hits" << endreq;
          continue;
        }
        
        fitAndRemove( track );
        int nbMeasureOK = track.hits().size();

        //== Enough mesures to have Chi2/ndof.
        if ( 3 > nbMeasureOK ) {
          if ( m_printing ) info() << " === not enough points" << endreq;
          continue;
        }

        //== Count if enough with high threshold
        int nbHigh = 0;
        int nbUsed = 0;
        for (itH1 = track.hits().begin(); track.hits().end() != itH1; ++itH1 ){
          if ( (*itH1)->hit()->sthit()->cluster().highThreshold() ) ++nbHigh;
          if ( (*itH1)->hit()->testStatus( Tf::HitBase::UsedByPatDownstream )  ) ++nbUsed;
        }
        if ( 2 > nbHigh ) {
          if ( m_printing ) info() << " === not enough high threshold points" << endreq;
          continue;
        }
        if ( nbMeasureOK == nbUsed ) {
          if ( m_printing ) info() << " === is a clone" << endreq;
          continue;
        }

        //== Good enough Chi2/ndof
        if ( m_maxChisq < track.chisq() ) {
          if ( m_printing ) info() << " === Chisq too big " << track.chisq() << endreq;
          continue;
        }
        
        //== Longest -> Keeep it
        if ( maxPoints <= nbMeasureOK ) {
          //== Same : Keep if better Chi2
          if ( minChisq < track.chisq() ) {
            if ( m_printing ) info() << " === Chisq bigger than previous " << track.chisq() << endreq;
            continue;
          }
          if ( track.chisq() < minChisq ) minChisq = track.chisq();
          maxPoints = nbMeasureOK;
          if ( maxPoints > 4 ) maxPoints = 4;
        } else {
          if ( m_printing ) info() << " === less points than previous" << endreq;
          continue;
        }

        deltaP = track.moment() * track.state()->qOverP() - 1.;
        //== Compatible momentum
        if ( m_deltaP < fabs(deltaP) ) {
          if ( m_printing ) info() << " === Deltap too big " << deltaP << endreq;
          continue;
        }

        if ( m_printing ) {
          info() << format( "  *** Good candidate ***  slope%8.2f displX%8.2f Y%8.2f Chi2%8.2f", 
                             1000.*track.slopeX(), track.displX(), track.displY(), track.chisq() );
          if ( 0 == bestHits.size() ) {
            info() << " OK"  << endreq;
          } else {
            info() << " Better" << endreq;
          }
        }

        //== Better candidate. Clear flag on previous hits, and store the new one in bestTrack
        for ( itH1 = bestHits.begin(); bestHits.end() != itH1; ++itH1 ) {
          (*itH1)->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
        }
        bestHits.clear();

        track.sortFinalHits();
        
        for ( itH1= track.hits().begin(); track.hits().end() != itH1; itH1++ ){
          (*itH1)->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, true );
          bestHits.push_back( *itH1 );
        
          if ( m_printing ) {
            LHCb::STChannelID icID = (*itH1)->hit()->lhcbID().stID();
            double xCoord = (*itH1)->x() ;
            info() << "      TT Clus " 
                   << format( "(S%1d,L%1d,R%2d,S%2d,S%3d) x%7.1f  dist%7.3f High %1d", 
                              icID.station(), icID.layer(), icID.detRegion(), 
                              icID.sector(), icID.strip(), xCoord,
                              track.distance( *itH1 ), (*itH1)->hit()->sthit()->cluster().highThreshold() ) ;
            if ( m_debugTool ) m_debugTool->debugTTCluster( info(), *itH1 );
            info() << endreq;
          }
        }
        bestTrack = track;
        nbOK++;
      }
    }

    //== Debug the track.
    if ( 0 == nbOK ) {
      if ( m_printing ) {
        info() << format( "Track %3d P=%7.2f GeV --- discarded, not enough planes",
                             tr->key(),  .001*track.moment() ) << endreq;
      }
    } else {
      //=== Store the tracks
      track = bestTrack;
      LHCb::Track* dTr = new LHCb::Track();
      dTr->addToAncestors( tr );                // set the seed as ancestor
      dTr->setLhcbIDs(  tr->lhcbIDs()      );   // copy those from the Seed
      LHCb::State tState = *track.state();
      tState.setQOverP( 1./track.moment() ); 
      dTr->addToStates( tState     );   // copy the state of the seed
      //== Adjust flags
      dTr->setType(         LHCb::Track::Downstream  );
      dTr->setHistory(      LHCb::Track::PatDownstream   );
      dTr->setPatRecStatus( LHCb::Track::PatRecIDs   );
      //== add new LHCbIDs
      for ( PatTTHits::iterator itH = track.hits().begin(); 
            track.hits().end() != itH; ++itH ){
        dTr->addToLhcbIDs( (*itH)->hit()->lhcbID() );
      }
      // create a state at zTTa
      LHCb::State ttState;
      ttState.setState( track.xAtZ( m_zTTa ),
                        track.yAtZ( m_zTTa ),
                        m_zTTa,
                        track.slopeX(),
                        track.slopeY(),
                        1./track.moment() );
      Gaudi::TrackSymMatrix cov;
      cov(0,0) = m_stateErrorX2;
      cov(1,1) = m_stateErrorY2;
      cov(2,2) = m_stateErrorTX2;
      cov(3,3) = m_stateErrorTY2;
      double errQOverP = m_stateErrorP / track.moment();
      cov(4,4) = errQOverP * errQOverP;
      
      ttState.setCovariance( cov );
      dTr->addToStates( ttState );
      
      finalTracks->insert( dTr);
    }
  }
  debug() << "Found " << finalTracks->size() << " tracks." << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatDownstream::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Cleaup already used TT coord
//=========================================================================
void PatDownstream::ttCoordCleanup ( ) {
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();

  PatTTHits::const_iterator itH;
  for ( itH = ttCoords.begin(); ttCoords.end() != itH; ++itH ){
    PatTTHit* hit = (*itH);
    hit->hit()->setStatus( Tf::HitBase::UsedByPatDownstream, false );
  }
  //== Tag hit used in forward

  if ( m_useForward ) {
    if ( exist<LHCb::Tracks>( LHCb::TrackLocation::Forward ) ) {
      LHCb::Tracks* tracks = get<LHCb::Tracks>( LHCb::TrackLocation::Forward );
      for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) {
        tagUsedTT( *itT );
      }
    }
  }
}

//=========================================================================
//  Tag used TT hits on this track.
//=========================================================================
void PatDownstream::tagUsedTT( const LHCb::Track* tr ) {
  Tf::TTStationHitManager<PatTTHit>::HitRange ttCoords = m_ttHitManager->hits();
  for ( std::vector<LHCb::LHCbID>::const_iterator itId = tr->lhcbIDs().begin();
        tr->lhcbIDs().end() != itId; ++itId ) {
    if ( !(*itId).isTT() ) continue;
    for ( PatTTHits::const_iterator itH = ttCoords.begin(); ttCoords.end() != itH; ++itH ){
      PatTTHit* hit = (*itH);
      if ( hit->hit()->lhcbID() == (*itId ) ) {
        hit->hit()->setStatus( Tf::HitBase::UsedByPatMatch, true );
      }
    }
  }
}
//=========================================================================
//  Fit and remove the worst hit, as long as over tolerance
//=========================================================================
void PatDownstream::fitAndRemove ( PatDownTrack& track ) {

  PatTTHits::iterator itH;

  if ( 2 > track.hits().size() ) return;  // no fit if single point !
  
  bool again;
  do {
    again = false;
    //== Fit, using the magnet point as constraint.
    double s0  = 1./( track.errXMag() * track.errXMag() );
    double sz  = 0.;
    double st  = 0.;
    double sz2 = 0.;
    double szt = 0.;
    double st2 = 0.;
    double sx  = s0 * track.dxMagnet();//( m_magnetSave.x() - m_magnet.x() );
    double sxz = 0.;
    double sxt = 0.;
    int nbUV = 0;

    for ( itH = track.hits().begin(); track.hits().end() != itH; itH++ ){  
      PatTTHit* hit = (*itH);
      double yTrack = track.yAtZ( 0. );
      double tyTr   = track.slopeY();
      updateTTHitForTrack( hit, yTrack, tyTr );
      double dz   = hit->z() - track.zMagnet();
      double dist = track.distance( hit );
      double w    = hit->hit()->weight();
      double t    = hit->hit()->sinT();          
      s0   += w;
      sz   += w * dz;
      st   += w * t;
      sz2  += w * dz * dz;
      szt  += w * dz * t ;
      st2  += w * t  * t ;
      sx   += w * dist;
      sxz  += w * dist * dz;
      sxt  += w * dist * t ;
      if ( ! hit->hit()->isX() ) nbUV++;
      if ( m_printing ) {
        info() << format( "   Plane %2d x %7.2f dist %6.3f ", 
                          hit->planeCode(), hit->x(), dist );
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endreq;
      }
    }
    double a1 = sz  * sz - s0  * sz2;
    double b1 = st  * sz - s0  * szt;
    double c1 = sx  * sz - s0  * sxz;
    double a2 = sz  * st - s0  * szt;
    double b2 = st  * st - s0  * st2;
    double c2 = sx  * st - s0  * sxt;
    
    double dx, dy, dsl;

    double den = a1 * b2 - a2 * b1;    
    if ( 0 == nbUV ) {
      den = s0 * sz2 - sz * sz;
      dsl = ( s0 * sxz - sx  * sz ) / den;
      dx  = ( sx * sz2 - sxz * sz ) / den;
      dy  = 0.;
    } else {
      dsl = ( c1 * b2 - c2 * b1 ) / den;
      dy  = ( a1 * c2 - a2 * c1 ) / den;
      dx  = (sx - dsl * sz - dy * st ) / s0;
    }

    if ( m_printing ) {
      info() << format( "  dx %7.3f dsl %7.6f dy %7.3f, displY %7.2f", 
                        dx, dsl, dy, track.displY() ) << endreq;
    }
    
    if ( 4 > nbUV ) track.updateX( dx, dsl );
    track.setDisplY( track.displY() + dy );

    //== Remove worst hit and retry, if too far.

    double chisq = track.initialChisq();

    double maxDist = -1.;
    PatTTHits::iterator worst;
    for ( itH = track.hits().begin(); track.hits().end() != itH; itH++ ){
      PatTTHit* hit = (*itH);
      double yTrack = track.yAtZ( 0. );
      double tyTr   = track.slopeY();
      updateTTHitForTrack( hit, yTrack, tyTr );
      double dist = track.distance( hit );
      if ( m_printing ) {
        info() << format( "   Plane %2d x %7.2f dist %6.3f ", 
                          hit->planeCode(), hit->x(), dist );
        if ( m_debugTool ) m_debugTool->debugTTCluster( info(), hit );
        info() << endreq;
      }
      chisq += dist * dist * hit->hit()->weight();
      if ( maxDist < fabs(dist) ) {
        maxDist = fabs( dist );
        worst = itH;
      }
    }
    if ( 2 < track.hits().size() ) chisq /= (track.hits().size() - 2 );
    track.setChisq( chisq );

    if ( 5. * m_maxDistance > maxDist && 0 < nbUV ) {
      // remove if Y is incompatible
      for ( itH = track.hits().begin(); track.hits().end() != itH; itH++ ){
        PatTTHit* hit = (*itH);
        double yTrack = track.yAtZ( hit->z() );
        if ( yTrack > hit->hit()->yMin() && yTrack < hit->hit()->yMax() ) continue;
        if ( m_printing ) {
          info() << "   remove Y incompatible hit measure = " << hit->x() 
              << " : y " << yTrack << " min " << hit->hit()->yMin()
              << " max " << hit->hit()->yMax() << endreq;
        }
        track.hits().erase( itH );
        maxDist = 0.;  // avoid deleting worst, may be already gone...
        if ( 2 < track.hits().size() ) again = true;
        break;
      }
      if ( again ) continue;
    }

    if ( m_maxDistance < maxDist ) {
      track.hits().erase( worst );
      if ( 2 < track.hits().size() ) {
        again = true;
        if ( m_printing ) info() << "   remove worst and retry" << endreq;
        continue;
      }
    } 
    
    if ( m_printing ) {
      info() << format( "  ---> chi2 %7.2f maxDist %7.3f tol %7.3f", chisq, maxDist, m_maxDistance ) << endreq;
    }    
  } while (again);
}


//=========================================================================
//  Collect the hits, and update the track parameters
//=========================================================================
void PatDownstream::findMatchingHits ( PatDownTrack& track, int plane, double tol ) {
  track.hits().clear();
  PatTTHits::iterator itH;

  //== separate tolerance for initial point and extrapolation...

  for ( itH = m_xHits.begin(); m_xHits.end() != itH; itH++ ){
    if ( plane == (*itH)->planeCode() ) {
      if ( m_maxDistance > fabs( track.distance( *itH ) ) ) {
        track.hits().push_back( *itH );
      }
    } else {
      if ( tol > fabs( track.distance( *itH ) ) ) {
        track.hits().push_back( *itH );
      }
    }
  }
  track.sortFinalHits();
}
//=========================================================================
//  Add the UV hits.
//=========================================================================
void PatDownstream::addUVHits ( PatDownTrack& track ) {
  PatTTHits::iterator itH;
  for ( itH = m_uvHits.begin(); m_uvHits.end() != itH; itH++ ){
    double yTrack = track.yAtZ( 0. );
    double tyTr   = track.slopeY();
    updateTTHitForTrack( *itH, yTrack, tyTr );
    if ( m_tolUV > fabs( track.distance( *itH ) ) ) {
      track.hits().push_back( *itH );
    }
  }
  track.sortFinalHits();
}
//=============================================================================

