// $Id: KsFitParams.cpp,v 1.5 2009/08/19 14:24:18 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHit.h"

// local
#include "MatchFitParams.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MatchFitParams
//
// 2002-11-02 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MatchFitParams )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MatchFitParams::MatchFitParams( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "NTupleName",        m_tupleName         = "Track" );
  declareProperty( "ZTT1",              m_zTT1              = 2469.0*Gaudi::Units::mm );
  declareProperty( "ZRef",              m_zRef              = 9410.0*Gaudi::Units::mm );
  declareProperty( "ZVelo",             m_zVelo             = 0.0*Gaudi::Units::mm );
  declareProperty( "zMagnetParams",     m_zMagParams );
  declareProperty( "momentumParams",    m_momParams  );
  declareProperty( "bendYParams",       m_bendYParams );
  declareProperty( "MaxZVertex"     ,   m_maxZVertex        = 500*Gaudi::Units::mm    );
  declareProperty( "MinMomentum"    ,   m_minMomentum       = 2*Gaudi::Units::GeV   );
  declareProperty( "zMatchY"        ,   m_zMatchY           =  8420*Gaudi::Units::mm    );
  declareProperty( "RequireTTHits"  ,   m_requireTTHits     = false    );
  declareProperty( "MagnetScaleFactor", m_magnetScaleFactor = -1 );
  
  m_nEvent = 0;
  m_nTrack = 0;
}

//=============================================================================
// Destructor
//=============================================================================
MatchFitParams::~MatchFitParams() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MatchFitParams::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if( m_zMagParams.empty() ) info() << "no starting values for magnet parameters provided. Will not calculate anything" << endmsg;
  if( m_momParams.empty() ) info() << "no starting values for momentum  parameters provided. Will not calculate anything" << endmsg;
  if( m_bendYParams.empty() ) info() << "no starting values for y bending  parameters provided. Will not calculate anything" << endmsg;
  
  
  m_zMagPar.init( "zMagnet"  , m_zMagParams );
  m_momPar.init ( "momentum" , m_momParams );
  m_bendParamY.init( "bendParamY", m_bendYParams);
  
  info() << "Magnet scale factor is: " << m_magnetScaleFactor << endmsg;
  
  
  m_fitTool = tool<FitTool>( "FitTool" );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MatchFitParams::execute() {

  debug() << "==> Execute" << endmsg;

  m_nEvent++;

  debug() << "Processing event: " << m_nEvent << endmsg;
  


  LHCb::MCParticles* partCtnr = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  // Get the IT and TT hits
  LHCb::MCHits* itHits = get<LHCb::MCHits>( LHCb::MCHitLocation::IT );
  LHCb::MCHits* ttHits = get<LHCb::MCHits>( LHCb::MCHitLocation::TT );

  // Get the OT hits
  LHCb::MCHits* otHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );

  // Get the Velo hits
  LHCb::MCHits* veloHits = get<LHCb::MCHits>( LHCb::MCHitLocation::Velo );

  LHCb::MCParticles::const_iterator pItr;
  const LHCb::MCParticle* part;
  SmartRefVector<LHCb::MCVertex> vDecay;
  
  // A container for used hits
  std::vector<Gaudi::XYZPoint> trHits;
  std::vector<Gaudi::XYZPoint> TTHits;
  std::vector<Gaudi::XYZPoint> vHits;

  MCTrackInfo trackInfo( evtSvc(), msgSvc() );
  

  for ( pItr = partCtnr->begin(); partCtnr->end() != pItr; pItr++ ) {
    part = *pItr;

     // only long track reconstructible
    if( !trackInfo.hasVeloAndT( part ) ) continue;

    // Select particles to work with:
    // == Origin vertex position
    const LHCb::MCVertex*  vOrigin = part->originVertex();
    if ( nullptr == vOrigin ) continue;
    if ( m_maxZVertex < vOrigin->position().z() ) continue;
    // == Decay vertices
    SmartRefVector< LHCb::MCVertex > vDecay = part->endVertices();
    double zDecay;

    if ( 0 < vDecay.size()  ) {
      zDecay = vDecay[0]->position().z();
    } else {
      zDecay = 20. * Gaudi::Units::m;
    }
    if ( 10000. > zDecay ) continue;

    // == Momentum cut
    double momentum = part->momentum().R();
    if ( m_minMomentum > momentum ) continue;
    if ( 11 == abs( part->particleID().pid() ) ) { continue; } // no electrons

    bool hasInteractionVertex = false;

    SmartRefVector<LHCb::MCVertex> endV = part->endVertices();
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
    
   
    

    debug() << "--- Found particle key " << part->key() << endmsg;
    
    TTHits.clear();
    trHits.clear();
    vHits.clear();
    
    // Get the Velo hits
    for ( LHCb::MCHits::const_iterator vHitIt = veloHits->begin() ;
          veloHits->end() != vHitIt ; vHitIt++ ) {
      if ( (*vHitIt)->mcParticle() ==  part ) {
        vHits.push_back( (*vHitIt)->midPoint() );
      }
    }
    
    // Get the IT hits
    for ( LHCb::MCHits::const_iterator iHitIt = itHits->begin() ; 
          itHits->end() != iHitIt ; iHitIt++ ) {
      if ( (*iHitIt)->mcParticle() ==  part ) {
        trHits.push_back( (*iHitIt)->midPoint() );
      }
    }
    
    // Get the TT hits
    for ( LHCb::MCHits::const_iterator iHittt = ttHits->begin() ; 
          ttHits->end() != iHittt ; iHittt++ ) {
      if ( (*iHittt)->mcParticle() ==  part ) {
        TTHits.push_back( (*iHittt)->midPoint() );
      }
    }
    
    // Get the OT hits
    for ( LHCb::MCHits::const_iterator oHitIt = otHits->begin() ; 
          otHits->end() != oHitIt ; oHitIt++ ) {
      if ( (*oHitIt)->mcParticle() ==  part ) {
        trHits.push_back( (*oHitIt)->midPoint() );
      }
    }
    if ( 12 > trHits.size() || 5 > vHits.size() )  continue;
    if( m_requireTTHits){
      if( 3 > TTHits.size() ) continue;
    }
    

    debug() << " particle momentum = " << part->momentum().R() / Gaudi::Units::GeV << " GeV"
            << endmsg;
    
    //== Fill ntuple
    double pz = part->momentum().Z();
    double plXT = part->momentum().X() / pz;
    double plYT = part->momentum().Y() / pz;
    
    Tuple tTrack = nTuple( m_tupleName, m_tupleName );
    
    tTrack->column("pz",pz);
    tTrack->column("plXT", plXT);
    tTrack->column("plYT", plYT);
    
    
    m_nTrack++;
    
    //== Fit the TT area
    // -- This is not needed at the moment for the matching, but it was kept it 
    // -- as it might be used at some point in the future
    double axt, bxt, ayt, byt, dz;
    double axt2,bxt2,cxt2;
    
    debug() << "  TT: ";
    m_fitTool->fitLine( TTHits, 0, m_zTT1, axt, bxt );
    m_fitTool->fitLine( TTHits, 1, m_zTT1, ayt, byt );
    m_fitTool->fitParabola( TTHits, 0, m_zTT1, axt2, bxt2, cxt2 );
    //m_fitTool->fitLine( TTHits, 1, m_zTT1, ayt, byt, cyt );
    debug() << format( " x %7.1f tx %7.4f   y %7.1f ty %7.4f ",
                       axt, bxt, ayt, byt ) 
            << endmsg;;
    tTrack->column( "axt" , axt );
    tTrack->column( "bxt", bxt );
    tTrack->column( "ayt" , ayt );
    tTrack->column( "byt", byt );
    tTrack->column( "axt2", axt2 );
    tTrack->column( "bxt2", bxt2 );
    tTrack->column( "cxt2", cxt2 );
    
    
    std::vector<Gaudi::XYZPoint>::const_iterator itP;
    if ( msgLevel( MSG::DEBUG ) ) {
      for ( itP = TTHits.begin(); TTHits.end() > itP; itP++ ) {
        dz = (*itP).z()-m_zTT1;
        //double dx = (*itP).x()-(axt+bxt*dz);
        //tTrack->column( "residual", dx );
        //tTrack->column( "dz2", dz );
        //tTrack->column("dSlope2",dSlope);
        debug() << format( "    : %7.1f %7.1f %7.1f  dx %7.3f  dy %7.3f",
                           (*itP).x(), (*itP).y(), (*itP).z(),
                           (*itP).x()-(axt+bxt*dz),
                           (*itP).y()-(ayt+byt*dz)
                           ) << endmsg;
      }
    }
    
    // -- Fit the T-stations
    // -- x(z) = ax + bx*z + cx*z*z + dx*z*z*z
    // -- y(z) = ay + by*z
    double ax, bx, cx, dx, ay, by;
    m_fitTool->fitCubic( trHits, 0, m_zRef, ax, bx, cx, dx );
    m_fitTool->fitLine(  trHits, 1, m_zRef, ay, by );
    tTrack->column( "ax", ax );
    tTrack->column( "bx", bx );
    tTrack->column( "cx", cx );
    tTrack->column( "dx", dx );
    tTrack->column( "ay", ay );
    tTrack->column( "by", by );
    
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << format( "  tr: x%7.1f %7.4f %7.3f %7.3f  y%7.1f %7.4f", 
                         ax, bx, 1.e6*cx, 1.e9*dx, ay, by ) << endmsg;
      
      for ( itP = trHits.begin(); trHits.end() > itP; itP++ ) {
        dz = (*itP).z()-m_zRef;
        debug() << format( "    : %7.1f %7.1f %7.1f  dx %7.3f  dy %7.3f",
                           (*itP).x(), (*itP).y(), (*itP).z(),
                           (*itP).x()-(ax + bx*dz + cx*dz*dz + dx*dz*dz*dz),
                           (*itP).y()-(ay + by*dz)
                           ) << endmsg;
      }
    }
    
    // -- Fit the velo area
    // -- x(z) = axv + bxv*z
    // -- y(z) = ayv + byv*z
    double axv, bxv, ayv, byv;
    m_fitTool->fitLine(  vHits, 0, m_zVelo, axv, bxv );
    m_fitTool->fitLine(  vHits, 1, m_zVelo, ayv, byv );
    tTrack->column( "axv", axv );
    tTrack->column( "bxv", bxv );
    tTrack->column( "ayv", ayv );
    tTrack->column( "byv", byv );
    
    if ( msgLevel( MSG::DEBUG ) ) {
      debug() << format( "  velo: x%7.1f %7.4f y%7.1f %7.4f", 
                         axv, bxv, ayv, byv ) << endmsg;
      
      for ( itP = vHits.begin(); vHits.end() > itP; itP++ ) {
        dz = (*itP).z()-m_zRef;
        debug() << format( "    : %7.1f %7.1f %7.1f  dx %7.3f  dy %7.3f",
                           (*itP).x(), (*itP).y(), (*itP).z(),
                           (*itP).x()-(ax + bx*dz + cx*dz*dz + dx*dz*dz*dz),
                           (*itP).y()-(ay + by*dz)
                           ) << endmsg;
      }
    }
    
    // -- This is for finding the zMagnet position when using straight lines from the Velo and the T-stations
    // -- Only really makes sense in x, as for y the different y resolutions of Velo and T-stations matter a lot
    double zMagnet = (axv-bxv*m_zVelo - (ax-bx*m_zRef) ) / (bx-bxv);
    double zMagnety = (ayt-byt*m_zTT1 - (ay-by*m_zRef) ) / (by-byt);
    double dSlope =  fabs(bx - bxv);
    double dSlopeY = fabs(by - byv);
   
    m_zMagPar.setFun( 0, 1. );
    m_zMagPar.setFun( 1, dSlope*dSlope );
    //m_zMagPar.setFun( 2, ax*ax );
    //m_zMagPar.setFun( 3, bx*bx );
    m_zMagPar.setFun( 2, bxv*bxv );
    m_zMagPar.setFun( 3, dSlope );
    m_zMagPar.setFun( 4, fabs(ax) );

    double zEst = m_zMagPar.sum();
    
    m_zMagPar.addEvent( zMagnet-zEst );

    tTrack->column( "zEst", zEst );
    tTrack->column( "zMagnet", zMagnet );
    tTrack->column( "zMagnety", zMagnety );

    // -- This is the parameter that defines the bending in y
    double bendParamY = (ay - ayv) + ( m_zMatchY - m_zRef )*by - (m_zMatchY - m_zVelo)*byv;
    
    m_bendParamY.setFun( 0, dSlope*dSlope*byv );
    m_bendParamY.setFun( 1, dSlopeY*dSlopeY*byv );
    
    double bendParamYEst = m_bendParamY.sum();
    m_bendParamY.addEvent( bendParamY - bendParamYEst );
    // ------------------------------------------------------
    
    tTrack->column("dSlope",dSlope);
    tTrack->column("dSlope2",dSlope*dSlope);
  
    // -- Need to write something to calculate the momentum Params
    const double charge = part->particleID().threeCharge()/3;
    const double qOverP = charge/momentum;
    
    // -- The magnet scale factor is hardcoded, as then one does not need to run the 
    // -- field service 
    const double proj = sqrt( ( 1. + bxv*bxv + byv*byv ) / ( 1. + bxv*bxv ) );
    const double coef = (bxv - bx)/(proj*m_magnetScaleFactor*Gaudi::Units::GeV*qOverP);
    
    m_momPar.setFun(0, 1.);
    m_momPar.setFun(1, bx*bx );
    m_momPar.setFun(2, bx*bx*bx*bx );
    m_momPar.setFun(3, bx*bxv );
    m_momPar.setFun(4, byv*byv );
    m_momPar.setFun(5, byv*byv*byv*byv );
    
    double coefEst = m_momPar.sum();
    m_momPar.addEvent( coef - coefEst );
    
    tTrack->write();
    

  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MatchFitParams::finalize() {

  debug() << "==> Finalize" << endmsg;

  MsgStream& msg = info() << "============================================" << endmsg;
  msg << "  Processed " << m_nEvent << " events and " << m_nTrack << " tracks. " << endmsg;
  msg << "============================================" << endmsg;
  m_zMagPar.updateParameters( msg );
  m_momPar.updateParameters(  msg );
  m_bendParamY.updateParameters( msg );

  std::cout << std::endl;
  m_zMagPar.printPythonParams( name() );
  m_momPar.printPythonParams(  name() );
  m_bendParamY.printPythonParams( name() );
  std::cout << std::endl;

  m_zMagPar.printParams( "PatMatch" );
  m_momPar.printParams(  "PatMatch" );
  m_bendParamY.printParams( "PatMatch" );
  std::cout << std::endl;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}
//=============================================================================
