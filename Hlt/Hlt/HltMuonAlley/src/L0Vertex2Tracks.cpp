// $Id: L0Vertex2Tracks.cpp,v 1.2 2007-11-15 14:55:03 graven Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "L0Vertex2Tracks.h"

using namespace LHCb;
//=============================================================================
namespace {
template <typename Container>
bool contains(Container c, typename Container::value_type t)
{
  return std::find(c.begin(),c.end(),t) != c.end();
};
}

//-----------------------------------------------------------------------------
// Implementation file for class : L0Vertex2Tracks
//
// 2007-08-03 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0Vertex2Tracks );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0Vertex2Tracks::L0Vertex2Tracks( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
}
//=============================================================================
// Destructor
//=============================================================================
L0Vertex2Tracks::~L0Vertex2Tracks() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0Vertex2Tracks::initialize() {
  debug() << "==> Initialize" << endmsg;
  return HltAlgorithm::initialize(); // must be executed first
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0Vertex2Tracks::execute() {
  debug() << "==> Execute" << endmsg;

  for(  Hlt::VertexContainer::const_iterator L0dimuon= m_inputVertices->begin();
        L0dimuon<m_inputVertices->end(); ++L0dimuon){

    SmartRefVector< LHCb::Track >  trks = (*L0dimuon)->tracks();
    if(trks.size()!=2) return StatusCode::FAILURE;
    if(m_debug) debug()<<"both L0 muons exist in input vertex "<<endreq;

    if(!contains(*m_outputTracks,trks[0])) m_outputTracks->push_back(trks[0]);
    if(!contains(*m_outputTracks,trks[1])) m_outputTracks->push_back(trks[1]);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0Vertex2Tracks::finalize() 
{
  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions
}

