// $Id: HltVertexToTracks.cpp,v 1.1 2007-11-20 12:48:46 hernando Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "HltVertexToTracks.h"

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
  return HltAlgorithm::initialize(); // must be executed first
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
    ELoop::extend(*m_outputTracks,track0);
    ELoop::extend(*m_outputTracks,track1);
  }

  if (m_debug)
    printInfo(" tracks from vertices ",*m_outputTracks);

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

