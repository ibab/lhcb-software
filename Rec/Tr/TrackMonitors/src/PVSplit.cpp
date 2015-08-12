#include <algorithm>
#include <numeric>

// From Gaudi
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
      m_pvsfit(NULL),
      m_velo(NULL),
      m_splitTracks(2),
      m_splitTracksWSum(2)
{
  declareProperty("InputVerticesLocation",
                  m_inputVerticesLocation = LHCb::RecVertexLocation::Primary);
  declareProperty("OutputVerticesLocation",
                  m_outputVerticesLocation = "Rec/Vertex/Split");
  declareProperty("RandomShuffle", m_randomShuffle = true);
  declareProperty("SplitMethod", m_splitMethodStr = "middle");
  declareProperty("UsePVForSeed", m_usePVForSeed = true);
  declareProperty("CopyPV", m_copyPV = false);
  declareProperty("KeyOfParentVertexIndex", m_keyOfParentVertexIndex = 1000001);
  declareProperty("KeyOfSumOfParentWeights", m_keyOfSumOfParentWeights = 1000002);
  declareProperty("KeyOfParentVertexZ", m_keyOfParentVertexZ = 1000003);
}

//=============================================================================
// Destructor
//=============================================================================
PVSplit::~PVSplit() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PVSplit::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "Initialisation" << endmsg;

  std::transform(m_splitMethodStr.begin(), m_splitMethodStr.end(),
                 m_splitMethodStr.begin(), ::tolower);
  if (m_splitMethodStr.compare("middle") == 0)
    m_splitTracksFun = &PVSplit::splitTracksByMiddle;
  else if (m_splitMethodStr.compare("velohalf") == 0)
    m_splitTracksFun = &PVSplit::splitTracksByVeloHalf;
  else if (m_splitMethodStr.compare("middlepervelohalf") == 0)
    m_splitTracksFun = &PVSplit::splitTracksByMiddlePerVeloHalf;
  else if (m_splitMethodStr.compare("velotopbottom") == 0)
    m_splitTracksFun = &PVSplit::splitTracksByVeloTopBottom;
  else {
    err() << "Unknown split method '" << m_splitMethodStr << "'!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool", this);

  // randomn number generator
  if (!m_rndm.initialize(randSvc(), Rndm::Flat(0., 1.))) {
    return Error("Unable to create Random generator");
  }

  m_velo = getDet<DeVelo>(DeVeloLocation::Default);

  return sc;
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

  std::vector<LHCb::RecVertex> rvts; // output for reconstructMultiPVFromTracks()

  LHCb::RecVertices::const_iterator it;
  for (it = inputVertices->begin(); it != inputVertices->end(); ++it) {
    const LHCb::RecVertex* vx = *it;
    LHCb::RecVertex::key_type vxKey = vx->key();

    if (m_copyPV) {
      LHCb::RecVertex* vxCopy = vx->clone();
      vxCopy->addInfo(m_keyOfParentVertexIndex, vxKey);
      outputVertices->insert(vxCopy);
    }

    if (msgLevel(MSG::DEBUG)) {
      debug() << "Original vertex:\n";
      debugVertex(vx);
    }

    m_tracks.assign(vx->tracks().begin(), vx->tracks().end());
    if (vx->weights().size() == vx->tracks().size())
      m_weights.assign(vx->weights().begin(), vx->weights().end());
    else
      m_weights.assign(vx->tracks().size(), 1.0);

    if (m_randomShuffle)
      randomShuffleTracks();

    // Split tracks
    (this->*m_splitTracksFun)();  // call m_splitTracksFun bound function on "this"

    for (int j = 0; j < 2; j++) {
      LHCb::RecVertex splitVx;  // sensible defaults, see RecVertex() and VertexBase()
      bool ok = false;

      if (m_usePVForSeed) {
        StatusCode scfit = m_pvsfit->reconstructSinglePVFromTracks(
          vx->position(), m_splitTracks[j], splitVx);
        ok = (scfit == StatusCode::SUCCESS);
      } else {
        rvts.clear();
        StatusCode scfit = m_pvsfit->reconstructMultiPVFromTracks(
          m_splitTracks[j], rvts);
        ok = (scfit == StatusCode::SUCCESS) && (!rvts.empty());
        if (ok) splitVx = rvts[0];
      }

      // The next is to protect from the future (setTechnique() is redundant as
      // the default is Unknown and the fitter should set Primary on success.)
      if (ok) {
        splitVx.setTechnique(LHCb::RecVertex::Primary);
      } else {
        splitVx.setTechnique(LHCb::RecVertex::Unknown);
        splitVx.clearTracks(); // number of tracks is used to identify success
      }

      splitVx.addInfo(m_keyOfParentVertexIndex, vxKey);
      splitVx.addInfo(m_keyOfSumOfParentWeights, m_splitTracksWSum[j]);
      splitVx.addInfo(m_keyOfParentVertexZ, vx->position().z());

      outputVertices->insert(splitVx.clone());

      if (msgLevel(MSG::DEBUG)) {
        debug() << "Split vertex " << j+1 << ":\n";
        debugVertex(&splitVx);
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

void PVSplit::clearSplitTracks() {
  for (int i = 0; i < 2; i++) {
    m_splitTracks[i].clear();
    m_splitTracksWSum[i] = 0;
  }
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
  int oddShift = (m_rndm.shoot() > 0.5); // add 0 or 1 if odd number of tracks
  unsigned int n1 = randomMiddle(m_tracks.size(), oddShift);

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

void PVSplit::splitTracksByVeloTopBottom() {
  clearSplitTracks();

  int n = m_tracks.size();
  for (int i = 0; i < n; i++) {
    int j = isTopTrack(m_tracks[i]);
    m_splitTracks[j].push_back(m_tracks[i]);
    m_splitTracksWSum[j] += m_weights[i];
  }
}

void PVSplit::splitTracksByVeloHalf() {
  clearSplitTracks();

  int n = m_tracks.size();
  for (int i = 0; i < n; i++) {
    int j = isLeftTrack(m_tracks[i]);
    m_splitTracks[j].push_back(m_tracks[i]);
    m_splitTracksWSum[j] += m_weights[i];
  }
}

void PVSplit::splitTracksByMiddlePerVeloHalf() {
  clearSplitTracks();

  std::vector<unsigned int> lrIndices[2];
  for (unsigned int i = 0; i < m_tracks.size(); i++) {
    int h = isLeftTrack(m_tracks[i]);
    lrIndices[h].push_back(i);
  }

  // Determine the number of tracks for the first split vertex from the
  // left and right set of tracks.
  int oddShift = (m_rndm.shoot() > 0.5); // 0 or 1
  unsigned int n1[2] = {randomMiddle(lrIndices[0].size(), oddShift),
                        randomMiddle(lrIndices[1].size(), 1 - oddShift)};

  if (msgLevel(MSG::DEBUG)) {
    debug() << "splitTracksByMiddlePerVeloHalf():\n";
    for (int h = 0; h < 2; h++) {
      if (h == 0)
        debug() << "    left:   ";
      else
        debug() << "    right:  ";
      debug() << "n1 = " << n1[h]
              << "  n2 = " << lrIndices[h].size() - n1[h] << '\n';
    }
    debug() << endmsg;
  }

  for (int h = 0; h < 2; h++) {
    for (unsigned int k = 0; k < lrIndices[h].size(); k++) {
      int i = lrIndices[h][k];
      int j = (k < n1[h]);
      m_splitTracks[j].push_back(m_tracks[i]);
      m_splitTracksWSum[j] += m_weights[i];
    }
  }
}

unsigned int PVSplit::randomMiddle(unsigned int n, int oddShift){
  return n / 2 + (n % 2)*oddShift;
}

void PVSplit::countVeloLhcbIDsLeftRight(const LHCb::Track* track, int& left, int& right) const {
  int n = 0;
  left = 0;
  const LHCb::Track::LHCbIDContainer& lhcbIDs = track->lhcbIDs();
  LHCb::Track::LHCbIDContainer::const_iterator it;
  for (it = lhcbIDs.begin(); it != lhcbIDs.end(); ++it) {
    const LHCb::LHCbID& lhcbID = *it;
    if (lhcbID.isVelo()) {
      const DeVeloSensor* sensor = m_velo->sensor(lhcbID.veloID());
      left += sensor->isLeft();
      n++;
    }
  }
  right = n - left;
}

void PVSplit::countVeloLhcbIDsTopBottom(const LHCb::Track* track, int& top, int& bottom) const {
  int n = 0;
  top = 0;
  const LHCb::Track::LHCbIDContainer& lhcbIDs = track->lhcbIDs();
  LHCb::Track::LHCbIDContainer::const_iterator it;
  for (it = lhcbIDs.begin(); it != lhcbIDs.end(); ++it) {
    const LHCb::LHCbID& lhcbID = *it;
    if (lhcbID.isVelo()) {
      const LHCb::VeloChannelID veloID = lhcbID.veloID();
      const DeVeloPhiType* sensor = m_velo->phiSensor(veloID);
      if (sensor) {  // check if sensor is indeed phi type
        // can use globalPhiOfStrip() as well (makes tiny difference)
        double phi = sensor->idealPhiOfStrip(veloID.strip());
        top += (sin(phi) > 0);
        n++;
      }
    }
  }
  bottom = n - top;
}


bool PVSplit::isLeftTrack(const LHCb::Track* track) {
  int nLeftIDs, nRightIDs;
  countVeloLhcbIDsLeftRight(track, nLeftIDs, nRightIDs);

  if (nLeftIDs != nRightIDs)
    return nLeftIDs > nRightIDs;
  else
    return m_rndm.shoot() < 0.5;
}

bool PVSplit::isTopTrack(const LHCb::Track* track) {
  int nTopIDs, nBottomIDs;
  countVeloLhcbIDsTopBottom(track, nTopIDs, nBottomIDs);

  if (nTopIDs != nBottomIDs)
    return nTopIDs > nBottomIDs;
  else
    return m_rndm.shoot() < 0.5;
}

void PVSplit::debugVertex(const LHCb::RecVertex* vx) const {
  std::ostringstream oss;
  vx->fillStream(oss);
  debug() << oss.str() << endmsg;
}
