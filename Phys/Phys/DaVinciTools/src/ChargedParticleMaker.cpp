// $Id: ChargedParticleMaker.cpp,v 1.5 2003-06-18 12:21:51 gcorti Exp $
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
#include "ChargedParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedParticleMaker
//
// Description: This tool fills the particle class with information from
// ProtoParticles and places it in the Transient Event Store
// 12/06/2002 : Jose Helder Lopes.
// 18/07/2002 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ChargedParticleMaker>          s_factory ;
const        IToolFactory& ChargedParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedParticleMaker::ChargedParticleMaker( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent ), m_ppSvc(0), m_EDS(0){
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Clean data members
  m_particleNames.clear();
  m_ids.clear();
  m_confLevels.clear();
  
  // Declare properties
  declareProperty( "InputProtoP", m_input = ProtoParticleLocation::Charged);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "ConfLevelCuts", m_confLevels );
  
}
//=============================================================================
// Destructor
//=============================================================================
ChargedParticleMaker::~ChargedParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ChargedParticleMaker::initialize() {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> ChargedParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  msg << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }  
  
  // Test the ParticleNames Vector 
  if (m_particleNames.size() == 0) {
    msg << MSG::ERROR << " ParticleNames is empty. "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }
  
  if(m_particleNames[0] != "All" ){
    if(m_particleNames[0] == "AllCompatible" ){ 
      // Test the Confidence Level Cuts Vector 
      if (m_confLevels.size() == 0) {
        msg << MSG::ERROR << "PhysDesktopConfLevelCuts is empty. " 
            << "Option AllCompatible requires this to be set. "
            << "Please, initialize it in your job options file " << endreq;
        return StatusCode::FAILURE;
      }
      
    }
    else {  
      // Test the Confidence Level Cuts Vector 
      if (m_confLevels.size() == 0) {
        msg << MSG::ERROR << "PhysDesktopConfLevelCuts is empty. " 
            << "Please, initialize it in your job options file " << endreq;
        return StatusCode::FAILURE;
      }
      
      // Test if the the Confidence Level Cuts Vector and ParticleNames 
      // have the same size
      if (m_confLevels.size() != m_particleNames.size() ) {
        msg << MSG::ERROR << "PhysDesktopConfLevelCuts size is  " 
            << "different from  PhysDesktopParticleNames" << endreq;
        return StatusCode::FAILURE;
      }
      
      // Everything is OK. So,
      // Retrieve the PDG Code of required particles, 
      // separating charged from neutrals. 
      std::vector<std::string>::const_iterator ipartsName;
      std::vector<double>::const_iterator iConfLevel=m_confLevels.begin();
      for ( ipartsName = m_particleNames.begin(); 
            ipartsName != m_particleNames.end(); ++ipartsName ) {
        ParticleProperty* partProp = m_ppSvc->find( *ipartsName );
        if ( 0 == partProp )   {
          msg << MSG::ERROR << "Cannot retrieve properties for particle \"" 
              << *ipartsName << "\" " << endreq;
          return StatusCode::FAILURE;
        }
        if( partProp->charge() != 0 ){
          std::pair<int,double> id(partProp->jetsetID(),(*iConfLevel++));
          m_ids.push_back(id);
        }
        //else{
        //  m_idsNeutral.push_back( std::pair::makePair(partProp->jetsetID(),
        //                                                (*iConfLevel++)) );
        //}
        // Print Debug message:
        msg << MSG::DEBUG << " Particle Requested: Name = " << (*ipartsName) 
            << " PID = " << partProp->jetsetID() << endreq;
      }
    } // else of "if(m_particleNames[0] != "AllCompatible" )"
  }  //if(m_particleNames[0] != "All")
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* ChargedParticleMaker::eventSvc() const
{
  return m_EDS;
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  msg( msgSvc(), name() );
  
  msg << MSG::DEBUG << "==> ChargedParticleMaker::makeParticles() is running." 
      << endreq;
  
  int protoID;
  double protoCL;
  
  // make charged particles:
  int nChargedParticles=0;   // Counter of charged particles created.
  
  SmartDataPtr<ProtoParticles> candidates ( eventSvc(),m_input);
  if ( !candidates || (0 == candidates->size()) ) { 
    msg << MSG::DEBUG << "    No Charged ProtoParticles retrieved from"  
        << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Msg number of ProtoPartCandidates retrieved
  msg << MSG::DEBUG << "    Number of Charged ProtoParticles retrieved   = "
      << candidates->size() << endreq;
  
  // Make all parrticles using bestPID
  if( "All" == m_particleNames[0] ){
    
    msg << MSG::DEBUG << "Making all particles " << endreq;
    // Loop over all Protoparticles and fill Particle using bestPID :
    
    ProtoParticles::const_iterator icand = 0;  // Iterator on ProtoParticles.
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      protoID=(*icand)->bestPID();
      if( protoID == 0 ) continue;
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
  
  // Make all particles compatible with a given PID for the required 
  // conf. level cut
  if( "AllCompatible" == m_particleNames[0] ){
    
    msg << MSG::DEBUG << "Making all compatible particles " << endreq;
    // Loop over all Protoparticles. For each one, create Particle 
    // with the PID's for which the conf. level is above requested cut
    
    ProtoParticles::const_iterator icand = 0;  // Iterator on ProtoParticles.
    for(icand = candidates->begin(); icand != candidates->end(); icand++){
      int partPerProto=0;
      ProtoParticle::PIDInfoVector::iterator iProtoID;
      for (iProtoID=(*icand)->pIDInfo().begin();
           iProtoID!=(*icand)->pIDInfo().end(); ++iProtoID){
        protoID = (*iProtoID).first;
        protoCL = (*iProtoID).second;
        
        if ( protoCL >= m_confLevels[0]  ){ 
          // We have a candidate compatible with this PID:
          msg << MSG::DEBUG << icand << " ProtoID = " << protoID 
              << " protoCL = " << protoCL << endreq;
          
          partPerProto++;
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
      }
      msg << MSG::DEBUG << "Made " << partPerProto 
          << " particles with one protoparticle"  << endreq;
    }
    return StatusCode::SUCCESS;
  }
  
  // Make only the requested particles with the corresponding conf. level cut  
  
  msg << MSG::DEBUG << "Making only the requested particles " << endreq;
  
  if( m_ids.size() !=0 ) {
    
    // Loop over all ProtoParticles and fill Particle if the ProtoParticle 
    // satisfies the PID and corresponding CL cut  :
    ProtoParticles::iterator icand = 0;  // Iterator on ProtoParticles.
    for(icand = candidates->begin();icand != candidates->end();icand++){  
      // Iterator on requested PID's
      std::vector<std::pair<int,double> >::iterator iWantedPID; 
      ProtoParticle::PIDInfoVector::iterator iProtoID;
      for ( iWantedPID = m_ids.begin();
            iWantedPID != m_ids.end(); 
            ++iWantedPID ) {
        for (iProtoID=(*icand)->pIDInfo().begin();
             iProtoID!=(*icand)->pIDInfo().end(); ++iProtoID){
          protoID = (*iProtoID).first;
          protoCL = (*iProtoID).second;
          
          if ( protoID  == (*iWantedPID).first && 
               protoCL >= (*iWantedPID).second  ){ 
            // We have a good candidate to this iWantedPID:
            Particle* particle = new Particle();
            StatusCode sc = fillParticle(*icand, protoID, protoCL, particle);
            if( sc.isFailure() ) {
              delete particle;
            }
            else {  
              // Insert particle in particle vector.
              parts.push_back(particle);
              nChargedParticles++;
            }
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode ChargedParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                               int protoID, double protoCL,
                                               Particle* particle ) {
  
  MsgStream  msg( msgSvc(), name() );
  
  // Start filling the particle:     
  particle->setParticleID( protoID );
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
//    msg << MSG::DEBUG << "track (x,y,z) = "
//        << trackState->x() << ", " << trackState->y() << ", " 
//        << trackState->z() << endreq;
//    msg << MSG::DEBUG << "position = " << position << endreq;
  
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
//    msg << MSG::DEBUG << "track (sx,sy,p) = "
//        << trackState->tx() << ", " << trackState->ty() << ", " 
//        << trackState->p() << endreq;
//    msg << MSG::DEBUG << "momentum = " << quadriMomentum << endreq;
  
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
//    msg << MSG::DEBUG << "ProtoParticle error matrix" 
//        << trkCov << endreq;
  
//    msg << MSG::DEBUG << "pointOnTrackErr" << particle->pointOnTrackErr() 
//        << endreq;
  
//    msg << MSG::DEBUG << "slopesMomErr" << particle->slopesMomErr() 
//        << endreq;
  
//    msg << MSG::DEBUG << "correlation" << particle->posSlopesCorr() 
//        << endreq;
  
//    msg << MSG::DEBUG << "momentumErr" << particle->momentumErr()
//        << endreq;
  
//    msg << MSG::DEBUG << "correlation" << particle->posMomCorr()
//        << endreq;

  particle->setOrigin(protoCand);
  return StatusCode::SUCCESS;
} 

//==========================================================================
//  Finalize
//==========================================================================
StatusCode ChargedParticleMaker::finalize() {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> ChargedParticleMaker::finalizing" << endreq;
  
  return StatusCode::SUCCESS;
}
//=========================================================================
