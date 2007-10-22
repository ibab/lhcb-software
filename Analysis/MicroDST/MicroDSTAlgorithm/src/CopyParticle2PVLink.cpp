// $Id: CopyParticle2PVLink.cpp,v 1.1 2007-10-22 16:29:50 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
#include "Event/Particle.h"
// local
#include "CopyParticle2PVLink.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2PVLink
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2PVLink );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2PVLink::CopyParticle2PVLink( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : 
  CopyAndStoreData ( name , pSvcLocator ),
  m_inputParticleLocation(""),
  m_inputPVLocation(LHCb::RecVertexLocation::Primary)
{

  declareProperty("InputParticleLocation", m_inputParticleLocation);
  declareProperty("InputPVLocation", m_inputPVLocation);

}
//=============================================================================
// Destructor
//=============================================================================
CopyParticle2PVLink::~CopyParticle2PVLink() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2PVLink::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2PVLink::execute() {

  return StatusCode::SUCCESS;  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticle2PVLink::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
