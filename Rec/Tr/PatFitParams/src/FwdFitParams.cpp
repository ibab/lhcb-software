// Include files
#include <string>
#include <stdio.h>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// from LHCbKernel
#include "Linker/LinkedTo.h"

// from LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHit.h"

// from TrackEvent
#include "Event/Track.h"

// local
#include "FwdFitParams.h"
#include "FitTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FwdFitParams
//
// 13/10/2001 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( FwdFitParams )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
FwdFitParams::FwdFitParams( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
  , m_fitTool(nullptr)  
  , m_minMomentum         (   2.0 * Gaudi::Units::GeV )
  , m_maxZVertex          (  500. * Gaudi::Units::mm )
  , m_zVelo               ( 1190. * Gaudi::Units::mm )
  , m_zMag                ( 5500. * Gaudi::Units::mm )
  , m_zRef                ( 8620. * Gaudi::Units::mm )
  , m_zRefExt             ( 9520. * Gaudi::Units::mm )
  , m_zRefM               ( 12100. * Gaudi::Units::mm )
  , m_tupleName           ( "Track" )
  , m_tupleName2          ( "Point" )
  , m_zBeforeST2          ( 8200. * Gaudi::Units::mm )
  , m_zBeforeST3          ( 8800. * Gaudi::Units::mm )
  , m_count()
{
  declareProperty( "NtupleName"           , m_tupleName     );
  declareProperty( "NtupleName2"          , m_tupleName2    );

  declareProperty( "MinMomentum"          , m_minMomentum   );
  declareProperty( "MaxZVertex"           , m_maxZVertex    );
  declareProperty( "ZVelo"                , m_zVelo         );
  declareProperty( "ZMagnet"              , m_zMag          );
  declareProperty( "ZReference"           , m_zRef          );
  declareProperty( "ZRefM"                , m_zRefM         );
  declareProperty( "ZRefExt"              , m_zRefExt       );

  declareProperty( "zMagnetParams"        , m_zMagParams    );
  declareProperty( "xParams"              , m_xParams      );
  declareProperty( "yParams"              , m_yParams      );
  declareProperty( "y1Params"             , m_y1Params      );

  declareProperty( "MomentumParams"       , m_momParams );
  declareProperty( "XsParams"             , m_XsParams      );

  declareProperty( "ZbeforeST2"           , m_zBeforeST2    );
  declareProperty( "ZbeforeST3"           , m_zBeforeST3    );

  declareProperty( "VeloFromMC"           , m_veloFromMC = false );

  declareProperty( "VeloTracksLocation", 
                   m_veloTracksLocation = LHCb::TrackLocation::Velo);
}

//=============================================================================
// Destructor
//=============================================================================
FwdFitParams::~FwdFitParams() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FwdFitParams::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_fitTool = tool<FitTool>( "FitTool" );

  unsigned int i;

  for ( i=0 ; 3 > i ; i++ ) {
    m_count[i] = 0;
  }

  m_ZmPar.init ( "zMagnet" , m_zMagParams );
  m_xPar.init  ( "x"        , m_xParams );
  m_yPar.init  ( "y"        , m_yParams );
  m_y1Par.init ( "y1"       , m_y1Params );

  m_momPar.init( "Momentum", m_momParams );

  m_XsPar.init( "Xs"      , m_XsParams );


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FwdFitParams::execute() {

  LHCb::MCParticles* partCtnr = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  // Get the Velo hits
  LHCb::MCHits* vHits = get<LHCb::MCHits>( LHCb::MCHitLocation::Velo );

  // Get the IT hits
  LHCb::MCHits* itHits = get<LHCb::MCHits>( LHCb::MCHitLocation::IT );

  // Get the OT hits
  LHCb::MCHits* otHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );

  LinkedTo<LHCb::MCParticle, LHCb::Track> myLink ( evtSvc(), msgSvc(),m_veloTracksLocation);
  // Get the Velo tracks
  LHCb::Tracks* veloTracks = get<LHCb::Tracks>( m_veloTracksLocation );


  LHCb::MCParticles::const_iterator pItr;
  SmartRefVector< LHCb::MCVertex > vDecay;
  const LHCb::MCVertex* vOrigin;

  SmartRefVector< LHCb::MCParticle > daughter;

  Gaudi::XYZPoint origin;
  Gaudi::XYZVector slope;
  double     momentum;

  double dx, dy, dz;
  double xSeed, ySeed, zSeed;
  LHCb::Tracks::iterator vTrack;
  LHCb::MCParticle* myMCPart;
  xSeed = 0.;

  double zMagnet;
  double xMagnet;

  for ( pItr = partCtnr->begin(); partCtnr->end() != pItr; pItr++ ) {
    myMCPart = *pItr;
    int trNum = myMCPart->index();

    // Select particles to work with:
    // == Origin vertex position
    vOrigin = myMCPart->originVertex();
    if ( 0 == vOrigin ) continue;
    if ( m_maxZVertex < vOrigin->position().z() ) continue;
    // == Decay vertices
    vDecay = myMCPart->endVertices();
    double zDecay;

    if ( 0 < vDecay.size()  ) {
      zDecay = vDecay[0]->position().z();
    } else {
      zDecay = 20. * Gaudi::Units::m;
    }
    if ( 10000. > zDecay ) continue;

    // == Momentum cut
    momentum = myMCPart->momentum().R();
    if ( m_minMomentum > momentum ) continue;
    if ( 11 == abs( myMCPart->particleID().pid() ) ) { continue; } // no electrons

    if ( 0 > myMCPart->particleID().threeCharge() ) {
      momentum = -1 * momentum;
    }

    bool hasInteractionVertex = false;

    SmartRefVector<LHCb::MCVertex> endV = myMCPart->endVertices();
    for ( SmartRefVector<LHCb::MCVertex>::const_iterator itV = endV.begin() ;
          endV.end() != itV; itV++ ) {
      if ( (*itV)->position().z() > 10000. ) continue;
      SmartRefVector<LHCb::MCParticle> parts = (*itV)->products();
      for ( SmartRefVector<LHCb::MCParticle>::const_iterator itP = parts.begin() ;
            parts.end() != itP; itP++ ){
        hasInteractionVertex = true;
      }
    }
    if ( hasInteractionVertex ) continue;

    bool veloFound = false;
    if ( m_veloFromMC ) {
      std::vector<Gaudi::XYZPoint> vPoints;
      
      for ( LHCb::MCHits::const_iterator vHitIt = vHits->begin() ;
            vHits->end() != vHitIt ; vHitIt++ ) {
        if ( (*vHitIt)->mcParticle() ==  myMCPart ) {
          vPoints.push_back( (*vHitIt)->midPoint() );
        }
      }
      if ( vPoints.size() >= 6 ) {
        veloFound = true;
	double a, b, ay, by;
	m_fitTool->fitLine(vPoints, 0, m_zVelo, a, b);
	m_fitTool->fitLine(vPoints, 1, m_zVelo, ay, by);
        origin.SetX( a );
        origin.SetY( ay );
        origin.SetZ( m_zVelo );
        slope.SetX( b );
        slope.SetY( by );
        slope.SetZ( 1. );
      }   
    } else {      
      for ( vTrack = veloTracks->begin(); veloTracks->end() != vTrack; vTrack++ ) {
        LHCb::Track* vTr = *vTrack ;
        bool ok = false;
        LHCb::MCParticle* aPart = myLink.first( vTr->key() );
        while ( 0 != aPart ) {
          if ( myMCPart == aPart ) ok = true;
          aPart = myLink.next();
        }
        if ( !ok ) { continue; }
        
        trNum    = vTr->index();
        origin.SetCoordinates( vTr->firstState().x(),  vTr->firstState().y(),  vTr->firstState().z() ) ;
        slope.SetCoordinates(vTr->firstState().tx(), vTr->firstState().ty(), 1. );
        origin += slope * ( m_zVelo - vTr->firstState().z() );
        veloFound = true;
      }
    }
    
    if ( !veloFound ) { continue; }

    debug() << format( "%4d Ori%7.2f%7.2f%8.2f Slope%8.4f%8.4f Id%7d P%10.2f ",
                      trNum,
                      origin.X() / Gaudi::Units::cm ,
                      origin.Y() / Gaudi::Units::cm ,
                      origin.Z() / Gaudi::Units::cm ,
                      slope.X() ,
                      slope.Y() ,
                      (int)myMCPart->particleID().pid() ,
                      momentum / Gaudi::Units::GeV        )
           << endmsg;
    
    // A container for used hits
    std::vector< Gaudi::XYZPoint > trHits;
    trHits.reserve(30);

    // Get the OT hits

    std::vector<int> hitCount(3,0);

    for ( LHCb::MCHits::const_iterator oHitIt = otHits->begin() ;
          otHits->end() != oHitIt ; oHitIt++ ) {
      if ( (*oHitIt)->mcParticle() ==  myMCPart ) {
        trHits.push_back((*oHitIt)->midPoint());
        if ( msgLevel( MSG::VERBOSE ) ) {
          verbose() << format( " Add OT x%9.3f y%9.3f z%10.3f", 
                               (*oHitIt)->midPoint().x(),
                               (*oHitIt)->midPoint().y(),
                               (*oHitIt)->midPoint().z() ) << endmsg;
        }
        dz = (*oHitIt)->midPoint().Z();
        if ( m_zMag > dz ) {
        } else if ( m_zBeforeST2 > dz ) {
          hitCount[0]++;
        } else if ( m_zBeforeST3 > dz ) {
          hitCount[1]++;
        } else {
          hitCount[2]++;
        }
      }
    }

    // Get the IT hits

    for ( LHCb::MCHits::const_iterator iHitIt = itHits->begin() ;
          itHits->end() != iHitIt ; iHitIt++ ) {
      if ( (*iHitIt)->mcParticle() ==  myMCPart ) {
        trHits.push_back(  (*iHitIt)-> midPoint());
        if ( msgLevel( MSG::VERBOSE ) ) {
          verbose() << format( " Add IT x%9.3f y%9.3f z%10.3f", 
                               (*iHitIt)->midPoint().x(),
                               (*iHitIt)->midPoint().y(),
                               (*iHitIt)->midPoint().z() ) << endmsg;
        }
        dz = (*iHitIt)->midPoint().Z();
        if ( m_zMag > dz ) {
        } else if ( m_zBeforeST2 > dz ) {
          hitCount[0]++;
        } else if ( m_zBeforeST3 > dz ) {
          hitCount[1]++;
        } else {
          hitCount[2]++;
        }
      }
    }

    if ( ( 4 > hitCount[0] ) ||
         ( 4 > hitCount[1] ) ||
         ( 4 > hitCount[2] )    ) continue;

    m_count[0] += 1;

    std::vector<Gaudi::XYZPoint>::iterator pt;
    double khi2 = 1000.;
    double AX = 0.;
    double BX = 0.;
    double CX = 0.;
    double DX = 0.;
    double AY = 0.;
    double BY = 0.;
    double worst;
    std::vector<Gaudi::XYZPoint>::iterator badGuy;

    while ( 100. < khi2 ) {
      //== Enough points on the 3 stations ?
      hitCount[0] = 0;
      hitCount[1] = 0;
      hitCount[2] = 0;
      for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
        dz = (*pt).z();
        if ( m_zMag > dz ) {
        } else if ( m_zBeforeST2 > dz ) {
          hitCount[0]++;
        } else if ( m_zBeforeST3 > dz ) {
          hitCount[1]++;
        } else {
          hitCount[2]++;
        }
      }
      if ( ( 4 > hitCount[0] ) ||
           ( 4 > hitCount[1] ) ||
           ( 4 > hitCount[2] )    ) break;

      // Now we have all the measures on the track. Compute the track parameters
      // We need a fix Z plane for reference, m_zRef, and the z of the magnet
      // center m_zMag.
      // A track is defined as x = AX + BX*dz + CX*dz*dz + DX*dz*dz*dz
      //                       y = AY + BY*dz
      // AX, BX, CX, DX, AY and BY will be later fitted as function of the
      //                               track measured parameters.
      // Compute the parameters: First compute the various sums

      if (!m_fitTool->fitCubic(trHits, 0, m_zRef, AX, BX, CX, DX)) {
        err() << " X matrix is singular " << endmsg;
        continue;
      }

      // For Y also

      if (!m_fitTool->fitLine(trHits, 1, m_zRef, AY, BY)) {
        err() << " X matrix is singular " << endmsg;
        continue;
      }

      // now check the distance / quality

      khi2 = 0;
      worst = 0;
      double dist;
      int nbMeas = 0;

      for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
        if ( m_zMag < (*pt).Z() ) {
          nbMeas++;
          dz = (*pt).Z() - m_zRef;
          dx = (*pt).X() - (AX +dz*(BX + dz*(CX + dz*DX)));
          dy = (*pt).Y() - (AY +dz* BY );

          // Fit error. Arbitrary, as this is MC hit position.

          dist = (dx*dx / .01) + (dy*dy / .01);
          khi2 += dist;
          if ( worst < dist ) {
            worst  = dist;
            badGuy = pt;
          }
          verbose() << format( "At z = %7.1f  dx%8.3f dy%8.3f",
                               (*pt).z(), dx, dy)
                    << endmsg;
        }
      }
      khi2 = khi2 / (nbMeas-7);
      verbose() << " Track Khi2 = " << khi2 << endmsg;
      if ( 100. < khi2 ) {
        debug() << " Remove worst hit and try again " << endmsg;
        (*badGuy).SetZ(-1.);
      }
    }
    //== Bad track !
    if ( 100. < khi2  ) {
      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "-- Bad track : Khi2 = " << khi2
                << " momentum " << momentum/Gaudi::Units::GeV
                << endmsg;
        
        for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
          dz = (*pt).Z() - m_zRef;
          dx = (*pt).X() - (AX +dz*(BX+dz*(CX+dz*DX)));
          dy = (*pt).Y() - (AY +dz* BY );
          debug() << format( " Z %8.1f  X %8.1f  dx %8.3f  Y %8.1f dy %8.3f",
                             (*pt).Z(),
                             (*pt).X(), dx,
                             (*pt).Y(), dy )
                  << endmsg;
        }
      }
      continue;
    }
    
    // Store now the good track parameters.
    // First  compute the extrapolation parameters

    double slopeX2 = slope.x() * slope.x();
    double slopeY2 = slope.y() * slope.y();
    double tbx = 0.;
    double dSlope = 0.;
    double dSl2 = 0.;

    for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
      if ( m_zMag > (*pt).z() ) continue;
      xSeed = (*pt).x();
      ySeed = (*pt).y();
      zSeed = (*pt).z();
      
      m_ZmPar.setFun( 0, 1. );
      m_ZmPar.setFun( 1, 0. );
      m_ZmPar.setFun( 2, slopeX2 );
      m_ZmPar.setFun( 3, xSeed * xSeed );
      m_ZmPar.setFun( 4, slopeY2 );
      zMagnet = m_ZmPar.sum();

      xMagnet   = origin.x() + ( zMagnet - origin.z() ) * slope.x();
      dz      = 1.e-3 * (zSeed-m_zRef);
      
      tbx     = ( xSeed - xMagnet ) / (zSeed - zMagnet );
      dSlope  = tbx - slope.x();
      double dSlope1 = dSlope;
      double tax;
      double xPred;
      double zMagnif;
      int nbLoop = 0;
      
      do {
        dSl2 = dSlope * dSlope;
        m_ZmPar.setFun( 1, dSl2 );
        zMagnet = m_ZmPar.sum();
        xMagnet = origin.x() + ( zMagnet - origin.z() ) * slope.x();
        
        m_xPar.setFun( 0, dz*dz* dSlope  );
        m_xPar.setFun( 1, dz*dz*dz* dSlope );
        tax     = m_xPar.sum();
        
        zMagnif = ( m_zRef - zMagnet)/(zSeed - zMagnet);
        xPred   = xMagnet + (xSeed - tax -xMagnet) * zMagnif;
        
        dSlope1 = dSlope;
        tbx     = ( xPred -xMagnet )/(m_zRef - zMagnet );
        dSlope  = tbx - slope.x();

        if ( msgLevel( MSG::VERBOSE ) ) {
          verbose() << format( "  N%2d tax%10.3f tbx%10.6f dSlope%10.6f",
                               nbLoop, tax, tbx, dSlope ) << endmsg;
        }
        
        nbLoop++;
        if ( 5 < nbLoop ) {
          dSlope = .5 * ( dSlope + dSlope1 );
        }
        
      } while ( (20 > nbLoop ) &&
                ( 1.e-5*fabs(dSlope) < fabs(dSlope1-dSlope) ) );
      
      if ( 20 <= nbLoop ) {
        info() << "dSlope Failed to converge : old "
               << dSlope1 << " new " << dSlope
               << endmsg;
        continue;
      }
      
      //== Compute 'automatically' the regressions
      double zCenter = ( m_zRef -
                  (origin.x()+( m_zRef-origin.z())*slope.x()- AX )/
                  (slope.x()-BX) );
      double taxP = ( xMagnet -AX ) / zMagnif + (xSeed-xMagnet) ;
      
      m_ZmPar.addEvent( zCenter-zMagnet );
      m_xPar.addEvent(  taxP - tax      );
      double yPred = origin.y() + slope.y() * ( zSeed - origin.z() );

      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( " Z %8.1f  X %8.1f  dx %8.3f  Y %8.1f dy %8.3f",
                           zSeed, xSeed, taxP-tax, ySeed, ySeed - yPred ) << endmsg;
      }

      Tuple tPoint = nTuple( m_tupleName2, m_tupleName2 );
      tPoint->column( "Xfit"     , AX );
      tPoint->column( "xSeed"  , xSeed );
      tPoint->column( "ySeed"  , ySeed );
      tPoint->column( "zSeed"  , zSeed );
      tPoint->column( "dz"     , dz );
      tPoint->column( "slopeX" , slope.x() );
      tPoint->column( "slopeY" , slope.y() );
      tPoint->column( "dSlope" , dSlope );
      tPoint->column( "ax"     , taxP );
      tPoint->column( "tax"    , tax );
      tPoint->column( "yPred"  , yPred );
      tPoint->column( "zMagnet", zMagnet );
      tPoint->column( "zCenter", zCenter );
      tPoint->column( "xPred"  , xPred );
      tPoint->write();
    }

    m_count[1] += 1;

    zMagnet = m_ZmPar.sum();

    double dxMagnet = (BX-tbx)*(zMagnet-m_zRef);
    double dSlopeOpt = BX - ( slope.x()+dxMagnet/(zMagnet-m_zVelo) );
    double proj = sqrt( (1.+pow(slope.x(),2)+pow(slope.y(),2)) /
                        (1.+pow(slope.x(),2)));
    
    m_momPar.setFun( 0, 1. );
    m_momPar.setFun( 1, BX * BX );
    m_momPar.setFun( 2, BX * BX * BX * BX );
    m_momPar.setFun( 3, BX * slope.x() );
    m_momPar.setFun( 4, slopeY2 );
    m_momPar.setFun( 5, slopeY2 * slopeY2 );

    double tmom  = m_momPar.sum();

    m_XsPar.setFun( 0, dSlope );
    double xSlopePred = slope.x() + m_XsPar.sum();
    double mcXSlope = myMCPart->momentum().x() / myMCPart->momentum().z();

    m_yPar.setFun( 0, dSl2 * slope.y() );
    
    double yPred = origin.y() + slope.y() * ( m_zRef - origin.z() ) + m_yPar.sum();
    m_yPar.addEvent( AY - yPred );

    m_y1Par.setFun( 0, dSl2 * slope.y() );
    double slopeYPred = slope.y() + m_y1Par.sum();
    m_y1Par.addEvent( BY - slopeYPred );

    //== Try to compute 'automatically' the regressions

    if ( (fabs(momentum) > m_minMomentum ) &&
         (fabs(momentum*dSlopeOpt/Gaudi::Units::GeV-tmom) < 0.1 ) ){
      m_count[2] += 1;

      m_momPar.addEvent( momentum*dSlopeOpt/Gaudi::Units::GeV/proj-tmom );

      if ( msgLevel( MSG::DEBUG ) ) {
        debug() << format( "    momentum %10.3f computed %10.3f",
                           momentum, tmom*Gaudi::Units::GeV*proj/dSlopeOpt )
                << endmsg;
      }

      m_XsPar.addEvent( mcXSlope - xSlopePred );
    }
    // Fill the ntuple

    Tuple tPoint = nTuple( m_tupleName, m_tupleName );
    tPoint->column( "xVelo"  , origin.x() );
    tPoint->column( "yVelo"  , origin.y() );
    tPoint->column( "slopeX" , slope.x());
    tPoint->column( "slopeY" , slope.y());
    tPoint->column( "momentum", momentum / Gaudi::Units::GeV);

    tPoint->column( "ax"     , AX);
    tPoint->column( "bx"     , BX);
    tPoint->column( "cx"     , CX);
    tPoint->column( "dx"     , DX);
    tPoint->column( "ay"     , AY);
    tPoint->column( "by"     , BY);

    tPoint->column( "xMagnet",  origin.x() + (zMagnet-origin.z()) * slope.x() );
    tPoint->column( "zMagnet", zMagnet );
    tPoint->column( "dSlope", dSlope );
    tPoint->column( "McSlopeX",  myMCPart->momentum().x() / myMCPart->momentum().z() );
    tPoint->column( "McSlopeY",  myMCPart->momentum().y() / myMCPart->momentum().z() );
    tPoint->column( "dp", 1 - Gaudi::Units::GeV*tmom*proj/dSlopeOpt/momentum );
    tPoint->write();

  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FwdFitParams::finalize() {

  info() << "===========================================" << endmsg;
  info() << "We have processed " << m_count[0]
         << " tracks. " << endmsg;
  if ( 0 != m_count[0] ) {
    info() << "    " << m_count[1] << " ("
           << 100.*m_count[1]/m_count[0] << " % ) with good Khi2" << endmsg
           << "    " << m_count[2] << " with good momentum match " << endmsg;
  }
  MsgStream& msg = info() << "==========================================="
                          << endmsg;

  if ( 100 < m_count[0] ) {

    // *** Solve the first change matrix

    m_ZmPar.updateParameters ( msg );
    m_xPar.updateParameters ( msg );
    m_yPar.updateParameters ( msg );
    m_y1Par.updateParameters ( msg );

    m_momPar.updateParameters( msg );

    m_XsPar.updateParameters( msg );

    info() << endmsg << "-- For JobOptions -- " << endmsg << endmsg;

    std::cout << std::endl;

    m_ZmPar.printPythonParams( name() );
    m_xPar.printPythonParams( name() );
    m_yPar.printPythonParams( name() );
    m_y1Par.printPythonParams( name() );

    m_momPar.printPythonParams(name() );
    m_XsPar.printPythonParams( name() );

    std::string toolName = "ToolSvc.PatFwdTool";
    std::cout << std::endl;
    
    m_ZmPar.printParams( toolName );
    m_xPar.printParams( toolName );
    std::cout << toolName << ".yParams = { "<< m_yPar.param(0) << ", " << m_y1Par.param(0) << " };" << std::endl;
    m_momPar.printParams(toolName );
    m_XsPar.printParams( toolName );
    std::cout << std::endl;
  }

  return GaudiTupleAlg::finalize();
}

//=============================================================================
