// $Id: FillParticleAlgorithm.cpp,v 1.1 2002-03-27 20:34:59 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "Event/Particle.h"
#include "LHCbEvent/Event.h"
#include "LHCbEvent/AxPartCandidate.h"
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
  SmartDataPtr<Event> evt(eventSvc(), "/Event" );

  if ( evt ) {   
    log << MSG::INFO << "    retrieved EVENT: " << evt->event()
        << " RUN: " << evt->run() << endreq;
  }
  else {
    log << MSG::ERROR << "    not able to retrieve event" << endreq;
    return StatusCode::FAILURE;
  }
  
  
  Particles* particles = new Particles();
  
  
  // Retrieve AxPartCandidates
  SmartDataPtr<AxPartCandidateVector> 
               candidates (eventSvc(),"/Event/Anal/AxPartCandidates");
  if ( ! candidates ) { 
    log << MSG::INFO << "    No AxPartCandidates retrieved" << endreq;
  }
  else {
    
    // Log number of AxPartCandidates retrieved
    log << MSG::INFO << "    Number of AxPartCandidates retrieved   = "
        << candidates->size() << endreq;

    // Loop over all particles and fill Particle class
    AxPartCandidateVector::iterator icand = 0;

    for( icand = candidates->begin(); icand != candidates->end(); icand++ ) {

      
      Particle* particle = new Particle();
//      particle->setCharge((*icand)->charge());
      particle->setParticleID((*icand)->particleID());
      particle->setConfLevel((*icand)->pIDConfidenceLevel());
      //Get mass from Particle Property Service to calculate the 4-momentum
      ParticleProperty* partProp = 
             m_ppSvc->findByStdHepID((*icand)->particleID().id());      
      double mass = (*partProp).mass();
      particle->setMomentum((*icand)->fourMomentum(mass));
      particle->setPointOnTrack((*icand)->firstPoint());
      particle->setMass(mass);
      particle->setMassErr(0.0);
      // The following already has false as default value     
      // particle->setIsResonance(false);

      // The following are calculated on the fly using the Px/Py/Pz
      // the setmethods do not exist
      // particle->setFitSlopeX((*icand)->fitSlopeX());
      // particle->setFitSlopeY((*icand)->fitSlopeY());

//    log << MSG::DEBUG << "    Slope X   = "
//        << particle->fitSlopeX() << endreq;

      const HepSymMatrix& trkCov = (*icand)->fitCovariance();
      HepSymMatrix pointOnTrackErr = trkCov.sub(1,3);
      particle->setPointOnTrackErr(pointOnTrackErr);

      // m_pointOnTrack violates coding conventions
      // All of the following can be replaced with the single line above
      //m_pointOnTrackErr(1, 1) = trkCov(1, 1);
      //m_pointOnTrackErr(2, 1) = trkCov(2, 1);
      //m_pointOnTrackErr(2, 2) = trkCov(2, 2);
      //m_pointOnTrackErr(3, 1) = trkCov(3, 1);
      //m_pointOnTrackErr(3, 2) = trkCov(3, 2);
      //m_pointOnTrackErr(3, 3) = trkCov(3, 3);

      // The follwoing is already done at creation
      //HepSymMatrix m_momentumErr(4, 0.0);
      //particle->setMomentumErr(m_momentumErr);
      
      //HepMatrix m_correlationErr(3, 3, 0.0);
      //particle->setPosMomCorr(m_correlationErr);


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

      //m_posSlpMomCorrelationErr(1, 1) = trkCov(4, 1);
      //m_posSlpMomCorrelationErr(1, 2) = trkCov(4, 2);
      //m_posSlpMomCorrelationErr(1, 3) = trkCov(4, 3);
      //m_posSlpMomCorrelationErr(2, 1) = trkCov(5, 1);
      //m_posSlpMomCorrelationErr(2, 2) = trkCov(5, 2);
      //m_posSlpMomCorrelationErr(2, 3) = trkCov(5, 3);
      //m_posSlpMomCorrelationErr(3, 1) = trkCov(6, 1);
      //m_posSlpMomCorrelationErr(3, 2) = trkCov(6, 2);
      //m_posSlpMomCorrelationErr(3, 3) = trkCov(6, 3);

      // as for point on track
      HepSymMatrix slpMomCorrelationErr = trkCov.sub(4,6);
      particle->setSlopesMomErr(slpMomCorrelationErr);
      //m_slpMomCorrelationErr(1, 1) = trkCov(4, 4);
      //m_slpMomCorrelationErr(2, 1) = trkCov(5, 4);
      //m_slpMomCorrelationErr(2, 2) = trkCov(5, 5);
      //m_slpMomCorrelationErr(3, 1) = trkCov(6, 4);
      //m_slpMomCorrelationErr(3, 2) = trkCov(6, 5);
      //m_slpMomCorrelationErr(3, 3) = trkCov(6, 6);

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

//    log << MSG::DEBUG << "    Particle ID   = "
//        << particle->particleID().id() << endreq;
      
    particles->insert(particle);
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
