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
#include "PrFitFwdParams.h"
#include "PrFitPolinomial.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrFitFwdParams
//
// 13/10/2001 : Olivier Callot
// 2013/01/23  : Yasmine Amhis
// Adapt to work with Fiber Tracker and VP
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PrFitFwdParams )

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
PrFitFwdParams::PrFitFwdParams( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
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
  , m_zBeforeST3          ( 8900. * Gaudi::Units::mm )
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
  declareProperty( "xParams"              , m_xParams       );
  declareProperty( "byParams"             , m_byParams      );
  declareProperty( "cyParams"             , m_cyParams      );
  declareProperty( "momentumParams"       , m_momParams     );

  declareProperty( "XsParams"             , m_XsParams      );

  declareProperty( "ZbeforeST2"           , m_zBeforeST2    );
  declareProperty( "ZbeforeST3"           , m_zBeforeST3    );

  declareProperty( "VeloFromMC"           , m_veloFromMC = false );
  // Next option is obsolete, kept for backward compatibility, VL no longer supported
  declareProperty( "useVeloPix"           , m_useVeloPix = true );

  declareProperty( "VeloTracksLocation", 
                   m_veloTracksLocation = LHCb::TrackLocation::Velo);

  for ( unsigned int i=0 ; 3 > i ; i++ ) {
    m_count[i] = 0;
  }


}

//=============================================================================
// Destructor
//=============================================================================
PrFitFwdParams::~PrFitFwdParams() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrFitFwdParams::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_ZmPar.init ( "zMagnet" , m_zMagParams );
  m_xPar.init  ( "x"        , m_xParams );
  m_byPar.init ( "by"       , m_byParams );
  m_cyPar.init ( "cy"       , m_cyParams );

  m_momPar.init( "momentum", m_momParams );

  m_XsPar.init( "Xs"      , m_XsParams );

  // MC - 2014-01-13: For backward compatibilty, when VL was also supported
  if (m_useVeloPix == false ){
    return Error("useVeloPix option is obsolete, should always be true, VL no longer supported", StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrFitFwdParams::execute() {

  LHCb::MCParticles* partCtnr = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  // Get the Velo hits
  //LHCb::MCHits* vHits = get<LHCb::MCHits>( LHCb::MCHitLocation::VP );

  const LHCb::MCHits* vHits = get<LHCb::MCHits>( LHCb::MCHitLocation::VP );
 
  // Get the FT hits
  LHCb::MCHits* ftHits = get<LHCb::MCHits>( LHCb::MCHitLocation::FT );

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

  double zMagnet = 0.0;
  double xMagnet = 0.0;

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
        PrFitPolinomial fitX( 1 );
        PrFitPolinomial fitY( 1 );
        for ( std::vector<Gaudi::XYZPoint>::iterator pt = vPoints.begin() ; 
              vPoints.end() != pt  ; pt++ ) {
          dz = (*pt).z() - m_zVelo;
          fitX.fill(  (*pt).x(), dz );
          fitY.fill(  (*pt).y(), dz );
        }
        fitX.solve();
        fitY.solve();
        
        origin.SetX( fitX.param(0) );
        origin.SetY( fitY.param(0) );
        origin.SetZ( m_zVelo );
        slope.SetX( fitX.param(1) );
        slope.SetY( fitY.param(1) );
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
    trHits.reserve(12);

    // Get the FT hits

    for ( LHCb::MCHits::const_iterator fHitIt = ftHits->begin() ;
          ftHits->end() != fHitIt ; fHitIt++ ) {
      if ( (*fHitIt)->mcParticle() ==  myMCPart ) {
        trHits.push_back((*fHitIt)->midPoint());
        if ( msgLevel( MSG::VERBOSE ) ) {
          verbose() << format( " Add FT x%9.3f y%9.3f z%10.3f", 
                               (*fHitIt)->midPoint().x(),
                               (*fHitIt)->midPoint().y(),
                               (*fHitIt)->midPoint().z() ) << endmsg;
        }
      }
    }

    if ( 12 != trHits.size() ) continue;
    m_count[0] += 1;

    std::vector<Gaudi::XYZPoint>::iterator pt;
    double chi2 = 1000.;
    double AX = 0.;
    double BX = 0.;
    double CX = 0.;
    double DX = 0.;
    double AY = 0.;
    double BY = 0.;
    double CY = 0.;
    std::vector<Gaudi::XYZPoint>::iterator badGuy;

    // Now we have all the measures on the track. Compute the track parameters
    // We need a fix Z plane for reference, m_zRef, and the z of the magnet
    // center m_zMag.
    // A track is defined as x = AX + BX*dz + CX*dz*dz + DX*dz*dz*dz
    //                       y = AY + BY*dz
    // AX, BX, CX, DX, AY and BY will be later fitted as function of the
    //                               track measured parameters.
    // Compute the parameters: First compute the various sums
    
    PrFitPolinomial fitX( 3 );

    for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
      if ( m_zMag < (*pt).Z() ) {
        dz = ( (*pt).z() - m_zRef ) * 1.e-3;      // Keep numbers reasonable !
        fitX.fill(  (*pt).X(), dz );
      }
    }
    if ( fitX.solve() ) {
      AX = fitX.param( 0 );
      BX = fitX.param( 1 ) * 1.e-3 ;
      CX = fitX.param( 2 ) * 1.e-6 ;
      DX = fitX.param( 3 ) * 1.e-9 ;
    } else {
      err() << " X matrix is singular " << endmsg;
      continue;
    }
    
    // For Y also
    
    PrFitPolinomial fitY( 2 );
    
    for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
      if ( m_zMag < (*pt).Z() ) {
        dz = ( (*pt).Z() - m_zRef ) * 1.e-3;      // Keep numbers reasonable !
        fitY.fill(  (*pt).Y(), dz );
      }
    }
    if ( fitY.solve() ) {
      AY = fitY.param( 0 );
      BY = fitY.param( 1 ) * 1.e-3 ;
      CY = fitY.param( 2 ) * 1.e-6 ;
    } else {
      err() << " Y matrix is singular " << endmsg;
      continue;
    }
    
    // now check the distance / quality
    
    chi2 = 0;
    double dist;
    int nbMeas = 0;
    double worst = 0.;
    
    for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
      if ( m_zMag < (*pt).Z() ) {
        nbMeas++;
        dz = (*pt).Z() - m_zRef;
        dx = (*pt).X() - (AX +dz*(BX + dz*(CX + dz*DX)));
        dy = (*pt).Y() - (AY +dz*(BY + dz* CY ));
        
        // Fit error. Arbitrary = 100 microns in X, 1mm in Y, as this is MC hit position.

        dist = (dx*dx / .01) + dy*dy;
        chi2 += dist;
        if ( worst < dist ) {
          worst  = dist;
          badGuy = pt;
        }
      }
    }
    chi2 = chi2 / (nbMeas-7);
    verbose() << " Track Chi2 = " << chi2 << endmsg;
    if ( 5. < chi2 ) {
      info() << "-- Bad track : Chi2 = " << chi2
             << " momentum " << momentum/Gaudi::Units::GeV
             << endmsg;
      
      for ( pt = trHits.begin() ; trHits.end() != pt  ; pt++ ) {
        dz = (*pt).Z() - m_zRef;
        dx = (*pt).X() - (AX +dz*(BX+dz*(CX+dz*DX)));
        dy = (*pt).Y() - (AY +dz*(BY+dz*CY) );
        info() << format( " Z %8.1f  X %8.1f  dx %8.3f  Y %8.1f dy %8.3f",
                          (*pt).Z(),
                          (*pt).X(), dx,
                          (*pt).Y(), dy )
               << endmsg;
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
      xSeed = (*pt).x();
      ySeed = (*pt).y();
      zSeed = (*pt).z();
      dz = ( zSeed - m_zRef ) * 1.e-3;
      
      m_ZmPar.setFun( 0, 1. );
      m_ZmPar.setFun( 1, 0. );
      m_ZmPar.setFun( 2, slopeX2 );
      m_ZmPar.setFun( 3, slopeY2 );

      dSlope  = 0.;
      double tax     = 0.;
        
      double zMagnif = 0.;
      double xPred   = 0.;

      for ( int nLoop = 0 ; 5 > nLoop ; ++nLoop ) {
        dSl2 = dSlope * dSlope;
        m_ZmPar.setFun( 1, dSl2 );
        zMagnet = m_ZmPar.sum();
        xMagnet = origin.x() + ( zMagnet - origin.z() ) * slope.x();
        
        m_xPar.setFun( 0, dz*dz* dSlope  );
        m_xPar.setFun( 1, dz*dz*dz* dSlope );
        tax     = m_xPar.sum();
        
        zMagnif = ( m_zRef - zMagnet)/(zSeed - zMagnet);
        xPred   = xMagnet + (xSeed - tax -xMagnet) * zMagnif;
        
        tbx     = ( xPred -xMagnet )/(m_zRef - zMagnet );
        dSlope  = tbx - slope.x();
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
    double slopeYPred = slope.y();

    m_byPar.setFun( 0, slope.y() * dSlope * dSlope );
    m_cyPar.setFun( 0, slope.y() * dSlope * dSlope );
    
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
      m_byPar.addEvent( BY - slopeYPred - m_byPar.sum() );
      m_cyPar.addEvent( CY - m_cyPar.sum() );

    }
    // Fill the ntuple

    Tuple tPoint = nTuple( m_tupleName, m_tupleName );
    tPoint->column( "xVelo"  , origin.x() );
    tPoint->column( "yVelo"  , origin.y() );
    tPoint->column( "slopeX" , slope.x());
    tPoint->column( "slopeY" , slope.y());
    tPoint->column( "momentum", momentum / Gaudi::Units::GeV);
    tPoint->column( "ypred"  , origin.y() + slope.y() * ( m_zRef - origin.z() ) );

    tPoint->column( "ax"     , AX);
    tPoint->column( "bx"     , BX);
    tPoint->column( "cx"     , CX);
    tPoint->column( "dx"     , DX);
    tPoint->column( "ay"     , AY);
    tPoint->column( "by"     , BY);
    tPoint->column( "cy"     , CY);

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
StatusCode PrFitFwdParams::finalize() {

  info() << "===========================================" << endmsg;
  info() << "We have processed " << m_count[0]
         << " tracks. " << endmsg;
  if ( 0 != m_count[0] ) {
    info() << "    " << m_count[1] << " ("
           << 100.*m_count[1]/m_count[0] << " % ) with good Chi2" << endmsg
           << "    " << m_count[2] << " with good momentum match " << endmsg;
  }
  MsgStream& msg = info() << "==========================================="
                          << endmsg;

  if ( 100 < m_count[0] ) {

    // *** Solve the first change matrix

    m_ZmPar.updateParameters ( msg );
    m_xPar.updateParameters ( msg );
    m_byPar.updateParameters ( msg );
    m_cyPar.updateParameters ( msg );
    m_momPar.updateParameters( msg );

    m_XsPar.updateParameters( msg );

    info() << endmsg << "-- For JobOptions -- " << endmsg << endmsg;

    std::cout << std::endl;

    std::string myName = name() + "()";
    m_ZmPar.printPythonParams( myName );
    m_xPar.printPythonParams(  myName );
    m_byPar.printPythonParams( myName );
    m_cyPar.printPythonParams( myName );
    m_momPar.printPythonParams(myName );
    m_XsPar.printPythonParams( myName );

    std::string toolName = "ToolSvc().PrGeometryTool";
    std::cout << std::endl;
    
    m_ZmPar.printPythonParams( toolName );
    m_xPar.printPythonParams( toolName );
    m_byPar.printPythonParams( toolName );
    m_cyPar.printPythonParams( toolName );
    m_momPar.printPythonParams(toolName );
    m_XsPar.printPythonParams( toolName );
    std::cout << std::endl;
  }

  return GaudiTupleAlg::finalize();
}

//=============================================================================
