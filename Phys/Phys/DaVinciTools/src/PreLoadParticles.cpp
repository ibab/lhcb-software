// $Id: PreLoadParticles.cpp,v 1.1 2002-10-13 21:24:16 gcorti Exp $
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
  : Algorithm ( name , pSvcLocator ) {
  
}

//=============================================================================
// Destructor
//=============================================================================
PreLoadParticles::~PreLoadParticles() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PreLoadParticles::initialize() {

  MsgStream log(msgSvc(), name());

  // Retrieve PhysDesktop
  StatusCode sc = toolSvc()->retrieveTool("PhysDesktop", m_pDesktop, this);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "PhysDesktop not found" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PreLoadParticles::execute() {

  MsgStream  log( msgSvc(), name() );

  // Produce particles via desktop configuration
  StatusCode scDesktop = desktop()->getInput();
  if( !scDesktop.isSuccess() ) {
    log << MSG::ERROR << "Not able to fill desktop " << endreq;
    return StatusCode::FAILURE;
  }
  
  // Save all of them
  scDesktop = desktop()->saveDesktop();
  if( !scDesktop.isSuccess() ) {
    log << MSG::ERROR << "Not able to save desktop " << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PreLoadParticles::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// desktop
//=============================================================================
IPhysDesktop* PreLoadParticles::desktop() const {
  return m_pDesktop;
}

//=============================================================================
