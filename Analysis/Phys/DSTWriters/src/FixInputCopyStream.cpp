// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IDataStoreLeaves.h"
// local
#include "FixInputCopyStream.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixInputCopyStream
//
// 2011-01-10 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FixInputCopyStream );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixInputCopyStream::FixInputCopyStream( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
FixInputCopyStream::~FixInputCopyStream() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FixInputCopyStream::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return toolSvc()->retrieveTool("DataSvcFileEntriesTool", 
                                 "InputCopyStreamTool",
                                 m_leavesTool);

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FixInputCopyStream::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  m_leavesTool->leaves();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FixInputCopyStream::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  toolSvc()->releaseTool(m_leavesTool).ignore();
  m_leavesTool = 0;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
