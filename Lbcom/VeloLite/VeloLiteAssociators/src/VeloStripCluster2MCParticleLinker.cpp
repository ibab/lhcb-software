// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"
// Event/DigiEvent
#include "Event/VeloStripCluster.h"
// Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"
// Linkers
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"
// Local
#include "VeloStripCluster2MCParticleLinker.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloStripCluster2MCParticleLinker);

//=============================================================================
/// Standard constructor
//=============================================================================
VeloStripCluster2MCParticleLinker::VeloStripCluster2MCParticleLinker(const std::string& name,
                                                                     ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator),
    m_asctName(LHCb::VeloStripClusterLocation::Default),
    m_inputClusters(LHCb::VeloStripClusterLocation::Default),
    m_inputParticles(LHCb::MCParticleLocation::Default) {

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VeloStripCluster2MCParticleLinker::execute() {

  const bool m_debug = msgLevel(MSG::DEBUG);
  if (m_debug) debug() << " ==> execute()" << endmsg;
  // Get clusters
  if (!exist<LHCb::VeloStripClusters>(m_inputClusters)) {
    error() << "There are no VeloStripClusters at " 
            << m_inputClusters << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::VeloStripClusters* clusters = get<LHCb::VeloStripClusters>(m_inputClusters);
  if (m_debug) debug()<< clusters->size() << " clusters" << endmsg;    
  // Get MC particles
  if (!exist<LHCb::MCParticles>(m_inputParticles)) {
    error() << "There are no MCParticles at " 
            << m_inputParticles << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::MCParticles* particles = get<LHCb::MCParticles>(m_inputParticles);
  if (m_debug) debug() << particles->size() << " particles" << endmsg;
  // Linker table 
  LinkerWithKey<LHCb::MCParticle, LHCb::VeloStripCluster> myLink(evtSvc(), 
                                                                 msgSvc(), 
                                                                 asctName());
  std::map<const LHCb::MCParticle*, double> relations;
  typedef LinkerTool<LHCb::VeloStripCluster, LHCb::MCHit> asctTool;
  typedef asctTool::DirectType Table;

  asctTool associator(evtSvc(), LHCb::VeloStripClusterLocation::Default + "2MCHits");
  const Table* table = associator.direct();
  if (!table) {
    error() << "Empty table with associations " << endmsg;
    return StatusCode::FAILURE;
  }
  LHCb::VeloStripClusters::const_iterator itc;
  for (itc = clusters->begin(); itc != clusters->end(); ++itc) {    
    Table::Range range = table->relations(*itc);
    Table::iterator ita;
    // Make associations
    if (m_debug) debug() << "table: " << range.size() << endmsg;
    for (ita = range.begin(); ita != range.end(); ++ita) {
      const LHCb::MCHit* hit = ita->to();
      const LHCb::MCParticle* particle = hit->mcParticle();
      if (m_debug) {
        debug() << "energy: " 
                << particle->momentum().e() / Gaudi::Units::GeV
                << " [GeV]" << endmsg;
      }
      relations[particle] += ita->weight();
    }
    // Linker table
    if (!relations.empty()) {
      if (m_debug) debug() << "Make links" << endmsg;
      std::map<const LHCb::MCParticle*, double>::const_iterator itr;
      for (itr = relations.begin(); itr != relations.end(); ++itr) {
        SmartRef<LHCb::MCParticle> myPart = (*itr).first;
        double weight = (*itr).second;
        myLink.link((*itc), myPart, weight);
      }
    } else {
      if (m_debug) debug() << "No associations made!" << endmsg;
    }
    relations.clear();
  }
  return StatusCode::SUCCESS;

}
