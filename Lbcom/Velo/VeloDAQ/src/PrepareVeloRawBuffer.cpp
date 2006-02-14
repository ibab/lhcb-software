// $Id: PrepareVeloRawBuffer.cpp,v 1.5 2006-02-14 18:20:59 krinnert Exp $
// Include files 

#include "GaudiKernel/AlgFactory.h"

#include "PrepareVeloRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareVeloRawBuffer
//
// 2003-04-11 : Olivier Callot
// 2004-04-13 : modified and moved to Velo area Chris Parkes
// 2004-11-03 : modified to GaudiAlg dependance
// 2006-02-14 : David Jones, Kuer Rinnert: started complete rewrite
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrepareVeloRawBuffer>          Factory ;
const        IAlgFactory& PrepareVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareVeloRawBuffer::PrepareVeloRawBuffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
 
}

//=============================================================================
// Destructor
//=============================================================================
PrepareVeloRawBuffer::~PrepareVeloRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareVeloRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareVeloRawBuffer::execute() {

  debug() << "==> Execute" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Whatever needs to be done at the end
//=============================================================================
StatusCode PrepareVeloRawBuffer::finalize() {

  debug() << "==> Finalise" << endreq;

  return StatusCode::SUCCESS;
};

