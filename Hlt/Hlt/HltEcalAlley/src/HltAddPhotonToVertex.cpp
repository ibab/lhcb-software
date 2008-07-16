// $Id: HltAddPhotonToVertex.cpp,v 1.1.1.1 2008-07-16 16:54:04 witekma Exp $
// Include files 


// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltAddPhotonToVertex.h"
//#include "HltBase/EParser.h"
//#include "HltBase/ESequences.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltAddPhotonToVertex
//
// 2008-07-10 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltAddPhotonToVertex );

using namespace LHCb;
//using namespace boost::lambda;
// #namespace bl = boost::lambda;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltAddPhotonToVertex::HltAddPhotonToVertex( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{

  m_inputVertices = 0;
  m_inputTracks   = 0;
  m_outputVertices = 0;

}

//=============================================================================
// Initialization
//=============================================================================

StatusCode HltAddPhotonToVertex::initialize() {
  StatusCode sc = HltAlgorithm::initialize();

  m_inputVertices = &(retrieveTSelection<LHCb::RecVertex>(m_inputSelectionName));
  m_inputTracks = &(retrieveTSelection<LHCb::Track>(m_inputSelection2Name));
  
  m_outputVertices = &(registerTSelection<LHCb::RecVertex>());

  saveConfiguration();

  return sc;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode HltAddPhotonToVertex::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  if ( m_debug ) debug() << "HltAddPhotonToVertex: Execute" << endmsg;

  RecVertices* overtices = new RecVertices();
  put(overtices,"Hlt/Vertex/"+m_outputVertices->id().str());

  if (( m_inputVertices->size() < 1 || m_inputTracks->size() <1)) {
    if ( m_debug ) debug() << " no tracks or no vertices to run on  " << endreq;
    return sc;
  } 

  for(  Hlt::VertexContainer::const_iterator it= m_inputVertices->begin(); 
        it != m_inputVertices->end(); ++it){

    const LHCb::RecVertex& vx_in = *(*it);

    for (Hlt::TrackContainer::const_iterator itM = m_inputTracks->begin(); 
         itM != m_inputTracks->end(); itM++) {
      const LHCb::Track& track1 = *(*itM);
    

      LHCb::RecVertex* vx_out = new RecVertex();
      *vx_out = vx_in;

      vx_out->addToTracks((Track*) &track1);

      m_outputVertices->push_back(vx_out);
      overtices->insert(vx_out);

    }
  }
  
  return StatusCode::SUCCESS;  
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltAddPhotonToVertex::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  // must be called after all other actions 
  
}

//=============================================================================

