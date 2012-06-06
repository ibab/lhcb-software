// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// Event/DigiEvent 
#include "Event/VeloStripCluster.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCVeloLiteDigit.h"
// Linker
#include "Linker/LinkerWithKey.h"
// Local
#include "VeloStripCluster2MCHitLinker.h"
#include "VeloLiteTruthTool.h"

DECLARE_ALGORITHM_FACTORY(VeloStripCluster2MCHitLinker);

//=============================================================================
/// Standard constructor
//=============================================================================
VeloStripCluster2MCHitLinker::VeloStripCluster2MCHitLinker(const std::string& name,
                                                           ISvcLocator* pSvcLocator) : 
    GaudiAlgorithm(name, pSvcLocator),
    m_asctName(LHCb::VeloStripClusterLocation::Default + "2MCHits"),
    m_inputClusters(LHCb::VeloStripClusterLocation::Default),
    m_inputDigits(LHCb::MCVeloLiteDigitLocation::Default) {

}

//=============================================================================
/// Main execution
//=============================================================================
StatusCode VeloStripCluster2MCHitLinker::execute() {

  debug() << " ==> execute()" << endmsg;
  // Get clusters
  LHCb::VeloStripClusters* clusters;
  if (!exist<LHCb::VeloStripClusters>(m_inputClusters)) {
    error() << "There are no VeloStripClusters at " 
            << m_inputClusters << endmsg;
    return StatusCode::FAILURE;
  }
  clusters = get<LHCb::VeloStripClusters>(m_inputClusters);
  // Get digits 
  LHCb::MCVeloLiteDigits* digits;
  if (!exist<LHCb::MCVeloLiteDigits>(m_inputDigits)) {
    error() << "There are no MCVeloLiteDigits at " 
            << m_inputDigits << endmsg;
    return StatusCode::FAILURE;
  }
  digits = get<LHCb::MCVeloLiteDigits>(m_inputDigits);

  LinkerWithKey<LHCb::MCHit, LHCb::VeloStripCluster> myLink(evtSvc(), 
                                                            msgSvc(), 
                                                            asctName());
  // Link the clusters to MCHits
  LHCb::VeloStripClusters::const_iterator itc;
  for (itc = clusters->begin(); itc != clusters->end(); ++itc) {
    std::map<LHCb::MCHit*, double> relations;
    StatusCode sc = VeloLiteTruthTool::associateToTruth((*itc), relations, digits);    
    if (sc) {
      debug() << "After the associator " << endmsg;
      std::map<LHCb::MCHit*, double>::iterator itr;
      for (itr = relations.begin(); itr != relations.end(); ++itr) {
        SmartRef<LHCb::MCHit> hit = (*itr).first;
        debug() << "Hit's mid point: " << hit->midPoint() << endmsg;
        // Relation weight
        double weight = (*itr).second;
        // Linker table
        myLink.link((*itc), hit, weight);
      }
    } else {
      debug() << "Empty linker table!" << endmsg;
    }
  }
  return StatusCode::SUCCESS;

}
