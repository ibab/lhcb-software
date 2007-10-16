// $Id: CopyParticles.cpp,v 1.1 2007-10-16 14:41:45 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// local
#include "CopyParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticles
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticles::CopyParticles( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : CopyAndStoreData ( name , pSvcLocator ),
    m_partCloner(ParticleCloner())
{
  //  declareProperty( "StoreTracks", m_storeTracks );
}
//=============================================================================
// Destructor
//=============================================================================
CopyParticles::~CopyParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "changing input TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  //  containerCloner().cloner().storeTracks(storeTracks());

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store Particles from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;

  return copyAndStoreContainer<Particles, ParticleContainerCloner>(inputTESLocation(),
                                                            fullOutputTESLocation(),
                                                            ParticleContainerCloner() );
  
  //  return copyAndStoreContainer<Vertices, PVContainerCloner>(inputTESLocation(),
    //                                                            cloner());
  // need to do something about the tracks...
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
