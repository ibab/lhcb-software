// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "DecodeVeloRawBuffer.h"


//-----------------------------------------------------------------------------
// Implementation file for class : DecodeVeloRawBuffer
//
// 2004-02-02 : Chris Parkes
// 2006-02-14 : David Jones, Kurt Rinnert
//              - started complete rewrite
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<DecodeVeloRawBuffer>          Factory ;
const        IAlgFactory& DecodeVeloRawBufferFactory = Factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecodeVeloRawBuffer::DecodeVeloRawBuffer( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
{
  ;
}


//=============================================================================
// Destructor
//=============================================================================
DecodeVeloRawBuffer::~DecodeVeloRawBuffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode DecodeVeloRawBuffer::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug () << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecodeVeloRawBuffer::execute() {

  MsgStream  msg( msgSvc(), name() );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Whatever needs to be done at the end
//=============================================================================
StatusCode DecodeVeloRawBuffer::finalize() {

  debug () << "==> Finalise" << endreq;

  return StatusCode::SUCCESS;
};


