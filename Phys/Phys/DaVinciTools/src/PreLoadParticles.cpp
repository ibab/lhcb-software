// $Id: PreLoadParticles.cpp,v 1.4 2004-07-16 16:12:12 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"


// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "PreLoadParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PreLoadParticles
//
// 2002-08-22 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PreLoadParticles>          s_factory ;
const        IAlgFactory& PreLoadParticlesFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PreLoadParticles::PreLoadParticles( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) {
  setProperty( "AvoidSelResult", "true");
  setProperty( "DecayDescriptor", "\"null\"");
}

//=============================================================================
// Destructor
//=============================================================================
PreLoadParticles::~PreLoadParticles() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PreLoadParticles::initialize() {

  debug() << "==> Initialize" << endreq;
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PreLoadParticles::execute() {

  MsgStream  msg( msgSvc(), name() );

  // Save desktop
  StatusCode scDesktop = desktop()->saveDesktop();
  if( !scDesktop.isSuccess() ) {
    msg << MSG::ERROR << "Not able to save desktop " << endreq;
    return StatusCode::FAILURE;
  }
  
  // Log number of vertices and particles
  msg << MSG::DEBUG << "Number of particles in desktop = " << 
    desktop()->particles().size() << endreq;
  msg << MSG::DEBUG << "Number of vertices in desktop = " << 
      desktop()->vertices().size() << endreq;

  setFilterPassed(true);
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PreLoadParticles::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

