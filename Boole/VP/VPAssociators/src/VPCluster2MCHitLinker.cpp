// $Id: VPCluster2MCHitLinker.cpp,v 1.3 2010-02-25 12:15:09 marcin Exp $
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Linker
#include "Linker/LinkerWithKey.h"
#include "Linker/LinkerTool.h"
// Event
#include "Event/VPCluster.h"
#include "Event/VPLiteCluster.h"
#include "Event/MCTruth.h"
// LHCbKernel
#include "Kernel/VPChannelID.h"
// Local
#include "VPCluster2MCHitLinker.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPCluster2MCHitLinker
//
// 01/12/2009 : Marcin Kucharczyk
// Based on ST code
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPCluster2MCHitLinker);

VPCluster2MCHitLinker::VPCluster2MCHitLinker(const std::string& name,
                            ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm(name, pSvcLocator),
    m_hitLocation("MC/VP/Hits"),
    m_asctLocation(LHCb::VPDigitLocation::VPDigitLocation + "2MCHits")
{
  declareProperty("InputData", m_inputData = 
                  LHCb::VPClusterLocation::VPClusterLocation);
  declareProperty("OutputData", m_outputData = 
                  LHCb::VPClusterLocation::VPClusterLocation + "2MCHits");
  declareProperty("AddSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("MinFraction", m_minFrac = 0.2);
  declareProperty("OneRef",m_oneRef = false);
  declareProperty("DigitLocation", m_digitLocation =
                  LHCb::VPDigitLocation::VPDigitLocation );
  declareProperty("Conversion", m_conversion = 2100.0);
  declareProperty("ScaleFactor", m_scaleFactor = 2.0);
}

//=============================================================================
// Destructor
//=============================================================================
VPCluster2MCHitLinker::~VPCluster2MCHitLinker() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPCluster2MCHitLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  if(sc) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPCluster2MCHitLinker::execute() 
{
  // Get STClusters
  const VPClusters* clusterCont = get<VPClusters>(m_inputData);
  // Get digits
  m_digitCont = get<VPDigits>(m_digitLocation);
  // Get MCHits
  MCHits* mcHits = get<MCHits>("/Event/"+m_hitLocation);
  // Create linker
  LinkerWithKey<MCHit,VPCluster> myLink(evtSvc(),msgSvc(),outputData());
  // Loop and link VPClusters to MC truth
  VPClusters::const_iterator iterClus = clusterCont->begin();
  for(; iterClus != clusterCont->end(); ++iterClus) {        // loop over clusters
    // Find all hits
    HitMap hitMap;
    StatusCode sc = associateToTruth(*iterClus,hitMap);
    if(sc.isFailure()) return sc;
    // Select references to add to table
    std::vector<HitPair> selectedRefs;
    refsToRelate(selectedRefs,hitMap,mcHits);
    if(selectedRefs.empty() == false) {
      if(m_oneRef == false) {
        std::vector<HitPair>::iterator iterPair = selectedRefs.begin();
        while(iterPair != selectedRefs.end()) { 
          myLink.link(*iterClus,iterPair->first,iterPair->second);
          ++iterPair;
        }
      }
      else{
        HitPair bestPair = selectedRefs.back();
        myLink.link(*iterClus,bestPair.first,bestPair.second);
      }
    } 
  }

  return StatusCode::SUCCESS;
};


//============================================================================
// Reference to related hits
//============================================================================
void VPCluster2MCHitLinker::refsToRelate(std::vector<HitPair>& selRefs,
                                              const HitMap& hitMap,
                                              MCHits* hits) const
{
  // Iterate over map
  HitMap::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()) {
    const MCHit* aHit = iterMap->first;
    if((0 != aHit) && (iterMap->second > m_minFrac)) {
      if((m_addSpillOverHits == true) || (hits == aHit->parent())) {
        selRefs.push_back(std::make_pair(aHit,iterMap->second));
      }
    }
    ++iterMap;
  }
  return;
}

//============================================================================
// Associate to MC truth
//============================================================================
StatusCode VPCluster2MCHitLinker::associateToTruth(
                                       const VPCluster* aCluster,
                                       HitMap& hitMap)
{
  // Make link to truth  to MCHit from cluster
  typedef LinkerTool<VPDigit, MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  // Use the VPDigit to MCHit association
  AsctTool associator(evtSvc(),m_asctLocation);
  const Table* aTable = associator.direct();
  if(!aTable) return Error("Failed to find " + m_asctLocation + " table",
                           StatusCode::FAILURE);
  std::vector< std::pair<LHCb::VPChannelID,int> > tdcVec = 
                                                       aCluster->pixelHitVec();
  std::vector<VPChannelID> chanVector;
  for(std::vector< std::pair<LHCb::VPChannelID,int> >::iterator 
      ip = tdcVec.begin(); ip != tdcVec.end(); ip++) {
    std::pair<LHCb::VPChannelID,int> pair = *ip;
    chanVector.push_back(pair.first);
  }
  std::vector<VPChannelID>::iterator iterChan = chanVector.begin();
  while(iterChan != chanVector.end()) {
    VPDigit* aDigit = m_digitCont->object(*iterChan);
    double foundCharge = 0.0;
    if(aDigit !=0) {
      Range hitsCont = aTable->relations(aDigit);
      iterator iterHit = hitsCont.begin();   
      for(; iterHit != hitsCont.end(); ++iterHit) {
        const MCHit* aHit = iterHit->to();
        hitMap[aHit] += iterHit->weight() * aDigit->ToTValue() * m_conversion;
        foundCharge += iterHit->weight() * aDigit->ToTValue() * m_conversion;
      }
      hitMap[0] += aDigit->ToTValue() * m_conversion - foundCharge;
    }
    ++iterChan;
  }
  // Renormalize to 1
  HitMap::iterator iterMap = hitMap.begin();
  for(; iterMap != hitMap.end() ; ++iterMap) {
    VPLiteCluster lc = aCluster->lCluster();
    iterMap->second /= lc.clustToT() * m_scaleFactor * m_conversion;
  }

  return StatusCode::SUCCESS;
}


//============================================================================
StatusCode VPCluster2MCHitLinker::finalize() {

  return GaudiAlgorithm::finalize();
}
