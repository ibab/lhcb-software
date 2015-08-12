// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from LHCbEvent
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/MCHit.h"

// local
#include "KsFitParams.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KsFitParams
//
// 2002-11-02 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( KsFitParams )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
KsFitParams::KsFitParams( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_fitTool(NULL)
{
  declareProperty( "NTupleName",     m_tupleName  = "Track" );
  declareProperty( "ZTT1",           m_zTT1       = 2469.0*Gaudi::Units::mm );
  declareProperty( "ZRef",           m_zRef       = 9410.0*Gaudi::Units::mm );
  declareProperty( "zMagnetParams",  m_zMagParams );
  declareProperty( "momentumParams", m_momParams  );
  
  m_nEvent = 0;
  m_nTrack = 0;
}

//=============================================================================
// Destructor
//=============================================================================
KsFitParams::~KsFitParams() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode KsFitParams::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_zMagPar.init( "zMagnet"  , m_zMagParams );
  m_momPar.init ( "momentum" , m_momParams );

  m_fitTool = tool<FitTool>( "FitTool" );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode KsFitParams::execute() {
  debug() << "==> Execute" << endmsg;

  m_nEvent++;

  LHCb::MCParticles* partCtnr = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

  // Get the IT and TT hits
  LHCb::MCHits* itHits = get<LHCb::MCHits>( LHCb::MCHitLocation::IT );
  LHCb::MCHits* ttHits = get<LHCb::MCHits>( LHCb::MCHitLocation::TT );

  // Get the OT hits
  LHCb::MCHits* otHits = get<LHCb::MCHits>( LHCb::MCHitLocation::OT );

  LHCb::MCParticles::const_iterator pItr;
  const LHCb::MCParticle* part;
  SmartRefVector<LHCb::MCVertex> vDecay;
  
  const LHCb::MCParticle* kShort  = 0;
  const LHCb::MCVertex*   kDecay  = 0;
  
  // A container for used hits
  std::vector<Gaudi::XYZPoint> trHits;
  std::vector<Gaudi::XYZPoint> TTHits;
  
  for ( pItr = partCtnr->begin(); partCtnr->end() != pItr; pItr++ ) {
    part = *pItr;

    // Find the Pi from the kShort
    if ( 211 == abs( part->particleID().pid() ) ) { 
      kDecay = part->originVertex();
      if ( 0 == kDecay ) continue;
      kShort = kDecay->mother();
      if ( 0 == kShort ) continue;
      if ( 310 != kShort->particleID().pid() ) continue;
      const LHCb::MCVertex* origin = kShort->originVertex();
      if ( 0 == origin ) continue;
      if ( 8. < origin->position().R() ) continue;  // Kshorts from close the beam line

      bool hasInteractionVertex = false;
      SmartRefVector<LHCb::MCVertex> endV = part->endVertices();
      for ( SmartRefVector<LHCb::MCVertex>::const_iterator itV = endV.begin() ;
            endV.end() != itV; itV++ ) {
        if ( (*itV)->position().z() < 9500. ) hasInteractionVertex = true;
      }
      if ( hasInteractionVertex ) continue;
    
      debug() << "--- Found pi key " << part->key() << endmsg;
    
      TTHits.clear();
      trHits.clear();
   
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
      if ( 3  > TTHits.size() || 12 > trHits.size() )  continue;

      debug() << "=== Found a good K0S Decay : " 
              << kShort->key() << " decay at " 
              << format( "%7.1f %7.1f %7.1f", 
                         kDecay->position().x(),
                         kDecay->position().y(),
                         kDecay->position().z() )
              << endmsg;
      debug() << " pion momentum = " << part->momentum().R() / Gaudi::Units::GeV << " GeV"
              << endmsg;
      
      //== Fill ntuple
      double pz = kShort->momentum().Z();
      double kSlXT = kShort->momentum().X() / pz;
      double kSlYT = kShort->momentum().Y() / pz;

      Tuple tTrack = nTuple( m_tupleName, m_tupleName );

      m_nTrack++;

      tTrack->column( "KVertX", kDecay->position().X() );
      tTrack->column( "KVertY", kDecay->position().Y() );
      tTrack->column( "KVertZ", kDecay->position().Z() );
      tTrack->column( "KMoment", kShort->momentum().R() );
      tTrack->column( "KSlopeX", kSlXT );
      tTrack->column( "KSlopeY", kSlYT );

      //== Fit the Pi+
      pz = part->momentum().Z();
      tTrack->column( "moment",  part->momentum().R() );
      tTrack->column( "slopeX",  part->momentum().X() / pz );
      tTrack->column( "slopeY",  part->momentum().Y() / pz );
    
      //== Fit the TT area
      double axt, bxt, ayt, byt, dz;
      debug() << "  TT: ";
      m_fitTool->fitLine( TTHits, 0, m_zTT1, axt, bxt );
      m_fitTool->fitLine( TTHits, 1, m_zTT1, ayt, byt );
      debug() << format( " x %7.1f tx %7.4f   y %7.1f ty %7.4f ",
                         axt, bxt, ayt, byt ) 
              << endmsg;;
      tTrack->column( "axt" , axt );
      tTrack->column( "bxt", bxt );
      tTrack->column( "ayt" , ayt );
      tTrack->column( "byt", byt );
      
      std::vector<Gaudi::XYZPoint>::const_iterator itP;
      if ( msgLevel( MSG::DEBUG ) ) {
        for ( itP = TTHits.begin(); TTHits.end() > itP; itP++ ) {
          dz = (*itP).z()-m_zTT1;
          debug() << format( "    : %7.1f %7.1f %7.1f  dx %7.3f  dy %7.3f",
                             (*itP).x(), (*itP).y(), (*itP).z(),
                             (*itP).x()-(axt+bxt*dz),
                             (*itP).y()-(ayt+byt*dz)
                             ) << endmsg;
        }
      }
      
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
      
      double zMagnet = (axt-bxt*m_zTT1 - (ax-bx*m_zRef) ) / (bx-bxt);
      tTrack->column( "zMagnet", zMagnet );
      m_zMagPar.setFun( 0, 1. );
      m_zMagPar.setFun( 1, by*by );
      m_zMagPar.setFun( 2, bx*bx );
      
      double zEst = m_zMagPar.sum();
      tTrack->column( "zEst", zEst );
      m_zMagPar.addEvent( zMagnet-zEst );

      double bytPred = by + 5. * by * fabs(by) * (bx-bxt) * (bx-bxt);
      double aytPred = ay + (zMagnet-m_zRef)*by + (m_zTT1-zMagnet)*bytPred;
      aytPred -= 2000*by*(bx-bxt)*(bx-bxt);
      
      tTrack->column( "bytPred", bytPred );
      tTrack->column( "aytPred", aytPred );
      
      tTrack->column( "xMagnet", axt + ( zMagnet - m_zTT1 ) * bxt );
      tTrack->column( "yMagnet", ayt + ( zMagnet - m_zTT1 ) * byt );
      
      m_momPar.setFun( 0, 1. );
      m_momPar.setFun( 1, bx * bx );
      m_momPar.setFun( 2, by * by );
      
      double dSlope = fabs( bx - bxt );
      double dp = dSlope * part->momentum().R() - m_momPar.sum();
      
      m_momPar.addEvent( dp );
      tTrack->column( "PredMom", m_momPar.sum() / dSlope );
      
      double xMag = ax + ( zEst - m_zRef ) * bx;
      tTrack->column( "xPred", xMag * m_zTT1 / zEst );
      tTrack->write();
      
      debug() << format( "  zMag%7.1f xMag%7.1f yMag%7.1f back%7.1f",
                         zMagnet,
                         axt + ( zMagnet - m_zTT1 ) * bxt,
                         ayt + ( zMagnet - m_zTT1 ) * byt,
                         ay  + ( zMagnet - m_zRef ) * by )
              << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode KsFitParams::finalize() {

  debug() << "==> Finalize" << endmsg;

  MsgStream& msg = info() << "============================================" << endmsg;
  msg << "  Processed " << m_nEvent << " events and " << m_nTrack << " tracks. " << endmsg;
  msg << "============================================" << endmsg;
  m_zMagPar.updateParameters( msg );
  m_momPar.updateParameters(  msg );

  std::cout << std::endl;
  m_zMagPar.printPythonParams( name() );
  m_momPar.printPythonParams(  name() );
  std::cout << std::endl;

  m_zMagPar.printParams( "PatKShort" );
  m_momPar.printParams(  "PatKShort" );
  std::cout << std::endl;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}
//=============================================================================
