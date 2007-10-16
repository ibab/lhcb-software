// $Id: CopyPrimaryVertices.cpp,v 1.1 2007-10-16 14:07:30 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/RecVertex.h"
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
  : CopyAndStoreData ( name , pSvcLocator ),
    m_storeTracks(false),
    m_cloner( PVCloner(m_storeTracks) )
{
  declareProperty( "StoreTracks", m_storeTracks );
}
//=============================================================================
// Destructor
//=============================================================================
CopyPrimaryVertices::~CopyPrimaryVertices() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyPrimaryVertices::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    verbose() << "changing input TES location " << endmsg;
    setInputTESLocation(LHCb::RecVertexLocation::Primary);
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  containerCloner().cloner().storeTracks(storeTracks());

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyPrimaryVertices::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store Primary Vertex bank from " << inputTESLocation()
            << " into " << fullOutputTESLocation() << endmsg;

  return copyAndStoreContainer<Vertices, PVContainerCloner>(inputTESLocation(),
                                                            fullOutputTESLocation(),
                                                            containerCloner() );
  
  //  return copyAndStoreContainer<Vertices, PVContainerCloner>(inputTESLocation(),
    //                                                            cloner());
  // need to do something about the tracks...
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyPrimaryVertices::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
