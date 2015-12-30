// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
// Local
#include "PatPVOffline.h"

DECLARE_ALGORITHM_FACTORY(PatPVOffline)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPVOffline::PatPVOffline(const std::string& name,
                           ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name,pSvcLocator)
  , m_pvsfit(0)
{
  declareProperty("OutputVertices",m_outputVertices =
                  LHCb::RecVertexLocation::Primary);
}

//=============================================================================
// Destructor
//=============================================================================
PatPVOffline::~PatPVOffline() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PatPVOffline::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "Initialisation" << endmsg;
  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
  if(!m_pvsfit) {
    err() << "Unable to retrieve the PVOfflineTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  return sc;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode PatPVOffline::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << "Execute" << endmsg;

  LHCb::RecVertices* v2tes = new LHCb::RecVertices();
  put(v2tes,m_outputVertices);

  std::vector<LHCb::RecVertex> rvts;
  StatusCode scfit = m_pvsfit->reconstructMultiPV(rvts);
  if (scfit.isFailure()) return StatusCode::SUCCESS;

  for(const auto& rvt : rvts ) {
    LHCb::RecVertex* vertex = new LHCb::RecVertex( rvt );
    vertex->setTechnique(LHCb::RecVertex::Primary);
    v2tes->insert(vertex);
  }

  // ---> Debug
  if(msgLevel(MSG::DEBUG)) {
    debug() << endmsg;
    debug() << "TES location filled with "
            << v2tes->size() << " PrimVertices" << endmsg;
    int nVtx = 0;
    for(const auto& vertex : *v2tes ) {
      debug() << " Vertex " << nVtx << endmsg;
      debug() << " x, y, z: "
              << vertex->position().x() << " "
              << vertex->position().y() << " "
              << vertex->position().z() << endmsg;
      debug() << " Errors : "
              << sqrt(vertex->covMatrix()(0,0)) << " "
              << sqrt(vertex->covMatrix()(1,1)) << " "
              << sqrt(vertex->covMatrix()(2,2)) << endmsg;
      debug() << " Number of tracks: "
              << vertex->tracks().size() << endmsg;
      debug() << " Chi2/DoF: " << vertex->chi2()/vertex->nDoF() << endmsg;
    }
    debug() << endmsg;
  }

  return StatusCode::SUCCESS;
}
