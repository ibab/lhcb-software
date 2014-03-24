// Gaudi
#include "GaudiKernel/AlgFactory.h"
// LHCb
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"
// Event/DigiEvent
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
// Event/MCEvent
#include "Event/MCTruth.h"
// Kernel/LHCbKernel
#include "Kernel/VPChannelID.h"
// Local
#include "VPClusterLinker.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPClusterLinker
//
//------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPClusterLinker)

VPClusterLinker::VPClusterLinker(const std::string& name,
                                 ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator) {


  declareProperty("ClusterLocation", m_clusterLocation = 
                  LHCb::VPClusterLocation::Default);
  declareProperty("DigitLocation", m_digitLocation =
                  LHCb::VPDigitLocation::Default);
  declareProperty("AsctLocation", m_asctLocation =
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
  // Get digits.
  const VPDigits* digits = getIfExists<VPDigits>(m_digitLocation);
  if (!digits) {
    error() << "No digits in " << m_digitLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Get the VPDigit to MCHit association table.
  LinkerTool<VPDigit, MCHit> associator(evtSvc(), m_asctLocation);
  const LinkerTool<VPDigit, MCHit>::DirectType* table = associator.direct();
  if (!table) {
    error() << "Cannot find association table " << m_asctLocation << endmsg;
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
    std::map<const LHCb::MCParticle*, double> particleMap;
    // Get the pixels in the cluster.
    std::vector<VPChannelID> pixels = (*itc)->pixels();
    double sum = 0.;
    std::vector<VPChannelID>::iterator itp;
    for (itp = pixels.begin(); itp != pixels.end(); ++itp) {
      VPDigit* digit = digits->object(*itp);
      if (!digit) continue;
      LinkerTool<VPDigit, MCHit>::DirectType::Range hits = table->relations(digit);
      LinkerTool<VPDigit, MCHit>::DirectType::iterator ith;
      for (ith = hits.begin(); ith != hits.end(); ++ith) {
        const MCHit* hit = ith->to();
        if (!hit) continue;
        const double weight = ith->weight();
        hitMap[hit] += weight;
        sum += weight;
        const MCParticle* particle = hit->mcParticle();
        if (!particle) continue;
        particleMap[particle] += weight;
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

