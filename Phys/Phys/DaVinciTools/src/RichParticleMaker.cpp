// $Id: RichParticleMaker.cpp,v 1.2 2003-08-05 17:25:19 gcorti Exp $
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
#include "RichParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichParticleMaker
//
// Description: This tool fills the particle class with information from
// ProtoParticles and places it in the Transient Event Store.
// Make only pions and kaons (according to request) setting all light
// particles to be pion and all heavy ones to be kaons
// 12/09/2002 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichParticleMaker>          s_factory ;
const        IToolFactory& RichParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichParticleMaker::RichParticleMaker( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent ), m_ppSvc(0), m_EDS(0){
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Declare properties
  declareProperty( "InputProtoP", m_input = ProtoParticleLocation::Charged);
  
}
//=============================================================================
// Destructor
//=============================================================================
RichParticleMaker::~RichParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode RichParticleMaker::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> RichParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }  
  

  // Retrieve the PDG Code of required particles: electron, pion, muon 
  ParticleProperty* partProp = m_ppSvc->find("e-");
  if( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for electrons \"" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_eID = abs(partProp->jetsetID());

  partProp = m_ppSvc->find("mu-");
  if( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for muons \"" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_muID = abs(partProp->jetsetID());

  partProp = m_ppSvc->find("pi+");
  if( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for pions \"" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_piID = abs(partProp->jetsetID());

  partProp = m_ppSvc->find("K+");
  if( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for kaons \"" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_kID = abs(partProp->jetsetID());

  partProp = m_ppSvc->find("p+");
  if( 0 == partProp )   {
    log << MSG::ERROR << "Cannot retrieve properties for protons \"" 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_pID = abs(partProp->jetsetID());
  
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* RichParticleMaker::eventSvc() const
{
  return m_EDS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode RichParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  log( msgSvc(), name() );
  
  log << MSG::DEBUG << "RichParticleMaker::makeParticles() is running." 
      << endreq;
  
  int protoID;
  double protoCL;
  
  // make charged particles:
  int nChargedParticles=0;   // Counter of charged particles created.

  SmartDataPtr<ProtoParticles> candidates ( eventSvc(),m_input);
//    if ( !candidates ) {
//      log << MSG::ERROR << "Charged ProtoParticles do not exist in"  
//          << m_input << endreq;
//      return StatusCode::FAILURE;
//    }
  if( ( !candidates ) || ( 0 == candidates->size() ) ) { 
    log << MSG::INFO << "No Charged ProtoParticles retrieved from"  
        << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  
  // Log number of ProtoPartCandidates retrieved
  log << MSG::INFO << "    Number of Charged ProtoParticles retrieved   = "
      << candidates->size() << endreq;


  log << MSG::INFO << "Making all particles as pions or kaons" << endreq;
  // Loop over all Protoparticles and fill Particle using bestPID :
      
  for(ProtoParticles::const_iterator icand = candidates->begin();
      icand != candidates->end(); icand++){
    protoID=(*icand)->bestPID();
    if( protoID == 0 ) continue;
    // Consider muon Best and electron Best as pions
    if( ( abs(protoID) == m_eID ) || ( abs(protoID) == m_muID ) ){
      protoID = ((int)(*icand)->charge())*m_piID;
    }
    // Consider proton Best as Kaons
    if( abs(protoID) == m_pID ) {
      protoID = ((int)(*icand)->charge())*m_kID;
    }        
    protoCL=(*icand)->probOfBestPID();
    Particle* particle = new Particle();
    StatusCode sc = fillParticle( *icand, protoID, protoCL, particle);
    if( sc.isFailure() ) {
      delete particle;
    }
    else {  
      // Insert particle in particle vector.
      parts.push_back(particle);
      nChargedParticles++;
    }
  }

  return StatusCode::SUCCESS;
  
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode RichParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                            int protoID, double protoCL,
                                            Particle* particle ) {
  
  MsgStream  log( msgSvc(), name() );

  // Start filling the particle:     
  particle->setParticleID( ParticleID(protoID) );
  particle->setConfLevel( protoCL );

  // Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  
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
  
//    log << MSG::DEBUG << "correlation" << particle->posMomCorr()
//        << endreq;
  
  particle->setOrigin(protoCand);
  return StatusCode::SUCCESS;
} 
  
//==========================================================================
//  Finalize
//==========================================================================
StatusCode RichParticleMaker::finalize() {
    
//    MsgStream log(msgSvc(), name());
  
  return StatusCode::SUCCESS;
}
//=========================================================================
