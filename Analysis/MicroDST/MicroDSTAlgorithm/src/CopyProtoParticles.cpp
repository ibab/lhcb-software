// $Id: CopyProtoParticles.cpp,v 1.1 2007-10-23 14:42:13 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
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
  : CopyAndStoreData ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
CopyProtoParticles::~CopyProtoParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyProtoParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

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

  return StatusCode::SUCCESS;

}
//=============================================================================
const LHCb::ProtoParticle* CopyProtoParticles::storeProtoParticle(const LHCb::Particle* particle)
{

  const LHCb::ProtoParticle* proto = particle->proto();

  if (proto ) {

    const std::string from = objectLocation(proto->parent());
    const std::string to = outputTESLocation(from);

    const LHCb::ProtoParticle* ppClone = 
      cloneKeyedContainerItem<LHCb::ProtoParticle, PPCloneFunctor>(particle->proto(), to );

    LHCb::Particle* particleClone = getStoredClone<LHCb::Particle>(particle);
    
    if (particleClone) particleClone->setProto(ppClone);

    return ppClone;

  } else {
    return 0;
  }
   
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyProtoParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
