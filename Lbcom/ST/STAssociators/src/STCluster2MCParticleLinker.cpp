// $Id: STCluster2MCParticleLinker.cpp,v 1.9 2007-01-09 15:04:59 jvantilb Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerTool.h"

// Event
#include "Event/STCluster.h"
#include "Event/MCHit.h"
#include "Event/MCParticle.h"
#include "Kernel/STDetSwitch.h"

// local
#include "STCluster2MCParticleLinker.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STCluster2MCParticleLinker );

STCluster2MCParticleLinker::STCluster2MCParticleLinker(const std::string& name,
                                                       ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  declareProperty("InputData" , m_inputData  = STClusterLocation::TTClusters);
  declareProperty("OutputData", m_outputData = STClusterLocation::TTClusters);
  declareProperty("addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.2);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("detType", m_detType = "TT");
}

STCluster2MCParticleLinker::~STCluster2MCParticleLinker()
{
  // destructer
} 

StatusCode STCluster2MCParticleLinker::initialize()
{ 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_asctLocation = STClusterLocation::TTClusters + "2MCHits";

  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);
  STDetSwitch::flip(m_detType,m_asctLocation);

  return StatusCode::SUCCESS;
};

StatusCode STCluster2MCParticleLinker::execute()
{
  // get STClusters
  STClusters* clusterCont = get<STClusters>(m_inputData);
 
  // get MCParticles
  MCParticles* mcParts = get<MCParticles>(MCParticleLocation::Default);
 
  // create a linker
  LinkerWithKey<MCParticle,STCluster> aLinker( evtSvc(), msgSvc(),outputData());

  // loop and link STClusters to MC truth
  STClusters::const_iterator iterClus =  clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){

    // find all particles
    std::map<const MCParticle*,double> partMap;
    associateToTruth(*iterClus,partMap);
     
    // select references to add to table
    std::vector<partPair> selectedRefs;
    refsToRelate(selectedRefs,partMap,mcParts);

    if (selectedRefs.empty() == false){

      if (m_oneRef == false){
        double tWeight = 0;
        std::vector<partPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          aLinker.link(*iterClus,iterPair->first,iterPair->second);
          if (iterPair->first != 0) tWeight += iterPair->second;
          ++iterPair;
        } //iterPair
      }
      else {
        partPair bestPair = selectedRefs.back();
        aLinker.link(*iterClus,bestPair.first,bestPair.second);
      } 
    } // refsToRelate != empty
  } // loop iterClus
  
  return StatusCode::SUCCESS;
};

StatusCode 
STCluster2MCParticleLinker::refsToRelate(std::vector<partPair>& selectedRefs,
                                         const std::map<const MCParticle*,
                                         double>& partMap,
                                         MCParticles* particles ) const
{
  // iterate over map
  std::map<const MCParticle*,double>::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    const MCParticle* aParticle = iterMap->first;
    if ((0 != aParticle)&&(iterMap->second>m_minFrac)){
      if ((m_addSpillOverHits == true)||(particles == aParticle->parent())){
        selectedRefs.push_back(std::make_pair(aParticle,iterMap->second));
      }
    }
    ++iterMap;
  } // iterMap
  
  return StatusCode::SUCCESS;
}

StatusCode STCluster2MCParticleLinker::associateToTruth(const STCluster* 
                                                        aCluster,
                                std::map<const MCParticle*,double>& particleMap)
{
  // make link to truth  to MCHit from cluster
  typedef LinkerTool<STCluster, MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  AsctTool associator(evtSvc(), m_asctLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  double foundCharge = 0;
  Range hitsCont = aTable->relations(aCluster);
  iterator iterHit = hitsCont.begin();   
  for ( ; iterHit != hitsCont.end(); ++iterHit){
    const MCParticle* aParticle = iterHit->to()->mcParticle();
    particleMap[aParticle] += iterHit->weight();
    foundCharge += iterHit->weight();
  } // iterHit

  // difference between depEnergy and total cluster charge = noise (due to norm)
  particleMap[0] += 1.0 - foundCharge;
 
  return StatusCode::SUCCESS;
}
