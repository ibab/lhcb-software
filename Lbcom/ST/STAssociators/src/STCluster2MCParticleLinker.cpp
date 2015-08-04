// Linker
#include "Linker/LinkerTool.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"


// local
#include "STCluster2MCParticleLinker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCluster2MCParticleLinker )

STCluster2MCParticleLinker::STCluster2MCParticleLinker(const std::string& name,
                                                       ISvcLocator* pSvcLocator)
  : ST::AlgBase (name,pSvcLocator)
{
  declareSTConfigProperty("ClusterASCTlocation" , m_asctLocation  , STClusterLocation::TTClusters + "2MCHits");
  declareSTConfigProperty("InputData" , m_inputData  , STClusterLocation::TTClusters);
  declareSTConfigProperty("OutputData", m_outputData , STClusterLocation::TTClusters);
  declareProperty("AddSpillOverHits", m_addSpillOverHits = false); 
  declareProperty("Minfrac", m_minFrac = 0.2);
  declareProperty("OneRef", m_oneRef = false);

  // register to be flipped IT/TT
  addToFlipList(&m_asctLocation);

}

STCluster2MCParticleLinker::~STCluster2MCParticleLinker()
{
  // destructer
} 

StatusCode STCluster2MCParticleLinker::execute()
{
  // get STClusters
  const STClusters* clusterCont = get<STClusters>(m_inputData);
 
  // get MCParticles
  MCParticles* mcParts = get<MCParticles>(MCParticleLocation::Default);
 
  // create a linker
  LinkerWithKey<MCParticle,STCluster> aLinker( evtSvc(), msgSvc(),outputData());

  // loop and link STClusters to MC truth
  STClusters::const_iterator iterClus =  clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){

    // find all particles
    ParticleMap partMap;
    StatusCode sc = associateToTruth(*iterClus,partMap);
    if (sc.isFailure()) return sc;

    // select references to add to table
    std::vector<PartPair> selectedRefs;
    refsToRelate(selectedRefs,partMap,mcParts);

    if (selectedRefs.empty() == false){

      if (m_oneRef == false){
        double tWeight = 0;
        std::vector<PartPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          aLinker.link(*iterClus,iterPair->first,iterPair->second);
          if (iterPair->first != 0) tWeight += iterPair->second;
          ++iterPair;
        } //iterPair
      }
      else {
        PartPair bestPair = selectedRefs.back();
        aLinker.link(*iterClus,bestPair.first,bestPair.second);
      } 
    } // refsToRelate != empty
  } // loop iterClus
  
  return StatusCode::SUCCESS;
}

void STCluster2MCParticleLinker::refsToRelate(std::vector<PartPair>& selRefs,
                                              const ParticleMap& partMap,
                                              MCParticles* particles ) const
{
  // iterate over map
  ParticleMap::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    const MCParticle* aParticle = iterMap->first;
    if ((0 != aParticle)&&(iterMap->second>m_minFrac)){
      if ((m_addSpillOverHits == true)||(particles == aParticle->parent())){
        selRefs.push_back(std::make_pair(aParticle,iterMap->second));
      }
    }
    ++iterMap;
  } // iterMap
  
  return;
}

StatusCode STCluster2MCParticleLinker::associateToTruth(const STCluster* 
                                                        aCluster,
                                                        ParticleMap& partMap)
{
  // make link to truth  to MCHit from cluster
  typedef LinkerTool<STCluster, MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  // Use the STCluster to MCHit association
  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  double foundCharge = 0;
  Range hitsCont = aTable->relations(aCluster);
  iterator iterHit = hitsCont.begin();   
  for ( ; iterHit != hitsCont.end(); ++iterHit){
    const MCParticle* aParticle = iterHit->to()->mcParticle();
    partMap[aParticle] += iterHit->weight();
    foundCharge += iterHit->weight();
  } // iterHit

  // difference between depEnergy and total cluster charge = noise (due to norm)
  partMap[0] += 1.0 - foundCharge;
 
  return StatusCode::SUCCESS;
}
