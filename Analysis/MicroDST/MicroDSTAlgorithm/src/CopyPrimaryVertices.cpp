// $Id: CopyPrimaryVertices.cpp,v 1.6 2007-12-11 17:37:12 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
// from MicroDST
#include <MicroDST/ICloneRecVertex.h>
#include <MicroDST/MicroDSTTool.h>
// local
#include "CopyPrimaryVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyPrimaryVertices
//
// 2007-10-15 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyPrimaryVertices );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyPrimaryVertices::CopyPrimaryVertices( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator ),
    m_vertexCloner(0),
    m_vertexClonerName("RecVertexCloner")
{
  declareProperty("ICloneRecVertex", m_vertexClonerName);
}
//=============================================================================
// Destructor
//=============================================================================
CopyPrimaryVertices::~CopyPrimaryVertices() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyPrimaryVertices::initialize() {

  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::RecVertexLocation::Primary);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_vertexCloner = tool<ICloneRecVertex>(m_vertexClonerName, this);

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyPrimaryVertices::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store Primary Vertex bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;

   const Vertices* v = 
     copyKeyedContainer<Vertices, ICloneRecVertex>(inputTESLocation(),
                                                   m_vertexCloner      );
  
  setFilterPassed(true);

  return (0!=v) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyPrimaryVertices::finalize() {

  debug() << "==> Finalize" << endmsg;

  return MicroDSTAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
