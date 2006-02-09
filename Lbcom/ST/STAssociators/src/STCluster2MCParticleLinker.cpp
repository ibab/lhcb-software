// $Id: STCluster2MCParticleLinker.cpp,v 1.3 2006-02-09 16:13:35 mneedham Exp $
// Include files 
#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Kernel/STDetSwitch.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "STCluster2MCParticleLinker.h"
#include "STTruthTool.h"

#include "Linker/LinkerTool.h"

#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : STCluster2MCParticleChi2Alg
//
// 25/04/2002 : M. Needham
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<STCluster2MCParticleLinker>          s_factory ;
const        IAlgFactory& STCluster2MCParticleLinkerFactory = s_factory ; 

STCluster2MCParticleLinker::STCluster2MCParticleLinker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  :GaudiAlgorithm (name,pSvcLocator) 
{

  // constructer
  declareProperty("InputData" , m_inputData = LHCb::STClusterLocation::TTClusters);
  declareProperty( "OutputData", m_outputData  = "TTCluster2MCParticles" ); 
  declareProperty( "addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.3);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("detType", m_detType = "TT");

}

STCluster2MCParticleLinker::~STCluster2MCParticleLinker() {
  // destructer
}; 

StatusCode STCluster2MCParticleLinker::initialize() {
 
  // get associator tool 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;

};

StatusCode STCluster2MCParticleLinker::execute() {

  // get STClusters
  LHCb::STClusters* clusterCont = get<LHCb::STClusters>(m_inputData);
 
  // get MCParticles
  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
 
  // create a linker
  LinkerWithKey<LHCb::MCParticle,LHCb::STCluster> aLinker( evtSvc(), msgSvc(), outputData() );

  // loop and link LHCb::STClusters to MC truth
  LHCb::STClusters::const_iterator iterClus =  clusterCont->begin();
  for( ; iterClus != clusterCont->end(); ++iterClus){

    // find all particles
    std::map<const LHCb::MCParticle*,double> partMap;
    associateToTruth(*iterClus,partMap);
     
    // total charge = cluster size due to norm
    double tCharge = (*iterClus)->size();

    // select references to add to table
    std::vector<partPair> selectedRefs;
    refsToRelate(selectedRefs,partMap,tCharge,mcParts);

    if (selectedRefs.empty() == false){

      if (m_oneRef == false){
        std::vector<partPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          aLinker.link(*iterClus,iterPair->first,iterPair->second);
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

StatusCode STCluster2MCParticleLinker::refsToRelate(std::vector<partPair>& selectedRefs,
                                            const std::map<const LHCb::MCParticle*,double>& partMap,
                                            const double totCharge,
                                            LHCb::MCParticles* particles) const{
  // iterate over map
  std::map<const LHCb::MCParticle*,double>::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    const LHCb::MCParticle* aParticle = iterMap->first;
    double frac = -1.0;
    if (totCharge > 0.0){
     frac = iterMap->second/totCharge;
    }
    if ((0 != aParticle)&&(frac>m_minFrac)){
      if ((m_addSpillOverHits == true)||(particles == aParticle->parent())){
	selectedRefs.push_back(std::make_pair(aParticle,frac));
      }
    }
    ++iterMap;
  } // iterMap
  
  return StatusCode::SUCCESS;
}

StatusCode STCluster2MCParticleLinker::associateToTruth(const LHCb::STCluster* aCluster,
                                                std::map<const LHCb::MCParticle*,double>& particleMap){
  // make link to truth  to LHCb::MCHit from cluster
  typedef LinkerTool<LHCb::STCluster, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  AsctTool associator(evtSvc(), m_inputData);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  double foundCharge = 0;
  Range hitsCont = aTable->relations(aCluster);
  iterator iterHit = hitsCont.begin();   
  for ( ; iterHit != hitsCont.end(); ++iterHit){
    const LHCb::MCParticle* aParticle = iterHit->to()->mcParticle();
    particleMap[aParticle] += iterHit->weight();
    foundCharge += iterHit->weight();
  } // iterHit

  // difference between depEnergy and total cluster charge is noise (due to norm)
  particleMap[0] += aCluster->size()-foundCharge;

  return StatusCode::SUCCESS;
}

 




