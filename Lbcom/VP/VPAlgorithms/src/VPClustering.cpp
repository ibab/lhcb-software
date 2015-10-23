// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPDataFunctor.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
// Det/VPDet
#include "VPDet/DeVP.h"

// Local
#include "VPClustering.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPClustering
//
// 2013-08-13 : Daniel Hynds
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPClustering)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClustering::VPClustering(const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiAlgorithm(name, pSvcLocator), m_vpDet(NULL), m_debug(false) {

  declareProperty("DigitContainer",
                  m_digitLocation = LHCb::VPDigitLocation::Default);
  declareProperty("VPClusterContainer",
                  m_clusterLocation = LHCb::VPClusterLocation::Default);

}

//=============================================================================
// Destructor
//=============================================================================
VPClustering::~VPClustering() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPClustering::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_debug = msgLevel(MSG::DEBUG);
  // Get detector element
  m_vpDet = getDet<DeVP>(DeVPLocation::Default);
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPClustering::execute() {

  // Check if clusters already exist.
  if (exist<LHCb::VPClusters>(m_clusterLocation)) {
    // If clusters are decoded there is no need to build clusters.
    if (m_debug) {
      debug() << "Clusters already exist. No clustering taking place" << endmsg;
    }
    return StatusCode::SUCCESS;
  }
  // Cluster the digits.
  if (m_debug) debug() << "Clustering digits" << endmsg;
  // Pick up pixel hits (digits) to cluster
  LHCb::VPDigits* digits = getIfExists<LHCb::VPDigits>(m_digitLocation);
  if (!digits) {
    error() << "No digits in " << m_digitLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Create container and transfer ownership to the TES.
  VPClusters* clusters = new VPClusters();
  put(clusters, m_clusterLocation);

  // Sort the digits by channel ID.
  std::stable_sort(digits->begin(), digits->end(),
                   VPDataFunctor::Less_by_Channel<const LHCb::VPDigit*>());
  LHCb::VPDigits::const_iterator itBegin = digits->begin();
  LHCb::VPDigits::const_iterator itLast = digits->end() - 1;

  std::vector<LHCb::VPDigits::const_iterator> cluster;
  cluster.reserve(100);
  std::vector<LHCb::VPChannelID> pixels;
  pixels.reserve(100);
  // Keep track of used digits.
  std::vector<bool> isUsed(digits->size(), false);
  // Loop over digits.
  LHCb::VPDigits::const_iterator itSeed;
  for (itSeed = digits->begin(); itSeed != digits->end(); ++itSeed) {
    if (isUsed[itSeed - itBegin]) continue;
    // Create a new cluster with this seed.
    cluster.clear();
    cluster.push_back(itSeed);
    // Store the sensor of this cluster.
    const unsigned int sensorNumber = (*itSeed)->channelID().sensor();
    // Tag the digit as used.
    isUsed[itSeed - itBegin] = true;
    // Look for neighbouring hits until the cluster size stops changing.
    bool keepSearching = true;
    if (itSeed == itLast) keepSearching = false;
    // Start the search on the next unused pixel.
    LHCb::VPDigits::const_iterator itCandBegin = itSeed;
    while (isUsed[itCandBegin - itBegin]) {
      if (itCandBegin == itLast) break;
      ++itCandBegin;
    }
    while (keepSearching) {
      keepSearching = false;
      // Loop over the digits which are already pare of the cluster.
      for (unsigned int i = 0; i < cluster.size(); ++i) {
        // Store some properties of this digit.
        const bool edge = isEdge((*cluster[i]));
        const unsigned int chip = (*cluster[i])->channelID().chip();
        const int row = (*cluster[i])->channelID().row();
        const int col = (*cluster[i])->channelID().col();
        // Loop down the stored digits until new pixels cannot be part of
        // cluster.
        LHCb::VPDigits::const_iterator iCand;
        for (iCand = itCandBegin; iCand != digits->end(); ++iCand) {
          if (isUsed[iCand - itBegin]) continue;
          // Check if the candidate is on the same sensor.
          if (sensorNumber != (*iCand)->channelID().sensor()) break;
          if (!edge) {
            if (chip != (*iCand)->channelID().chip())
              break;  // Next hit not on same chip
            // Skip pixels not neighbouring in y.
            if (abs(row - int((*iCand)->channelID().row())) > 1) continue;
            if (abs(col - int((*iCand)->channelID().col())) > 1)
              continue;  // Too far away to be added
            // Add pixel and tag it as used.
            cluster.push_back(iCand);
            isUsed[iCand - itBegin] = true;
            keepSearching = true;
            break;
          } else {
            // Deal with edge pixels
            if (chip == (*iCand)->channelID().chip()) {
              // On the same chip
              // Skip pixels not neighbouring in y.
              if (abs(row - int((*iCand)->channelID().row())) > 1) continue;
              if (abs(col - int((*iCand)->channelID().col())) > 1)
                continue;  // Too far away to be added
              // Add pixel and tag it as used.
              cluster.push_back(iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            } else {
              // Not on the same chip
              if (!isEdge(*iCand)) break;  // No hits on neighbouring edge
              // Skip pixels not neighbouring in y.
              if (abs(row - int((*iCand)->channelID().row())) > 1) continue;
              // Add pixel and tag it as used.
              cluster.push_back(iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            }
          }  // End of edge pixel check
        }
      }
    }
    // Calculate the barycentre of the cluster.
    double x = 0.;
    double y = 0.;
    const DeVPSensor* vp_sensor =
        m_vpDet->sensorOfChannel((*cluster[0])->channelID());
    const unsigned int nPixels = cluster.size();
    pixels.resize(nPixels);
    for (unsigned int i = 0; i < nPixels; ++i) {
      LHCb::VPChannelID channel = (*cluster[i])->channelID();
      pixels[i] = channel;
      Gaudi::XYZPoint pixel = vp_sensor->channelToPoint(channel, true);
      x += pixel.x();
      y += pixel.y();
    }
    x /= nPixels;
    y /= nPixels;
    Gaudi::XYZPoint point(x, y, 0.);
    // Get the channel ID and inter-pixel fractions of the barycentre.
    LHCb::VPChannelID id;
    std::pair<double, double> frac;
    if (!vp_sensor->pointToChannel(point, true, id, frac)) continue;
    // Make sure there isn't already a cluster with the same channel ID.
    if (clusters->object(id)) {
      warning() << "Duplicated channel ID " << id << endmsg;
      continue;
    }
    // Calculate the position in global coordinates.
    point = vp_sensor->localToGlobal(point);
    // Add the cluster to the list.
    LHCb::VPCluster* newCluster =
        new LHCb::VPCluster(frac, point.x(), point.y(), point.z(), pixels);
    clusters->insert(newCluster, id);
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool VPClustering::isEdge(LHCb::VPDigit* digit) const {

  if (digit->channelID().col() == 255) {
    if (digit->channelID().chip() < 2) return true;
  } else if (digit->channelID().col() == 0) {
    if (digit->channelID().chip() > 0) return true;
  }
  return false;

}
