// LHCb
// Event/MCEvent
#include "Event/MCHit.h"
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkedTo.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
#include "Event/VPDigit.h"
// Local
#include "VPClusterLinker.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPClusterLinker
//
//------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPClusterLinker)

//=============================================================================
// Constructor
//=============================================================================
VPClusterLinker::VPClusterLinker(const std::string& name,
                                 ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator) {


  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::Default);
  declareProperty("InputLocation", m_inputLocation =
                  LHCb::VPDigitLocation::Default + "2MCHits");
  declareProperty("HitLinkLocation", m_hitLinkLocation = 
                  LHCb::VPClusterLocation::Default + "2MCHits");
  declareProperty("ParticleLinkLocation", m_particleLinkLocation = 
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
  const VPClusters* clusters = getIfExists<VPClusters>(m_clusterLocation);
  if (!clusters) {
    error() << "No clusters in " << m_clusterLocation << endmsg;
    return StatusCode::FAILURE;
  }

  // Get the association table between digits and MCHits.
  LinkedTo<MCHit> associator(evtSvc(), msgSvc(), m_inputLocation);
  if (associator.notFound()) {
    error() << "Cannot find association table " << m_inputLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Create association tables for clusters. 
  LinkerWithKey<MCHit, VPCluster> hitLinker(evtSvc(), msgSvc(), 
                                            m_hitLinkLocation);
  LinkerWithKey<MCParticle, VPCluster> particleLinker(evtSvc(), msgSvc(), 
                                                      m_particleLinkLocation);
  // Loop over clusters.
  VPClusters::const_iterator itc;
  for (itc = clusters->begin(); itc != clusters->end(); ++itc) {
    std::map<const MCHit*, double> hitMap;
    std::map<const MCParticle*, double> particleMap;
    // Get the pixels in the cluster.
    std::vector<VPChannelID> pixels = (*itc)->pixels();
    double sum = 0.;
    std::vector<VPChannelID>::iterator itp;
    for (itp = pixels.begin(); itp != pixels.end(); ++itp) {
      LHCb::MCHit* hit = associator.first(*itp);
      while (hit) {
        const double weight = associator.weight();
        hitMap[hit] += weight;
        sum += weight;
        const MCParticle* particle = hit->mcParticle();
        if (!particle) continue;
        particleMap[particle] += weight;
        hit = associator.next();
      }
    }
    if (sum < 1.e-2) continue;
    for (std::map<const MCHit*, double>::iterator itm = hitMap.begin(); 
         itm != hitMap.end(); ++itm) {
      const MCHit* hit = (*itm).first;
      const double weight = (*itm).second / sum;
      hitLinker.link(*itc, hit, weight);
    }
    for (std::map<const MCParticle*, double>::iterator itm = particleMap.begin();
         itm != particleMap.end(); ++itm) {
      const MCParticle* particle = (*itm).first;
      const double weight = (*itm).second / sum;
      particleLinker.link(*itc, particle, weight);
    }
  }
  return StatusCode::SUCCESS;
}

