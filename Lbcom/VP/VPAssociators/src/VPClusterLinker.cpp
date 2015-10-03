// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"
// Event/DigiEvent
#include "Event/VPDigit.h"

// Local
#include "VPClusterLinker.h"

DECLARE_ALGORITHM_FACTORY(VPClusterLinker)

//=============================================================================
// Constructor
//=============================================================================
VPClusterLinker::VPClusterLinker(const std::string& name,
                                 ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator) {

  declareProperty("LinkParticles", m_linkParticles = true);
  declareProperty("LinkHits", m_linkHits = false);

  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::Default);
  declareProperty("InputLocation", m_inputLocation =
                  LHCb::VPDigitLocation::Default);
  declareProperty("OutputLocation", m_outputLocation = 
                  LHCb::VPClusterLocation::Default);

}

//=============================================================================
// Destructor
//=============================================================================
VPClusterLinker::~VPClusterLinker() {}

//=============================================================================
// Execution
//=============================================================================
StatusCode VPClusterLinker::execute() {

  // Get clusters.
  const auto clusters = getIfExists<LHCb::VPClusters>(m_clusterLocation);
  if (!clusters) {
    return Error("No clusters in " + m_clusterLocation, StatusCode::FAILURE);
  }

  if (m_linkParticles) {
    if (!linkParticles(clusters)) return StatusCode::FAILURE;
  }
  if (m_linkHits) {
    if (!linkHits(clusters)) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Link clusters and MCHits
//=============================================================================
bool VPClusterLinker::linkHits(const LHCb::VPClusters* clusters) {

  // Get the association table between digits and hits.
  const std::string inputLocation = m_inputLocation + "2MCHits";
  LinkedTo<LHCb::MCHit> associator(evtSvc(), msgSvc(), inputLocation);
  if (associator.notFound()) {
    error() << "Cannot find association table " << inputLocation << endmsg; 
    return false;
  }

  // Create an association table between hits and clusters. 
  const std::string outputLocation = m_outputLocation + "2MCHits";
  LinkerWithKey<LHCb::MCHit, LHCb::VPCluster> linker(evtSvc(), msgSvc(), 
                                                     outputLocation);

  // Loop over the clusters.
  for (const LHCb::VPCluster* cluster : *clusters) {
    std::map<const LHCb::MCHit*, double> hitMap;
    // Get the pixels in the cluster.
    auto pixels = cluster->pixels();
    double sum = 0.;
    for (auto itp = pixels.cbegin(), end = pixels.cend(); itp != end; ++itp) {
      LHCb::MCHit* hit = associator.first(*itp);
      while (hit) {
        const double weight = associator.weight();
        hitMap[hit] += weight;
        sum += weight;
        hit = associator.next();
      }
    }
    if (sum < 1.e-2) continue;
    for (auto itm = hitMap.begin(), end = hitMap.end(); itm != end; ++itm) {
      const LHCb::MCHit* hit = (*itm).first;
      const double weight = (*itm).second / sum;
      linker.link(cluster, hit, weight);
    }
  }
  return true;
}

//=============================================================================
// Link clusters and MCParticles
//=============================================================================
bool VPClusterLinker::linkParticles(const LHCb::VPClusters* clusters) {

  // Get the association table between digits and particles.
  LinkedTo<LHCb::MCParticle> associator(evtSvc(), msgSvc(), m_inputLocation);
  if (associator.notFound()) {
    error() << "Cannot find association table " << m_inputLocation << endmsg; 
    return false;
  }
  // Create an association table between particles and clusters. 
  LinkerWithKey<LHCb::MCParticle, LHCb::VPCluster> linker(evtSvc(), msgSvc(), 
                                                          m_outputLocation);

  // Loop over the clusters.
  for (const LHCb::VPCluster* cluster : *clusters) {
    std::map<const LHCb::MCParticle*, double> particleMap;
    // Get the pixels in the cluster.
    auto pixels = cluster->pixels();
    double sum = 0.;
    for (auto itp = pixels.cbegin(), end = pixels.cend(); itp != end; ++itp) {
      LHCb::MCParticle* particle = associator.first(*itp);
      while (particle) {
        const double weight = associator.weight();
        particleMap[particle] += weight;
        sum += weight;
        particle = associator.next();
      }
    }
    if (sum < 1.e-2) continue;
    for (auto itm = particleMap.begin(), end = particleMap.end(); itm != end; ++itm) {
      const LHCb::MCParticle* particle = (*itm).first;
      const double weight = (*itm).second / sum;
      linker.link(cluster, particle, weight);
    }
  }
  return true;
}

