// $Id: AxParticleMaker.cpp,v 1.2 2002-05-17 11:41:51 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "Event/Particle.h"
#include "Event/EventHeader.h"
#include "Event/AxPartCandidate.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "AxParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AxParticleMaker
//
// Description: This tool fills the particle class with information from
// AxParticleCandidate and places it in the Transient Event Store
// 25/02/2002 : Sandra Amato: FillParticleAlgorithm
// 22/04/2002 : Jose Helder Lopes: Convert to ParticleMaker Tool.
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<AxParticleMaker>          s_factory ;
const        IToolFactory& AxParticleMakerFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AxParticleMaker::AxParticleMaker( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_ppSvc(0)
  , m_EDS(0) {

  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Clean data members
  m_particleNames.clear();
  m_ids.clear();
  m_confLevels.clear();
  
  // Declare properties
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "ConfLevelCuts", m_confLevels );
  declareProperty( "InputLocation", 
                   m_inputLocation = AxPartCandidateLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
AxParticleMaker::~AxParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode AxParticleMaker::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> AxParticleMaker:Initialising" << endreq;
  
  StatusCode sc = StatusCode::FAILURE;;
  
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
  
  if (m_particleNames.size() == 0) {
    log << MSG::ERROR << " ParticleNames is empty "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }

  if( m_particleNames[0] != "all" ){  
    // Retrieve the PDG Code of required particles 
    m_ids.clear();
    
    std::vector<std::string>::const_iterator ipartsName;
    for ( ipartsName = m_particleNames.begin(); 
          ipartsName != m_particleNames.end(); ipartsName++ ) {
      ParticleProperty* partProp = m_ppSvc->find( *ipartsName );
      if ( 0 == partProp )   {
        log << MSG::ERROR << "Cannot retrieve properties for particle \"" 
            << *ipartsName << "\" " << endreq;
        return StatusCode::FAILURE;
      }
      m_ids.push_back( partProp->jetsetID() );
    }
    
    // Test the Confidence Level Cuts Vector 
    if (m_confLevels.size() == 0) {
      log << MSG::ERROR << "PhysDesktopConfLevelCuts is empty " 
          << "Please, initialize it in your job options file " << endreq;
      return StatusCode::FAILURE;
    }
    
    // Test if the the Confidence Level Cuts Vector and ParticleNames 
    // have the same size
    if (m_confLevels.size() != m_particleNames.size() ) {
      log << MSG::ERROR << "PhysDesktopConfLevelCuts size is  " 
          << "different from  PhysDesktopParticleNames" << endreq;
      return StatusCode::FAILURE;
    }
   
    // Print out the conditions
    std::vector<int>::const_iterator ipartsID;
    int icount = 0;
    for ( ipartsID = m_ids.begin(); ipartsID != m_ids.end(); ipartsID++ ) {
      log << MSG::DEBUG 
          << " Particle ID Requested " << (*ipartsID) 
          << " , with CL >= " << m_confLevels[icount]
          << endreq;
      icount++;
    }
    
  }  //if( m_particleNames[0] != "all" )
  return StatusCode::SUCCESS;
  
};


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* AxParticleMaker::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Pointer to the ParticleProperty service
//=============================================================================
IParticlePropertySvc* AxParticleMaker::ppSvc() const
{
  return m_ppSvc;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode AxParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  log( msgSvc(), name() );
  
  log << MSG::DEBUG << "==> AxParticleMaker::makeParticles() running." 
      << endreq;
  
  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );
  
  if ( evt ) {   
    log << MSG::DEBUG << "    retrieved EVENT: " << evt->evtNum()
        << " RUN: " << evt->runNum() << endreq;
  }
  else {
    log << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
  // Particles* particles = new Particles();
  
  
  // Retrieve AxPartCandidates
  SmartDataPtr<AxPartCandidates> candidates (eventSvc(),m_inputLocation);
  if ( ! candidates || ( candidates->size() == 0) ) { 
    log << MSG::INFO << "    No AxPartCandidates retrieved" << endreq;
  }
  else {
    
    // Log number of AxPartCandidates retrieved
    log << MSG::INFO << "    Number of AxPartCandidates retrieved   = "
        << candidates->size() << endreq;
    
    // Loop over all Axparticles and fill Particle if  Axparticle 
    // satisfy the selection criteria
    AxPartCandidates::iterator icand = 0;
    std::vector<int>::iterator ipartsID = 0;
    
    for( icand = candidates->begin(); icand != candidates->end(); icand++ ) {
      if ( ((*icand)->charge() != 0 && (*icand)->trkQuality() == 1 )
           || ((*icand)->charge() == 0) ){
        
        bool goodAxCand = false;
        if ( m_particleNames[0] == "all" ){
          goodAxCand = true;
        }
        else{
          int count=0;
          for ( ipartsID = m_ids.begin(); ipartsID != m_ids.end(); 
                ipartsID++ ) {
            count++;
            
            if ( (*icand)->particleID().pid() == (*ipartsID) &&
                 (*icand)->pIDConfidenceLevel()>= m_confLevels[count-1]) {
              goodAxCand = true;
            }
          }
        }          
        
        if( goodAxCand ){
          Particle* particle = new Particle();
          particle->setParticleID((*icand)->particleID());
          particle->setConfLevel((*icand)->pIDConfidenceLevel());
          //Get mass from Particle Property Service to calculate the 4-momentum
          ParticleProperty* partProp = 
            ppSvc()->findByStdHepID((*icand)->particleID().pid());      
          double mass = (*partProp).mass();
          particle->setMomentum((*icand)->fourMomentum(mass));
          particle->setPointOnTrack((*icand)->firstPoint());
          particle->setMass(mass);
          particle->setMassErr(0.0);

          // Now set the error matrices: point on track
          const HepSymMatrix& trkCov = (*icand)->fitCovariance();
          HepSymMatrix pointOnTrackErr = trkCov.sub(1,3);
          particle->setPointOnTrackErr(pointOnTrackErr);

          // point/slopes correlation terms
          HepMatrix posSlpMomCorrelationErr(3, 3, 0.0);
          int i, j;
          for( i = 1; i <= 3; i++ ) {
            for ( j = 1; j <= 3; j++ ) {
              int istart = i+3;
              posSlpMomCorrelationErr(i,j) = trkCov(istart,j);
            }
          }
          particle->setPosSlopesCorr(posSlpMomCorrelationErr);
          
          // on slopes and |P|
          HepSymMatrix slpMomCorrelationErr = trkCov.sub(4,6);
          particle->setSlopesMomErr(slpMomCorrelationErr);
          
          // check
          log << MSG::DEBUG << "AxPartCand error matrix" << endreq;
          for( i = 1; i <= trkCov.num_row(); i++ ) {
            for ( j = 1; j <= i; j++ ) {          
              log << MSG::DEBUG 
                  << "(" << i << "," << j << ") = " << trkCov(i, j) << endreq;
            }
          }
          
          log << MSG::DEBUG << "pointOnTrackErr" << endreq;
          for( i = 1; i <= pointOnTrackErr.num_row(); i++ ) {
            for ( j = 1; j <= i; j++ ) {          
              log << MSG::DEBUG 
                  << "(" << i << "," << j << ") = " << pointOnTrackErr(i, j) 
                  << endreq;
            }
          }
          
          log << MSG::DEBUG << "slopesMomErr" << endreq;
          for( i = 1; i <= 3; i++ ) {
            for ( j = 1; j <= i; j++ ) {          
              log << MSG::DEBUG  
                  << "(" << i << "," << j << ") = "<<slpMomCorrelationErr(i, j)
                  << endreq;
            }
          }
          
          log << MSG::DEBUG << "correlation" << endreq;
          for( i = 1; i <= posSlpMomCorrelationErr.num_row(); i++ ) {
            for ( j = 1; j <= posSlpMomCorrelationErr.num_col(); j++ ) {      
              log << MSG::DEBUG 
                  << "(" << i << "," << j << ") = " 
                  << posSlpMomCorrelationErr(i, j) 
                  << endreq;
            }
          }

          // Set pointer to Originating object
          particle->setOrigin((*icand));
          
          // Insert particle in particle vector.
          // particles->insert(particle);
          parts.push_back(particle);
        }
      }
    }
  }

  log << MSG::DEBUG 
      << "AxParticleMaker: Number of particles made for AxPartCand:"
      << parts.size() << endreq;
    //<< particles->size() << endreq;
  
  // Now fill the parts vector of pointers from makeParticles caller.
  //Particles::iterator icand = 0;
  //for( icand = particles->begin(); icand != particles->end(); icand++ ){
  //  parts.push_back(*icand);
  // }
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode AxParticleMaker::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> AxParticleMaker::finalize" << endreq;
  
  return StatusCode::SUCCESS;
}


//=============================================================================

