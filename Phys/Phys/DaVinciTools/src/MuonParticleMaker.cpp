// $Id: MuonParticleMaker.cpp,v 1.2 2002-09-11 16:45:13 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// local
#include "MuonParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonParticleMaker
//
// Description: This tool fills the particle class with information from
// ProtoParticles and places it in the Transient Event Store
// 22/07/2002 : Jose Helder Lopes.
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<MuonParticleMaker>          s_factory ;
const        IToolFactory& MuonParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonParticleMaker::MuonParticleMaker( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_input( ProtoParticleLocation::Charged )
  , m_confLevel( 0.00 )
  , m_ppSvc(0)
  , m_EDS(0) 
  {
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Declare properties
  declareProperty( "InputProtoP", m_input );
  declareProperty( "ConfLevelCut", m_confLevel );
  
}
//=============================================================================
// Destructor
//=============================================================================
MuonParticleMaker::~MuonParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonParticleMaker::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "MuonParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::DEBUG << "Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }
  ParticleProperty* partProp = m_ppSvc->find( "mu-" );
  if ( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for muon" << endreq;
    return StatusCode::FAILURE;
  }
  m_muonPID = partProp->jetsetID();
  
  log << MSG::DEBUG << "Muon ( mu- ) Id code: " << m_muonPID  << endreq;  
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* MuonParticleMaker::eventSvc() const
{
  return m_EDS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  log( msgSvc(), name() );
  
  log << MSG::DEBUG << "MuonParticleMaker::makeParticles() is running." 
      << endreq;
  
  double protoCL;
  
  // make charged particles:
  SmartDataPtr<ProtoParticles> candidates ( eventSvc(),m_input);
  if ( !candidates || (0 == candidates->size()) ) { 
    log << MSG::INFO << "No Charged ProtoParticles retrieved from"  
        << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Log number of ProtoPartCandidates retrieved
  log << MSG::DEBUG << "Number of ProtoParticles retrieved from "
      << m_input << " = " << candidates->size() << endreq;
  
  int nMuons=0;   // Counter of muon particles created.
  
  // Loop over all ProtoParticles and fill Particle if the ProtoParticle 
  // satisfies the PID and corresponding CL cut  :
  ProtoParticles::iterator icand = 0;  // Iterator on ProtoParticles.
  for(icand = candidates->begin();icand != candidates->end();icand++){
    protoCL = (*icand)->detPIDvalue( ProtoParticle::MuonMuon );
    log << MSG::VERBOSE <<  "Muon CL = " << protoCL << endreq;
    log << MSG::VERBOSE <<  "Best Id = " <<  (*icand)->bestPID() 
        << " with CL " <<  (*icand)->probOfBestPID() << endreq;
    if ( protoCL >= m_confLevel ) {
      
      // We have a good candidate for Muon:
      double charge = (*icand)->charge();
      Particle* particle = new Particle();
      int muonID = -((int)charge)*m_muonPID;
      StatusCode sc = fillParticle(*icand, muonID, protoCL, particle);
      if( sc.isFailure() ) {
        delete particle;
      }
      else {  
        // Insert particle in particle vector.
        parts.push_back(particle);
        nMuons++;
      }
      
    }
  }
  
  log << MSG::DEBUG << "Number of muons created from protoparticles: " 
      << nMuons << endreq;
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode MuonParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                            int protoID, double protoCL,
                                            Particle* particle ) {
  
  MsgStream  log( msgSvc(), name() );
  
  log << MSG::VERBOSE << " Starting to fill a particle ..." << endreq;
  
  // Start filling the particle:
  particle->setParticleID( ParticleID(protoID) );
  particle->setConfLevel( protoCL );
  //Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  particle->setMassErr(0.0); // For the moment but already in constructor
  particle->setIsResonance(false);  // Already in constructor
  
  const TrStateP* trackState = protoCand->trStateP();
  if( !trackState ) return StatusCode::FAILURE;
  
  // Set position of first measured point on track:
  HepPoint3D position( trackState->x(), trackState->y(), trackState->z() ) ;
  particle->setPointOnTrack( position );
//    log << MSG::DEBUG << "track (x,y,z) = "
//        << trackState->x() << ", " << trackState->y() << ", " 
//        << trackState->z() << endreq;
//    log << MSG::DEBUG << "position = " << position << endreq;
  
  // Calculate and set four momentum: do this in ProtoParticle... 
  double momentum = trackState->p();
  double slopeX   = trackState->tx();
  double slopeY   = trackState->ty();	  
  double pZ = momentum/sqrt( 1.0 + slopeX*slopeX + slopeY*slopeY );
  HepLorentzVector quadriMomentum;
  quadriMomentum.setPx( pZ*slopeX );
  quadriMomentum.setPy( pZ*slopeY );
  quadriMomentum.setPz( pZ );
  quadriMomentum.setE( sqrt( mass*mass + momentum*momentum) );
  particle->setMomentum( quadriMomentum );
//    log << MSG::DEBUG << "track (sx,sy,p) = "
//        << trackState->tx() << ", " << trackState->ty() << ", " 
//        << trackState->p() << endreq;
//    log << MSG::DEBUG << "momentum = " << quadriMomentum << endreq;
  
  // Retrieve track state covariance matrix and set particle error matrices:
  const HepSymMatrix& trkCov = trackState->pCovMatrix();
  
  // Set pointOnTrackErr: (Error on x and y. No error on z!)
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr = trkCov.sub(1,3);
  pointOnTrackErr(3,1) = 0.0;
  pointOnTrackErr(3,2) = 0.0;
  pointOnTrackErr(3,3) = 0.0;
  particle->setPointOnTrackErr(pointOnTrackErr);
  
  // Set slope+Momentum error:
  HepSymMatrix slpMomErr(3, 0);
  slpMomErr = trkCov.sub(3,5);
  particle->setSlopesMomErr(slpMomErr);
  
  // Set position-slopes correlation matrix. 
  // Position X Momentum correlation matrix also automatically set.
  // No correlation with Z
  HepMatrix posSlopesCorr(3, 3, 0);
  int i, j;
  for( i = 1; i <= 3; i++ ) {
    for ( j = 1; j <= 2; j++ ) {
      int istart = i+2;
      posSlopesCorr(i,j) = trkCov(istart,j);
    }
  }
  particle->setPosSlopesCorr(posSlopesCorr);
  
  // Print out informations 
//    log << MSG::DEBUG << "ProtoParticle error matrix" 
//        << trkCov << endreq;
  
//    log << MSG::DEBUG << "pointOnTrackErr" << particle->pointOnTrackErr() 
//        << endreq;
  
//    log << MSG::DEBUG << "slopesMomErr" << particle->slopesMomErr() 
//        << endreq;
  
//    log << MSG::DEBUG << "correlation" << particle->posSlopesCorr() 
//        << endreq;
  
//    log << MSG::DEBUG << "momentumErr" << particle->momentumErr()
//        << endreq;
  
  log << MSG::DEBUG << "correlation" << particle->posMomCorr()
      << endreq;

  particle->setOrigin(protoCand);

  return StatusCode::SUCCESS;
} 

//==========================================================================
//  Finalize
//==========================================================================
StatusCode MuonParticleMaker::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "MuonParticleMaker::finalizing" << endreq;
  
  return StatusCode::SUCCESS;
}
//=========================================================================
