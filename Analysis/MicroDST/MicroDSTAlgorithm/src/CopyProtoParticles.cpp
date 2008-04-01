// $Id: CopyProtoParticles.cpp,v 1.6 2008-04-01 14:43:52 jpalac Exp $
// Include files 

// from Gaudi
#include <GaudiKernel/AlgFactory.h> 
// from LHCb
#include <Event/ProtoParticle.h>
// from MicroDST
#include <MicroDST/ICloneProtoParticle.h>
// local
#include "CopyProtoParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyProtoParticles
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyProtoParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyProtoParticles::CopyProtoParticles( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : 
  MicroDSTAlgorithm ( name , pSvcLocator ),
  m_cloner(0),
  m_clonerName("ProtoParticleCloner")
{
  declareProperty( "ICloneProtoParticle", m_clonerName );
}
//=============================================================================
// Destructor
//=============================================================================
CopyProtoParticles::~CopyProtoParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyProtoParticles::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_cloner = tool<ICloneProtoParticle>(m_clonerName, this);

  if (m_cloner) {
    debug() << "Found ICloneProtoParticle" << endmsg;
  } else {
    error() << "Failed to find IClonerProtoParticle" << endmsg;
  }

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyProtoParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store ProtoParticles starting from " 
            << inputTESLocation() << endmsg;

  const ProtoParticles* pp = 
    copyKeyedContainer<ProtoParticles, ICloneProtoParticle>(inputTESLocation(),
                                                            m_cloner     );
  
  setFilterPassed(true);
  
  return (0!=pp) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyProtoParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
