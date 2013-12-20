// Gaudi
#include "GaudiKernel/AlgFactory.h"
// LHCb
// Kernel/LHCbKernel
#include "Kernel/SiDataFunctor.h"
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
VPClustering::VPClustering(const std::string& name, ISvcLocator* pSvcLocator) :
  GaudiAlgorithm(name, pSvcLocator)
, m_vpDet(NULL)
, m_debug(false)
{

  declareProperty("DigitContainer",       m_digitLocation       = LHCb::VPDigitLocation::VPDigitLocation);
  declareProperty("LiteClusterContainer", m_liteClusterLocation = LHCb::VPLiteClusterLocation::Default);
  declareProperty("VPClusterContainer",   m_clusterLocation     = LHCb::VPClusterLocation::VPClusterLocation);

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

  // Check if LiteClusters already decoded. 
  if (exist<LHCb::VPLiteCluster::VPLiteClusters>(m_digitLocation)) {
    // If lite clusters are decoded there is no need to build clusters.
    if (m_debug) {
      debug() << "Lite clusters already exist. No clustering taking place" << endmsg;
    }
    return StatusCode::SUCCESS;
  }  
  // If no lite clusters, then cluster the digits.
  if (m_debug) debug() << "Clustering digits" << endmsg;
  // Pick up pixel hits (digits) to cluster
  LHCb::VPDigits* digits = getIfExists<LHCb::VPDigits>(m_digitLocation);
  if (!digits) {
    error() << "No digits in " << m_digitLocation << endmsg;
    return StatusCode::SUCCESS;
  }
  // Create containers for clusters and lite clusters.
  VPLiteCluster::VPLiteClusters* liteClusters = new VPLiteCluster::VPLiteClusters();
  VPClusters* clusters = new VPClusters();

  // Sort digits by channelID
  std::sort(digits->begin(), digits->end(), less_than_channelID()); 
  LHCb::VPDigits::const_iterator itBegin = digits->begin();
  LHCb::VPDigits::const_iterator itLast = digits->end() - 1;
 
  std::vector<LHCb::VPDigits::const_iterator> cluster;
  cluster.reserve(100);
  std::vector<std::pair<LHCb::VPChannelID, int> > totVec;
  totVec.reserve(100);
  // Keep track of used digits.
  std::vector<bool> isUsed(digits->size(), false);
  // Loop over digits.
  LHCb::VPDigits::const_iterator itSeed; 
  for (itSeed = digits->begin(); itSeed != digits->end(); ++itSeed) {
    if(isUsed[itSeed - itBegin]) continue;
    // Create a new cluster with this seed.
    cluster.clear();
    cluster.push_back(itSeed);
    // Store the sensor and module of this cluster.
    const unsigned int sensorNumber = floor((*itSeed)->channelID().chip() / 3);
    const unsigned int module = (*itSeed)->channelID().module();
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
        const unsigned int row = (*cluster[i])->channelID().row();
        const unsigned int col = (*cluster[i])->channelID().col();
        // Loop down the stored digits until new pixels cannot be part of cluster.
        LHCb::VPDigits::const_iterator iCand;
        for (iCand = itCandBegin; iCand != digits->end(); ++iCand) {
          if (isUsed[iCand - itBegin]) continue;
          // Check if the candidate is on the same module.
          if (module != (*iCand)->channelID().module()) break;
          // Check if the candidate is on the same sensor.
          if (sensorNumber != (*iCand)->channelID().chip() / 3) break;
          if (!edge) {
            if (chip != (*iCand)->channelID().chip()) break;  // Next hit not on same chip
            // Skip pixels not neighbouring in y.
            if (abs(row - (*iCand)->channelID().row()) > 1) continue;
            if (abs(col - (*iCand)->channelID().col()) > 1) break; // Too far away to be added
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
              if (abs(row - (*iCand)->channelID().row()) > 1) continue;
              if (abs(col - (*iCand)->channelID().col()) > 1) break; // Too far away to be added
              // Add pixel and tag it as used.
              cluster.push_back(iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            } else {
              // Not on the same chip
              if (!isEdge(*iCand)) break; // No hits on neighbouring edge
              // Skip pixels not neighbouring in y.
              if (abs(row - (*iCand)->channelID().row()) > 1) continue;
              // Add pixel and tag it as used.
              cluster.push_back(iCand);
              isUsed[iCand - itBegin] = true;
              keepSearching = true;
              break;
            }
          } // End of edge pixel check
        }
      }
    }
    // Calculate the barycentre of the cluster. 
    double x = 0.;
    double y = 0.;
    const DeVPSensor* vp_sensor = m_vpDet->sensorOfChannel((*cluster[0])->channelID());
    int sum = 0;
    const unsigned int nPixels = cluster.size();
    totVec.resize(nPixels);
    for (unsigned int i = 0; i < nPixels; ++i) {
      LHCb::VPChannelID channel = (*cluster[i])->channelID();
      const int tot = (*cluster[i])->ToTValue();
      totVec[i] = std::make_pair(channel, tot);
      sum += tot;
      Gaudi::XYZPoint pixel = vp_sensor->channelToPoint(channel, true);
      x += pixel.x() * tot;
      y += pixel.y() * tot;
    }
    x /= sum; 
    y /= sum;
    Gaudi::XYZPoint point(x, y, 0.);
    // Get the channel ID and inter-pixel fractions of the barycentre.
    LHCb::VPChannelID id;
    std::pair<double, double> frac;
    StatusCode sc = vp_sensor->pointToChannel(point, true, id, frac);
    if (sc == StatusCode::FAILURE) continue;
    // Make sure there isn't already a cluster with the same channel ID.
    if (clusters->object(id)) {
      warning() << "Duplicated channel ID " << id << endmsg;
      continue; 
    }
    // Map the interpixel-fractions to 3-bit integers.
    std::pair<unsigned int,  unsigned int> intFrac;
    intFrac.first = int(ceil(frac.first * 7));
    intFrac.second = int(ceil(frac.second * 7));
    if (intFrac.first > 7) intFrac.first = 7;
    if (intFrac.second > 7) intFrac.second = 7; 
    // Add the lite cluster to the list.
    bool isLong = false; 
    const VPLiteCluster newLiteCluster(id, 1, intFrac, isLong);
    liteClusters->push_back(newLiteCluster);
    // Add the cluster to the list. 
    LHCb::VPCluster* newCluster = new LHCb::VPCluster(newLiteCluster, totVec);
    clusters->insert(newCluster, id);
  }
  // Sort the lite clusters.
  std::sort(liteClusters->begin(), liteClusters->end(), 
            SiDataFunctor::Less_by_Channel<LHCb::VPLiteCluster>()); 
  // Store clusters and lite clusters on TES.
  put(liteClusters, m_liteClusterLocation);
  put(clusters, m_clusterLocation);
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool VPClustering::isEdge(LHCb::VPDigit* digit) const {
  
  if (digit->channelID().col() == 255) {
    if (digit->channelID().chip() % 3 < 2) return true;
  } else if (digit->channelID().col() == 0) {
    if (digit->channelID().chip() % 3 > 0) return true;
  }
  return false;
  
}

