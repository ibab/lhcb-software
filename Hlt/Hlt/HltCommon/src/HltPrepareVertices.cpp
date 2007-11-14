// $Id: HltPrepareVertices.cpp,v 1.2 2007-11-14 13:57:03 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltPrepareVertices.h"
#include "HltBase/HltSequences.h"
#include "HltBase/HltFunctions.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltPrepareVertices
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltPrepareVertices );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltPrepareVertices::HltPrepareVertices( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("TESInputVerticesName", m_TESInputVerticesName = "");
}
//=============================================================================
// Destructor
//=============================================================================
HltPrepareVertices::~HltPrepareVertices() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltPrepareVertices::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  checkInput(m_outputVertices," output vertices ");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltPrepareVertices::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  RecVertices* ivertices = get<RecVertices>(m_TESInputVerticesName);
  
  Hlt::copy(*ivertices,*m_outputVertices);  
  
  int ncan = m_outputVertices->size();
  candidateFound(ncan);
  
  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltPrepareVertices::finalize() {

  debug() << "==> Finalize" << endmsg;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
