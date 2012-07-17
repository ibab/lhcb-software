#include <algorithm>
#include <numeric>

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "VeloDet/DeVelo.h"

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
PVSplit::PVSplit(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator),
  m_splitMethod(Unknown),
  m_pvsfit(0),
  m_splitTracks(2),
  m_splitTracksWSum(2)
{
  declareProperty("InputVerticesLocation",
    m_inputVerticesLocation = LHCb::RecVertexLocation::Primary);
  declareProperty("OutputVerticesLocation",
    m_outputVerticesLocation = "Rec/Vertex/Split");
  declareProperty("SplitMethod", m_splitMethodStr = "random");
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

  std::transform(m_splitMethodStr.begin(), m_splitMethodStr.end(),
                 m_splitMethodStr.begin(), ::tolower);
  if (m_splitMethodStr.compare("random") == 0)
    m_splitMethod = Random;
  else if (m_splitMethodStr.compare("velohalf") == 0)
    m_splitMethod = VeloHalf;
  else {
    err() << "Unknown split method '" << m_splitMethodStr << "'!" << endmsg;
    return StatusCode::FAILURE;
  }
  
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

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);

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
      debugVertex(vx);
    }

    m_tracks.assign(vx->tracks().begin(), vx->tracks().end());
    if (vx->weights().size() == vx->tracks().size())
      m_weights.assign(vx->weights().begin(), vx->weights().end());
    else
      m_weights.assign(vx->tracks().size(), 1.0);
    
    if (m_splitMethod == Random) {
        randomShuffleTracks();
        splitTracksByMiddle();
    }
    else if (m_splitMethod == VeloHalf) {
        splitTracksByVeloHalf();
    }
    
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
          debugVertex(&splitVx);
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

void PVSplit::randomShuffleTracks() {
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

void PVSplit::splitTracksByMiddle() {
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

void PVSplit::splitTracksByVeloHalf() {
  for (int i = 0; i < 2; i++) {
    m_splitTracks[i].clear();
    m_splitTracksWSum[i] = 0;
  }

  int n = m_tracks.size();
  for (int i = 0; i < n; i++) {
    int left, right;
    countVeloLhcbIDs(m_tracks[i], left, right);
    
    int j = (left > right)?0:1;
    if (left == right)
      j = static_cast<int>(m_rndm.shoot() + 0.5); // 0 or 1
    
    m_splitTracks[j].push_back(m_tracks[i]);
    m_splitTracksWSum[j] += m_weights[i];
  }
}

void PVSplit::countVeloLhcbIDs(const LHCb::Track* track, int& left, int& right) {
  left = 0;
  right = 0;
  const LHCb::Track::LHCbIDContainer& lhcbIDs = track->lhcbIDs();
  LHCb::Track::LHCbIDContainer::const_iterator it;
  for (it = lhcbIDs.begin(); it != lhcbIDs.end(); ++it) {
    const LHCb::LHCbID& lhcbID = *it;
    if (lhcbID.isVelo()) {
      const DeVeloSensor* sensor = m_velo->sensor(lhcbID.veloID());
      if (sensor->isLeft())
        left++;
      else
        right++;
    }
  }
}

void PVSplit::debugVertex(const LHCb::RecVertex* vx) const {
  std::ostringstream oss;
  vx->fillStream(oss);
  debug() << oss.str() << endmsg;
}
