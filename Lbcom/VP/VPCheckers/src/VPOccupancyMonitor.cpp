// LHCb
// Kernel/LHCbKernel
#include "Kernel/VPConstants.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
// Local
#include "VPOccupancyMonitor.h"

DECLARE_ALGORITHM_FACTORY(VPOccupancyMonitor)

//=============================================================================
// Standard constructor
//=============================================================================
VPOccupancyMonitor::VPOccupancyMonitor(const std::string& name,
                                       ISvcLocator* pSvcLocator) :
    GaudiHistoAlg(name, pSvcLocator) {

  declareProperty("ClusterLocation", 
                  m_clusterLocation = LHCb::VPClusterLocation::Default);
  declareProperty("Station", m_station = 10);

}

//=============================================================================
// Destructor
//=============================================================================
VPOccupancyMonitor::~VPOccupancyMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPOccupancyMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if (sc.isFailure()) return sc;
  // Set histo dir
  setHistoTopDir("VP/");
  // Get detector element
  m_det = getDet<DeVP>(DeVPLocation::Default);

  for (auto sensor : m_det->sensors()) {
    if (sensor->station() != m_station) continue;
    const unsigned int sn = sensor->sensorNumber();
    for (unsigned int i = 0; i < VP::NChipsPerSensor; ++i) {
      for (unsigned int j = 0; j < VP::NColumns; ++j) {
        for (unsigned int k = 0; k < VP::NRows; ++k) {
          LHCb::VPChannelID id(sn, i, j, k);
          Gaudi::XYZPoint p = sensor->channelToPoint(id, false);
          plot(p.rho(), "ChannelsPerBin", 5.1, 40.1, 500); 
        } 
      }
    }
  }
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPOccupancyMonitor::execute() {

  // Get the clusters.
  const LHCb::VPClusters* clusters = getIfExists<LHCb::VPClusters>(m_clusterLocation);
  if (!clusters) {
    return Error("No clusters in " + m_clusterLocation);
  }
  // Loop over the clusters.
  for (const LHCb::VPCluster* cluster : *clusters) {
    // Check if the cluster is on the station we are interested in.
    LHCb::VPChannelID id = cluster->channelID();
    if (id.station() != m_station) continue;
    // Fill the cluster occupancy histogram.
    Gaudi::XYZPoint point(cluster->x(), cluster->y(), cluster->z());
    plot(point.rho(), "Clusters", 5.1, 40.1, 500);
    // Get the sensor.
    const DeVPSensor* sensor = m_det->sensorOfChannel(id);
    // Loop over the pixels in the cluster.
    for (auto pixel : cluster->pixels()) {
      point = sensor->channelToPoint(pixel, false);
      // Fill the pixel occupancy histogram.
      plot(point.rho(), "Pixels", 5.1, 40.1, 500);
    }
  }
  return StatusCode::SUCCESS;
}

