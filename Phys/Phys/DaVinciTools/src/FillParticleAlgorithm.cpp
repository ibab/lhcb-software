// $Id: FillParticleAlgorithm.cpp,v 1.3 2002-05-15 23:22:20 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "Event/Particle.h"
#include "Event/EventHeader.h"
#include "Event/AxPartCandidate.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "FillParticleAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FillParticleAlgorithm
//
// Description: This algorithm fills the particle class with information from
// AxParticleCandidate and places it in the Transient Event Store
// 25/02/2002 : Sandra Amato
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FillParticleAlgorithm>          s_factory ;
const        IAlgFactory& FillParticleAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FillParticleAlgorithm::FillParticleAlgorithm( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {
  m_ppSvc = 0;

}

//=============================================================================
// Destructor
//=============================================================================
FillParticleAlgorithm::~FillParticleAlgorithm() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FillParticleAlgorithm::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  log << MSG::INFO << "    Looking for Particle Property Service." << endreq;

  StatusCode sc;
  sc = service("ParticlePropertySvc", m_ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FillParticleAlgorithm::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Retrieve informations about event
  SmartDataPtr<EventHeader> evt(eventSvc(), EventHeaderLocation::Default );

  if ( evt ) {   
    log << MSG::INFO << "    retrieved EVENT: " << evt->evtNum()
        << " RUN: " << evt->runNum() << endreq;
  }
  else {
    log << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
  
  Particles* particles = new Particles();
  
  
  // Retrieve AxPartCandidates
  SmartDataPtr<AxPartCandidates> 
               candidates (eventSvc(),AxPartCandidateLocation::Default);
  if ( ! candidates ) { 
    log << MSG::INFO << "    No AxPartCandidates retrieved" << endreq;
  }
  else {
    
    // Log number of AxPartCandidates retrieved
    log << MSG::INFO << "    Number of AxPartCandidates retrieved   = "
        << candidates->size() << endreq;

    // Loop over all particles and fill Particle class
    AxPartCandidates::iterator icand = 0;

    for( icand = candidates->begin(); icand != candidates->end(); icand++ ) {

      if( ((*icand)->charge() != 0 && (*icand)->trkQuality() == 1 ) ||
          ((*icand)->charge() == 0) ){
      
        Particle* particle = new Particle();
        particle->setParticleID((*icand)->particleID());
        particle->setConfLevel((*icand)->pIDConfidenceLevel());
        //Get mass from Particle Property Service to calculate the 4-momentum
        ParticleProperty* partProp = 
          m_ppSvc->findByStdHepID((*icand)->particleID().pid());      
        double mass = (*partProp).mass();
        particle->setMomentum((*icand)->fourMomentum(mass));
        particle->setPointOnTrack((*icand)->firstPoint());
        particle->setMass(mass);
        particle->setMassErr(0.0);

        const HepSymMatrix& trkCov = (*icand)->fitCovariance();
        HepSymMatrix pointOnTrackErr = trkCov.sub(1,3);
        particle->setPointOnTrackErr(pointOnTrackErr);

        // This we need to do by hand...
        HepMatrix posSlpMomCorrelationErr(3, 3, 0.0);
        int i, j;
        for( i = 1; i <= 3; i++ ) {
          for ( j = 1; j <= 3; j++ ) {
            int istart = i+3;
            posSlpMomCorrelationErr(i,j) = trkCov(istart,j);
          }
        }
        particle->setPosSlopesCorr(posSlpMomCorrelationErr);

        // as for point on track
        HepSymMatrix slpMomCorrelationErr = trkCov.sub(4,6);
        particle->setSlopesMomErr(slpMomCorrelationErr);

        log << MSG::DEBUG << "AxPartCand error" << endreq;
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
                << "(" << i << "," << j << ") = " << slpMomCorrelationErr(i, j)
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
        particle->setOrigin((*icand));

        particles->insert(particle);
      }
    }
  }
  

  // Now register the container in the store
  StatusCode sc = 
    eventSvc()->registerObject(ParticleLocation::Production,particles);

  if ( sc.isFailure() ) {
    // Unsuccesfull: delete the allocated container
    delete particles;
    log << MSG::ERROR  
        << "    Unable to register the Particles in TES" << endreq;
    return StatusCode::FAILURE;
  }




  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FillParticleAlgorithm::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  m_ppSvc = 0;

  return StatusCode::SUCCESS;
}

//=============================================================================
