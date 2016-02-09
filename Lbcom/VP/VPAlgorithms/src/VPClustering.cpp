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
namespace {

//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool isEdge(const LHCb::VPDigit& digit) {
  auto id = digit.channelID();
  return id.col() == 255 ?  id.chip() < 2
       : id.col() == 0   ?  id.chip() > 0
       : false;
}

}
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
  auto itBegin = digits->begin();
  auto itLast = std::prev(digits->end());

  std::vector<const LHCb::VPDigit*> cluster;
  cluster.reserve(100);
  std::vector<LHCb::VPChannelID> pixels;
  pixels.reserve(100);
  // Keep track of used digits.
  std::vector<bool> isUsed(digits->size(), false);
  // Loop over digits.
  for (auto itSeed = digits->begin(); itSeed != digits->end(); ++itSeed) {
    if (isUsed[itSeed - itBegin]) continue;
    // Create a new cluster with this seed.
    cluster.clear();
    cluster.push_back(*itSeed);
    // Store the sensor of this cluster.
    const unsigned int sensorNumber = (*itSeed)->channelID().sensor();
    // Tag the digit as used.
    isUsed[itSeed - itBegin] = true;
    // Look for neighbouring hits until the cluster size stops changing.
    bool keepSearching = (itSeed != itLast);
    // Start the search on the next unused pixel.
    auto itCandBegin = itSeed;
    while (isUsed[itCandBegin - itBegin]) {
      if (itCandBegin == itLast) break;
      ++itCandBegin;
    }
    while (keepSearching) {
      keepSearching = false;
      // Loop over the digits which are already pare of the cluster.
      for (unsigned int i = 0; i < cluster.size(); ++i) {
        // Store some properties of this digit.
        const bool edge = isEdge(*cluster[i]);
        const unsigned int chip = cluster[i]->channelID().chip();
        const int row = cluster[i]->channelID().row();
        const int col = cluster[i]->channelID().col();
        // Loop down the stored digits until new pixels cannot be part of
        // cluster.
        for (auto iCand = itCandBegin; iCand != digits->end(); ++iCand) {
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
            cluster.push_back(*iCand);
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
              cluster.push_back(*iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            } else {
              // Not on the same chip
              if (!isEdge(**iCand)) break;  // No hits on neighbouring edge
              // Skip pixels not neighbouring in y.
              if (abs(row - int((*iCand)->channelID().row())) > 1) continue;
              // Add pixel and tag it as used.
              cluster.push_back(*iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            }
          }  // End of edge pixel check
        }
      }
    }
    // Calculate the barycentre of the cluster.
    const DeVPSensor* vp_sensor = m_vpDet->sensorOfChannel(cluster[0]->channelID());
    pixels.clear();
    std::transform( clusters->begin(), clusters->end(), std::back_inserter(pixels),
                    [](const LHCb::VPCluster* cluster) { return cluster->channelID(); } );
    auto xy = std::accumulate( pixels.begin(), pixels.end(), std::make_pair(0.,0.),
                               [&](std::pair<double,double> xy, const LHCb::VPChannelID& channel) {
                Gaudi::XYZPoint pixel = vp_sensor->channelToPoint(channel, true);
                xy.first += pixel.x(); xy.second+=pixel.y(); return xy;
    });
    const unsigned int nPixels = pixels.size();
    Gaudi::XYZPoint point(xy.first/nPixels, xy.second/nPixels, 0.);
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
