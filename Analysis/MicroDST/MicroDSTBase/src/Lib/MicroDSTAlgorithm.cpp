// $Id: MicroDSTAlgorithm.cpp,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
// Include files 

// from Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/KeyedContainer.h>
// local
#include "MicroDST/MicroDSTAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MicroDSTAlgorithm
//
// 2007-10-11 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MicroDSTAlgorithm::MicroDSTAlgorithm( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : 
  MicroDSTCommon<GaudiAlgorithm> ( name , pSvcLocator )
{
  std::cout << "MicroDSTAlgorithm constructor for " << name << std::endl;
}
//=============================================================================
// Destructor
//=============================================================================
MicroDSTAlgorithm::~MicroDSTAlgorithm() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MicroDSTAlgorithm::initialize() {
  StatusCode sc = MicroDSTCommon<GaudiAlgorithm>::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode MicroDSTAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MicroDSTAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTCommon<GaudiAlgorithm>::finalize();  // must be called after all other actions
}
//=============================================================================
