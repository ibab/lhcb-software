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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixInputCopyStream::FixInputCopyStream( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_leavesTool   ( NULL )
{
}

//=============================================================================
// Destructor
//=============================================================================
FixInputCopyStream::~FixInputCopyStream() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FixInputCopyStream::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  

  return toolSvc()->retrieveTool("DataSvcFileEntriesTool", 
                                 "InputCopyStreamTool",
                                 m_leavesTool);
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FixInputCopyStream::execute() 
{
  m_leavesTool->leaves();
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FixInputCopyStream::finalize() 
{
  toolSvc()->releaseTool(m_leavesTool).ignore();
  m_leavesTool = NULL;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FixInputCopyStream )
