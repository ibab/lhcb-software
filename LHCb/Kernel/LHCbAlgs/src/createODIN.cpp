// Include files 

// from Gaudi
#include "GaudiKernel/IEventTimeDecoder.h"

// local
#include "createODIN.h"

//-----------------------------------------------------------------------------
// Implementation file for class : createODIN
//
// 2008-01-15 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( createODIN )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
createODIN::createODIN( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_decoder(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
createODIN::~createODIN() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode createODIN::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_decoder = tool<IEventTimeDecoder>( "OdinTimeDecoder" );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode createODIN::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  // OdinTimeDecoder internally calls getODIN() when getTime() is invoked
  m_decoder->getTime();
  
  return StatusCode::SUCCESS;
}
//=============================================================================
