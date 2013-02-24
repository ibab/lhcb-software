// Gaudi
#include "GaudiKernel/AlgFactory.h"
// Local
#include "VLClusterMonitor.h"

/** @file VLClusterMonitor.cpp
 *
 *  Implementation of class : VLClusterMonitor
 *
 */

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY(VLClusterMonitor)

//=============================================================================
/// Standard constructor
//=============================================================================
VLClusterMonitor::VLClusterMonitor(const std::string& name,
                                   ISvcLocator* pSvcLocator) : 
    GaudiTupleAlg(name, pSvcLocator),
    m_clusters(0),
    m_nClusters(0.),
    m_nClusters2(0.),
    m_nOneStrip(0.),
    m_nTwoStrip(0.),
    m_nThreeStrip(0.),
    m_nFourStrip(0.),
    m_nFiveStrip(0.),
    m_nEvents(0) {

  declareProperty("Detailed",  m_detailed = false);

}

//=============================================================================
/// Initialization
//=============================================================================
StatusCode VLClusterMonitor::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc; 
  if (msgLevel(MSG::DEBUG)) debug() << " ==> initialize()" << endmsg;
  m_det = getDet<DeVL>(DeVLLocation::Default);
  setHistoTopDir("VL/");
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VLClusterMonitor::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> execute()" << endmsg;
  ++m_nEvents;
  m_clusters = getIfExists<VLClusters>(VLClusterLocation::Default);
  if (!m_clusters) {
    error() << "No clusters at " << VLClusterLocation::Default << endmsg;
    return StatusCode::FAILURE;
  } 
  monitor();
  return StatusCode::SUCCESS;

}

//=============================================================================
/// Finalize
//=============================================================================
StatusCode VLClusterMonitor::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> finalize()" << endmsg;
  double err = 0.;
  if (0 != m_nEvents) {
    m_nClusters /= m_nEvents;
    m_nClusters2 /= m_nEvents;
    err = sqrt((m_nClusters2 - (m_nClusters * m_nClusters)) / m_nEvents);
    m_nOneStrip /= m_nEvents;
    m_nTwoStrip /= m_nEvents;
    m_nThreeStrip /= m_nEvents;
    m_nFourStrip /= m_nEvents;
    m_nFiveStrip /= m_nEvents;
  }
  info() << "------------------------------------------------------" << endmsg;
  info() << "                    VLClusterMonitor                  " << endmsg;
  info() << "------------------------------------------------------" << endmsg;
  if (m_nClusters > 0) {
    info() << " Number of clusters / event: " << m_nClusters << " +/- " 
           << err << endmsg;
    info().precision(4);
    info() << "   1 strip clusters / event:     " << m_nOneStrip 
           << " (" << (m_nOneStrip / m_nClusters) * 100 << "%)"
           << endmsg;
    info().precision(4);
    info() << "   2 strip clusters / event:     " << m_nTwoStrip 
           << " (" << (m_nTwoStrip / m_nClusters) * 100 << "%)"
           << endmsg;
    info().precision(3);
    info() << "   3 strip clusters / event:     " << m_nThreeStrip 
           << " (" << (m_nThreeStrip / m_nClusters) * 100 << "%)"
           << endmsg;
    info().precision(3);
    info() << "   4 strip clusters / event:     " << m_nFourStrip 
           << " (" << (m_nFourStrip / m_nClusters) * 100 << "%)"
           << endmsg;
    info().precision(3);
    info() << "   5 strip clusters / event:     " << m_nFiveStrip 
           << " (" << (m_nFiveStrip / m_nClusters) * 100 << "%)"
           << endmsg;
  } else {
    info() << " ==> No VLClusters found! " << endmsg;
  }
  info() << "------------------------------------------------------" << endmsg;
  return GaudiAlgorithm::finalize();

}

void VLClusterMonitor::monitor() {

  if (msgLevel(MSG::DEBUG)) debug() << " ==> monitor()" << endmsg;
  const int size = m_clusters->size();
  m_nClusters += size;
  m_nClusters2 += size * size;
  plot(size, "nClusters", "Number of clusters / event", 0., 3000., 50);

  VLClusters::iterator it;
  for (it = m_clusters->begin(); it != m_clusters->end(); ++it) {
    int nstrips = (*it)->size();
    plot(nstrips, "clusterSize",
         "Number of strips in cluster",
         -0.5, 5.5, 6);
    switch (nstrips) {
      case 1: m_nOneStrip += 1.; break;
      case 2: m_nTwoStrip += 1.; break;
      case 3: m_nThreeStrip += 1.; break;
      case 4: m_nFourStrip += 1.; break;
      case 5: m_nFiveStrip += 1.; break;
      default : break;
    }
    if (!m_detailed) continue;
    VLChannelID channel = (*it)->channelID();
    const unsigned int sensor = channel.sensor();
    const unsigned int firstStrip = (*it)->strip(0);
    double adcSum = 0.;
    for (int i = 0; i < nstrips; ++i) {
      adcSum += double((*it)->adcValue(i));
    }
    plot(adcSum, "adcSum",
         "ADC sum",
         -0.5, 255.5, 256);
    plot((*it)->interStripFraction(), "isp",
         "Inter-strip fraction",
         0., 1., 50);
    plot2D(sensor, firstStrip, "sensorAndStrip",
           "Sensor and first strip number",
           0., 132., 0., 3000., 132, 50);
    if ((*it)->isRType()) {
      double r = m_det->rSensor(sensor)->rOfStrip(firstStrip);
      plot2D(r, nstrips, "radiusVsSize",
             "Cluster size as function of radius",
             0., 40., -0.5, 5.5, 40, 6);
      plot(adcSum, "adcSumR",
           "ADC sum",
           -0.5, 255.5, 256);
      plot((*it)->interStripFraction(), "ispR",
           "Inter-strip fraction",
           0., 1., 50);
    } else {
      plot(adcSum, "adcSumPhi",
           "ADC sum",
           -0.5, 255.5, 256);
      plot((*it)->interStripFraction(), "ispPhi",
           "Inter-strip fraction",
           0., 1., 50);
      unsigned int zone = m_det->phiSensor(sensor)->zoneOfStrip(firstStrip);
      if (0 == zone) {
        plot(nstrips, "clusterSizePhiZone0",
             "Number of strips in cluster (zone 0)",
             -0.5, 5.5, 6);
      } else if (1 == zone) {
        plot(nstrips, "clusterSizePhiZone1",
             "Number of strips in cluster (zone 1)",
             -0.5, 5.5, 6);
      } else if (2 == zone) {
        plot(nstrips, "clusterSizePhiZone2",
             "Number of strips in cluster (zone 2)",
             -0.5, 5.5, 6);
      }
    }
  }

}

