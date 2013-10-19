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
    GaudiAlgorithm(name, pSvcLocator) {

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
  return clusterDigits();

}

//=============================================================================
// Loop over track container
//=============================================================================
StatusCode VPClustering::clusterDigits() {
  
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
  
  // Cluster while there are still pixels in the temporary storage
  unsigned int oldsize = 0;
  std::vector<bool> isUsed(digits->size()); // If each pixel is used NEW
  LHCb::VPDigits::const_iterator iSeed; // Cluster seed iterator NEW
  for (iSeed = digits->begin(); iSeed != digits->end(); iSeed++) { // NEW
    if(isUsed[iSeed - digits->begin()]) continue;
    // Create a new cluster.
    std::vector<LHCb::VPDigits::const_iterator> cluster; //NEW
    std::vector<std::pair<LHCb::VPChannelID,int> > totVec;     
    // Add the first hit in the container.
    cluster.push_back(iSeed); // NEW
    isUsed[iSeed - digits->begin()] = true;
    totVec.push_back(std::make_pair((*iSeed)->channelID(), (*iSeed)->ToTValue())); // NEW

    do {
      oldsize = cluster.size();
      for (unsigned int i = 0; i < cluster.size(); i++) { 
        LHCb::VPDigits::const_iterator iCand = cluster[i];
        if (iCand != digits->end() - 1) iCand++; // Candidate  iterator NEW
        else break;
        if ((*cluster[i])->channelID().module() != (*iCand)->channelID().module()) break;  // Next hit not on same module NEW
        if (sensorNumber((*cluster[i])) != sensorNumber(*iCand) ) break; // Next hit not on same sensor
        // Loop down the stored hits until new pixels cannot be part of cluster    
        for (; iCand != digits->end(); iCand++) { 
          if (isUsed[iCand - digits->begin()]) continue;
          if (!isEdge((*cluster[i]))) {
            // Not edge
            if ((*cluster[i])->channelID().chip() != (*iCand)->channelID().chip()) break;  // Next hit not on same chip
            if (abs((*cluster[i])->channelID().row() - (*iCand)->channelID().row()) > 1) continue; // Not neighbouring in y
            if (abs((*cluster[i])->channelID().col() - (*iCand)->channelID().col()) > 1) break; // Too far away to be added
            addToCluster(cluster, totVec, iCand); 
            isUsed[iCand - digits->begin()] = true;
            break; //NEW
          } else {
            // Deal with edge pixels
            if ((*cluster[i])->channelID().chip() == (*iCand)->channelID().chip()) {
              // On the same chip
              if (abs((*cluster[i])->channelID().row() - (*iCand)->channelID().row()) > 1) continue; // Not neighbouring in y
              if (abs((*cluster[i])->channelID().col() - (*iCand)->channelID().col()) > 1) break; // Too far away to be added
              addToCluster(cluster, totVec, iCand); 
              isUsed[iCand - digits->begin()] = true;
              break; //NEW
            } else {
              // Not on the same chip
              if (!isEdge(*iCand)) break; // No hits on neighbouring edge
              if (abs((*cluster[i])->channelID().row() - (*iCand)->channelID().row()) > 1) continue; // Not neighbouring in y
              addToCluster(cluster, totVec, iCand); 
              isUsed[iCand - digits->begin()] = true;
              break; //NEW
            }
          } // End of edge pixel check
        }
      }
    } while (cluster.size() != oldsize); // no more hits to be added to cluster
    // Calculate the barycentre of the cluster. 
    double x = 0;
    double y = 0;
    double z = 0;
    const DeVPSensor* vp_sensor = m_vpDet->sensorOfChannel((*cluster[0])->channelID());
    int clustToT = 0;
    for (unsigned int i = 0; i < cluster.size(); i++) { 
      const double tot = (*cluster[i])->ToTValue();
      clustToT += tot;
      Gaudi::XYZPoint pixel = vp_sensor->channelToPoint((*cluster[i])->channelID());
      x += pixel.x() * tot;
      y += pixel.y() * tot;
      z += pixel.z() * tot;
    }
    x /= clustToT;
    y /= clustToT;
    z /= clustToT;
    Gaudi::XYZPoint cluster_point(x, y, z);
    // Get the channel ID and inter-pixel fraction of the barycentre.
    LHCb::VPChannelID id;
    std::pair<double, double> frac;
    StatusCode mycode = vp_sensor->pointToChannel(cluster_point, id, frac);
    if (mycode == StatusCode::FAILURE) continue;
    // Make sure there isn't already a cluster with the same channel ID.
    if (clusters->object(id)) {
      warning() << "Duplicated channel ID " << id << endmsg;
      continue; 
    }
    // Add the lite cluster to the list.
    bool isLong = false; 
    const VPLiteCluster newLiteCluster(id, 1, scaleFrac(frac), isLong);
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
//  Get sensor number for a given digit
//=============================================================================
int VPClustering::sensorNumber(LHCb::VPDigit* digit) {
  return floor(digit->channelID().chip() / 3);
}


//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool VPClustering::isEdge(LHCb::VPDigit* digit) {
  
  const int ladder_position = digit->channelID().chip() % 3;
  if ((ladder_position == 0 && digit->channelID().col() == 255) ||
      (ladder_position == 1 && (digit->channelID().col() == 255 || digit->channelID().col() == 0)) ||
      (ladder_position == 2 && digit->channelID().col() == 0)) {
    return true;  
  }
  return false;
  
}

//=============================================================================
//  Add pixel to cluster
//=============================================================================
void VPClustering::addToCluster(std::vector<LHCb::VPDigits::const_iterator>& cluster, 
                                std::vector<std::pair<LHCb::VPChannelID, int> >& totVec,
                                LHCb::VPDigits::const_iterator candidate) {
  // Add hit to cluster
  totVec.push_back(std::make_pair((*candidate)->channelID(), (*candidate)->ToTValue()));
  cluster.push_back(candidate);      
}

//============================================================================
// Scale 3 bit xyFraction
//============================================================================
std::pair<unsigned int, unsigned int> 
VPClustering::scaleFrac(std::pair<double, double> xyFraction) {
  const unsigned int maxValue = 7;
  std::pair<unsigned int, unsigned int> xyFrac;
  xyFrac.first = int(ceil(xyFraction.first * maxValue));
  xyFrac.second = int(ceil(xyFraction.second * maxValue));
  if (xyFrac.first > maxValue) xyFrac.first = maxValue;
  if (xyFrac.second > maxValue) xyFrac.second = maxValue;
  return xyFrac;
}
