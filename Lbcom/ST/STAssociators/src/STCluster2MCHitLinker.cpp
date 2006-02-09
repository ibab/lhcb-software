// $Id: STCluster2MCHitLinker.cpp,v 1.4 2006-02-09 16:13:35 mneedham Exp $
// Include files 

#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/MCTruth.h"

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// linker
#include "Linker/LinkerTool.h"


#include "Kernel/STDetSwitch.h"

#include "STTruthTool.h"
#include "STCluster2MCHitLinker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STCluster2MCHitLinkerChi2Alg
//
// 25/04/2002 : M. Needham
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<STCluster2MCHitLinker>          s_factory ;
const        IAlgFactory& STCluster2MCHitLinkerFactory = s_factory ; 

STCluster2MCHitLinker::STCluster2MCHitLinker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructer
  declareProperty("OutputData", m_outputData  ="TTCluster2MCHits");
  declareProperty("addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.3);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("digitLocation", m_digitLocation = LHCb::STDigitLocation::TTDigits );
  declareProperty("detType", m_detType = "TT");

}

STCluster2MCHitLinker::~STCluster2MCHitLinker() {
  // destructer
}; 

StatusCode STCluster2MCHitLinker::initialize() {
  
  // get associator tool 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_hitLocation = LHCb::MCHitLocation::TT;
  m_inputData = LHCb::STClusterLocation::TTClusters;

  STDetSwitch::flip(m_detType,m_hitLocation);
  STDetSwitch::flip(m_detType,m_digitLocation);
  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;
};

StatusCode STCluster2MCHitLinker::execute() {

  // get STClusters
  LHCb::STClusters* clusterCont = get<LHCb::STClusters>(m_inputData);

  // get the digits
  m_digitCont = get<LHCb::STDigits>(m_digitLocation);

  // get MCParticles
  LHCb::MCHits* mcHits = get<LHCb::MCHits>("/Event/"+m_hitLocation);

  // create a linker
  LinkerWithKey<LHCb::MCHit,LHCb::STCluster> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STClusters to MC truth
  LHCb::STClusters::const_iterator iterClus;
  for(iterClus = clusterCont->begin(); 
      iterClus != clusterCont->end(); ++iterClus){

    // find all hits
    std::map<const LHCb::MCHit*,double> hitMap;
    associateToTruth(*iterClus,hitMap);
     
    // total charge = cluster size due to norm
    double tCharge = (*iterClus)->size();

    // select references to add to table
    std::vector<hitPair> selectedRefs;
    refsToRelate(selectedRefs,hitMap,tCharge,mcHits);

    if (selectedRefs.empty() == false){
      if (m_oneRef == false ){
        std::vector<hitPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){ 
          myLink.link(*iterClus,iterPair->first,iterPair->second);
          ++iterPair;
        } //iterPair
      }
      else{
        hitPair bestPair = selectedRefs.back();
        myLink.link(*iterClus,bestPair.first,bestPair.second);
      }
    } // refsToRelate ! empty

  } // loop iterClus

  return StatusCode::SUCCESS;
};

StatusCode STCluster2MCHitLinker::refsToRelate(std::vector<hitPair>& selectedRefs,
                                            const std::map<const LHCb::MCHit*,double>& hitMap,
                                            const double& totCharge,
                                            LHCb::MCHits* hits) const{
  // iterate over map
  std::map<const LHCb::MCHit*,double>::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()){
    const LHCb::MCHit* aHit = iterMap->first;
    double frac = -1.0;
    if (totCharge >0.0){
      frac = fabs(iterMap->second/totCharge);
    }
    if ((0 != aHit)&&(frac>m_minFrac)){
      if ((m_addSpillOverHits == true)||(hits == aHit->parent())){
	selectedRefs.push_back(std::make_pair(aHit,frac));
      }
    }
    ++iterMap;
  } // iterMap
  
  return StatusCode::SUCCESS;
}

StatusCode STCluster2MCHitLinker::associateToTruth(const LHCb::STCluster* aCluster,
                                                std::map<const LHCb::MCHit*,double>& hitMap){

  // make link to truth  to MCHit from cluster
  typedef LinkerTool<LHCb::STDigit, LHCb::MCHit> AsctTool;
  typedef AsctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;

  AsctTool associator(evtSvc(), m_digitLocation);
  const Table* aTable = associator.direct();
  if (!aTable) return Error("Failed to find table", StatusCode::FAILURE);

  double foundCharge = 0.;

  std::vector<LHCb::STChannelID> chanVector = aCluster->channels();
  std::vector<LHCb::STChannelID>::iterator iterChan = chanVector.begin();
  while (iterChan != chanVector.end()){

    LHCb::STDigit* aDigit = m_digitCont->object(*iterChan);
    if (aDigit !=0){
      Range hitsCont = aTable->relations(aDigit);
      iterator iterHit = hitsCont.begin();   
      for ( ; iterHit != hitsCont.end(); ++iterHit){

        const LHCb::MCHit* aHit = iterHit->to();
        hitMap[aHit] += iterHit->weight();
        foundCharge += iterHit->weight();
      } // iterHit
    } // aDigit
    ++iterChan;
  } // Digit

  // difference between depEnergy and total cluster size is noise
  // This is due to the normalization !!!!
  hitMap[0] = aCluster->size()-foundCharge;

  return StatusCode::SUCCESS;
}

 









