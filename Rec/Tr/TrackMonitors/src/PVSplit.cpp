#include <algorithm>
#include <numeric>

// From Gaudi
#include "GaudiKernel/AlgFactory.h"

// Local
#include "PVSplit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVSplit
//
// 2012-06-21 : Colin Barschel, Rosen Matev
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PVSplit )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVSplit::PVSplit( const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
  m_pvsfit(0),
  m_splitTracks(2),
  m_splitTracksWSum(2)
{
  declareProperty("InputVerticesLocation",
    m_inputVerticesLocation = LHCb::RecVertexLocation::Primary);
  declareProperty("OutputVerticesLocation",
    m_outputVerticesLocation = "Rec/Vertex/Split");
}

//=============================================================================
// Destructor
//=============================================================================
PVSplit::~PVSplit() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PVSplit::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "Initialisation" << endmsg;
  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
  if (!m_pvsfit) {
    err() << "Unable to retrieve the PVOfflineTool" << endmsg;
    return StatusCode::FAILURE;
  }
  
  // randomn number generator
  if (!m_rndm.initialize(randSvc(), Rndm::Flat(0., 1.))) {
    return Error("Unable to create Random generator");
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVSplit::execute() {
  if (msgLevel(MSG::DEBUG)) debug() << "Execute" << endmsg;

  LHCb::RecVertices* inputVertices =
    get<LHCb::RecVertices>(m_inputVerticesLocation);
  if (!inputVertices) {
    err() << "Vertices not found at location: "
          << m_inputVerticesLocation << endmsg;
    return StatusCode::FAILURE;
  }

  LHCb::RecVertices* outputVertices = new LHCb::RecVertices();
  put(outputVertices, m_outputVerticesLocation);

  LHCb::RecVertices::const_iterator it;
  for (it = inputVertices->begin(); it != inputVertices->end(); ++it) {
    const LHCb::RecVertex* vx = *it;
    LHCb::RecVertex::key_type vxKey = vx->key();

    if (msgLevel(MSG::DEBUG)) {
      debug() << "Original vertex:\n";
      debug_vertex(vx);
    }

    m_tracks.assign(vx->tracks().begin(), vx->tracks().end());
    if (vx->weights().size() == vx->tracks().size())
      m_weights.assign(vx->weights().begin(), vx->weights().end());
    else
      m_weights.assign(vx->tracks().size(), 1.0);
    
    random_shuffle_tracks();
    split_tracks();
    
    for (int j = 0; j < 2; j++) {
      LHCb::RecVertex splitVx;
      StatusCode scfit = m_pvsfit->reconstructSinglePVFromTracks(
                           vx->position(), m_splitTracks[j], splitVx);
      if (scfit == StatusCode::SUCCESS) {
        splitVx.addInfo(ParentVertexIndex, vxKey);
        splitVx.addInfo(SumOfParentWeights, m_splitTracksWSum[j]);
        splitVx.setTechnique(LHCb::RecVertex::Primary);
        outputVertices->insert(splitVx.clone());

        if (msgLevel(MSG::DEBUG)) {
          debug() << "Split vertex " << j+1 << ":\n";
          debug_vertex(&splitVx);
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVSplit::finalize() {
  // finalize random number generator
  const StatusCode sc = m_rndm.finalize();
  if (sc.isFailure())
    Warning( "Failed to finalise random number generator" );
  
  return GaudiAlgorithm::finalize(); // must be called after all other actions
}

void PVSplit::random_shuffle_tracks() {
  unsigned int i, n;
  n = m_tracks.size();
  for (i = n - 1; i > 0; --i) {
    // pick random int from 0 to i inclusive
    int j = static_cast<int>(m_rndm.shoot() * (i + 1));
    std::swap(m_tracks[i], m_tracks[j]);
    std::swap(m_weights[i], m_weights[j]);
  }
  
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Randomized tracks (n=" << m_tracks.size() << "):\n";
    for (i = 0; i < n; i++)
      debug() << m_tracks[i] << " " << m_weights[i] << "\n";
    debug() << endmsg;
  }
}

void PVSplit::split_tracks() {
  unsigned int n1 = m_tracks.size() / 2;
  if (m_tracks.size() % 2 == 1)
    n1 += static_cast<int>(m_rndm.shoot() + 0.5); // add 0 or 1
     
  if (msgLevel(MSG::DEBUG))
    debug() << "Split track containers size: n1=" << n1
            << ", n2=" << m_tracks.size() - n1 << endmsg;
      
  m_splitTracks[0].assign(m_tracks.begin(), m_tracks.begin() + n1);
  m_splitTracks[1].assign(m_tracks.begin() + n1, m_tracks.end());
  m_splitTracksWSum[0] = std::accumulate(m_weights.begin(),
                                         m_weights.begin() + n1, 0.);
  m_splitTracksWSum[1] = std::accumulate(m_weights.begin() + n1,
                                         m_weights.end(), 0.);
}

void PVSplit::debug_vertex(const LHCb::RecVertex* vx) const {
  std::ostringstream oss;
  vx->fillStream(oss);
  debug() << oss.str() << endmsg;
}
