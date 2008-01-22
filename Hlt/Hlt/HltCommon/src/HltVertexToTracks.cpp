// $Id: HltVertexToTracks.cpp,v 1.2 2008-01-22 09:56:44 hernando Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltVertexToTracks.h"
#include "HltBase/ESequences.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexToTracks
//
// 2007-08-03 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexToTracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexToTracks::HltVertexToTracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  m_doInitSelections = false;
  m_algoType = "HltVertesToTracks";
}
//=============================================================================
// Destructor
//=============================================================================
HltVertexToTracks::~HltVertexToTracks() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltVertexToTracks::initialize() {
  debug() << "==> Initialize" << endmsg;
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first

  m_inputVertices = 
    &(retrieveTSelection<LHCb::RecVertex>(m_inputSelectionName));
  
  m_outputTracks = &(registerTSelection<LHCb::Track>(m_outputSelectionName));

  saveConfiguration();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltVertexToTracks::execute() {
  debug() << "==> Execute" << endmsg;

  for(  Hlt::VertexContainer::const_iterator it= m_inputVertices->begin(); 
        it<m_inputVertices->end(); ++it){
    
    SmartRefVector< LHCb::Track >  trks = (*it)->tracks();
    if(trks.size()!=2) return StatusCode::FAILURE;

    Track* track0 = (Track*) trks[0];
    Track* track1 = (Track*) trks[1];
    zen::extend(*m_outputTracks,track0);
    zen::extend(*m_outputTracks,track1);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltVertexToTracks::finalize() 
{
  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

