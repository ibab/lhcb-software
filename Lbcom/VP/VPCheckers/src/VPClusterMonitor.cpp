// Event/LinkerEvent
#include "Linker/LinkedTo.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
// Local
#include "VPClusterMonitor.h"

DECLARE_ALGORITHM_FACTORY(VPClusterMonitor)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClusterMonitor::VPClusterMonitor(const std::string& name,
                                   ISvcLocator* pSvcLocator) :
    GaudiHistoAlg(name, pSvcLocator),
    m_det(nullptr) {

  declareProperty("ClusterLocation", 
                  m_clusterLocation = LHCb::VPClusterLocation::Default);
  declareProperty("LinkedHitsLocation", 
                  m_linkedHitsLocation = LHCb::VPClusterLocation::Default + "2MCHits");

}

//=============================================================================
// Destructor
//=============================================================================
VPClusterMonitor::~VPClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPClusterMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); 
  if (sc.isFailure()) return sc;

  // Get detector element
  m_det = getDetIfExists<DeVP>(DeVPLocation::Default);
  if (!m_det) {
    return Error("No detector element at " + DeVPLocation::Default);
  }
  setHistoTopDir("VP/");
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPClusterMonitor::execute() {

  // Get the clusters.
  const LHCb::VPClusters* clusters = getIfExists<LHCb::VPClusters>(m_clusterLocation);
  if (!clusters) {
    return Error("No clusters in " + m_clusterLocation);
  }
  if (UNLIKELY(msgLevel(MSG::DEBUG))) {
    debug() << "Found " << clusters->size() << " clusters" << endmsg;
  }
  // Get the linker table.
  auto links = LinkedTo<LHCb::MCHit, LHCb::VPCluster>(evtSvc(), msgSvc(), 
                                                      m_linkedHitsLocation);
  // Loop over the clusters. 
  for (const LHCb::VPCluster* cluster : *clusters) {
    // Get sensor
    const DeVPSensor* sensor = m_det->sensorOfChannel(cluster->channelID());
    std::vector<LHCb::VPChannelID> pixels = cluster->pixels();
    // Loop over the pixel hits.
    for (auto it = pixels.cbegin(), end = pixels.cend(); it != end; ++it) {
      // Get the pixel position in the global frame and plot it.
      Gaudi::XYZPoint pointGlobal = sensor->channelToPoint(*it, false);
      plot(pointGlobal.rho(), "PixelRadius", 0, 100, 200);
      plot2D(pointGlobal.x(), pointGlobal.y(), "PixelXY", -100, 100, -100, 100, 400, 400);
      plot2D(pointGlobal.z(), pointGlobal.x(), "PixelZX", -500, 1000, -100, 100, 1000, 400);
      plot2D(pointGlobal.z(), pointGlobal.y(), "PixelZY", -500, 1000, -100, 100, 1000, 400);
    }
    const unsigned int nPixels = pixels.size();
    // Get the global cluster position. 
    const double x = cluster->x();
    const double y = cluster->y();
    const double z = cluster->z();
    Gaudi::XYZPoint pGlobal(x, y, z);
    const double rho = pGlobal.rho();
    plot(nPixels, "ClusterSize", 0, 100, 100);
    plot(rho, "ClusterRadius", 0, 100, 200);
    plot2D(rho, nPixels, "ClusterSizeVsRadius", 0, 100, 0, 100, 200, 100);
    plot2D(x, y, "ClusterXY", -100, 100, -100, 100, 400, 400);
    plot2D(z, x, "ClusterZX", -500, 1000, -100, 100, 1000, 400);
    plot2D(z, y, "ClusterZY", -500, 1000, -100, 100, 1000, 400);
    plot(cluster->fraction().first, "InterpixelFractionX", -0.1, 1.1, 120);
    plot(cluster->fraction().second, "InterpixelFractionY", -0.1, 1.1, 120);

    // Get MC hit for this cluster and plot residuals
    const LHCb::MCHit* hit = links.first(cluster->channelID());
    if (!hit) continue;
    // Get true track direction for this hit
    const double yangle = atan(hit->dydz()) / Gaudi::Units::degree;
    const double xangle = atan(hit->dxdz()) / Gaudi::Units::degree;
    const double theta = sqrt(xangle * xangle + yangle * yangle);
    plot(theta, "TrackTheta", 0., 50., 100);
    plot(xangle, "TrackAngleX", -30., 30., 120);
    plot(yangle, "TrackAngleY", -30., 30., 120);
    // Get hit position.
    const Gaudi::XYZPoint mchitPoint = hit->midPoint();
    // Calculate the residuals.
    const double dx = x - mchitPoint.x();
    const double dy = y - mchitPoint.y();
    const double dz = z - mchitPoint.z();
    const double d3 = sqrt(dx * dx + dy * dy + dz * dz);
    // Plot the residuals.
    plot(dx, "ResidualsX", -0.2, 0.2, 4000);
    plot(dy, "ResidualsY", -0.2, 0.2, 4000);
    plot(d3, "Residuals3d", -0.2, 0.2, 4000);
    if (1 == nPixels) {
      plot(dx, "ResidualsX1", -0.2, 0.2, 4000);
      plot(dy, "ResidualsY1", -0.2, 0.2, 4000);
    } else if (2 == nPixels) {
      plot(dx, "ResidualsX2", -0.2, 0.2, 4000);
      plot(dy, "ResidualsY2", -0.2, 0.2, 4000);
    }
    plot2D(theta, d3, "Residuals3dVsTrackTheta", 0., 50., -0.2, 0.2, 100, 400);
    plot2D(rho, d3, "Residuals3dVsRadius", 0., 100, -0.2, 0.2, 200, 400);
    plot2D(xangle, dx, "ResidualsXVsTrackAngleX", -30., 30., -0.2, 0.2, 120, 400);
    plot2D(xangle, dy, "ResidualsYVsTrackAngleX", -30., 30., -0.2, 0.2, 120, 400);
    plot2D(xangle, nPixels, "ClusterSizeVsTrackAngleX", -30., 30., 0, 20, 120, 20);
    // Plot the same for small y angles
    if (yangle < 2 && yangle > -2) {
      plot2D(xangle, dx, "ResidualsXVsTrackAngleXSmallY", -30., 30., -0.2, 0.2, 120, 400);
      plot2D(xangle, dy, "ResidualsYVsTrackAngleXSmallY", -30., 30., -0.2, 0.2, 120, 400);
      plot2D(xangle, nPixels, "ClusterSizeVsTrackAngleXSmallY", -30., 30., 0, 20, 120, 20);
    }      
  }
  return StatusCode::SUCCESS;
}

