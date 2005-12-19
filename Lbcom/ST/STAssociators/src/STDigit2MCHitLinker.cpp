// $Id: STDigit2MCHitLinker.cpp,v 1.2 2005-12-19 16:42:47 mneedham Exp $

// Event
#include "Event/STDigit.h"
#include "Event/STCluster.h"
#include "Event/MCTruth.h"
#include "Linker/LinkerWithKey.h"

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "STDigit2MCHitLinker.h"
#include "STTruthTool.h"
#include "ISTDigitSelector.h"

#include "Kernel/STDetSwitch.h"

//-----------------------------------------------------------------------------
// Implementation file for class : STDigit2MCHitLinker
//
// 25/04/2002 : M. Needham
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<STDigit2MCHitLinker>          s_factory ;
const        IAlgFactory& STDigit2MCHitLinkerFactory = s_factory ; 

STDigit2MCHitLinker::STDigit2MCHitLinker( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{
  // constructer
  declareProperty("OutputDataBase", m_outputData  = "TTDigits2MCHits" );
  declareProperty("InputData", m_inputData  = LHCb::STDigitLocation::TTDigits);
  declareProperty("addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.05);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("selector",m_selectorName = "STSelectFromCluster");
  declareProperty("detType", m_detType = "TT");
}

STDigit2MCHitLinker::~STDigit2MCHitLinker() {
  // destructer
}; 

StatusCode STDigit2MCHitLinker::initialize() {

  // initialize
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  m_selector = tool<ISTDigitSelector>(m_selectorName,"digitSelector", this);
 
  m_hitLocation = LHCb::MCHitLocation::TT;

  STDetSwitch::flip(m_detType,m_hitLocation);
  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;
};


StatusCode STDigit2MCHitLinker::execute() {

  // get LHCb::STDigits
  LHCb::STDigits* digitCont = get<LHCb::STDigits>(m_inputData);

  // get hits
  LHCb::MCHits* mcHits = get<LHCb::MCHits>(m_hitLocation);
 
  // init selector
  m_selector->initializeEvent();

  // create an association table 
  LinkerWithKey<LHCb::MCHit,LHCb::STDigit> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STDigits to MC truth
  LHCb::STDigits::const_iterator iterDigit;
  for(iterDigit = digitCont->begin(); 
      iterDigit != digitCont->end(); ++iterDigit){

    // find all hits
    std::map<const LHCb::MCHit*,double> hitMap;
    double foundCharge = 0.0;
    STTruthTool::associateToTruth(*iterDigit,hitMap,foundCharge);
    hitMap[0] += ((*iterDigit)->depositedCharge()-foundCharge);
 
    // select references to add to table
    std::vector<hitPair> selectedRefs;
    double tCharge = this->totalCharge(hitMap);
    refsToRelate(selectedRefs,hitMap,tCharge,mcHits);

    if ((selectedRefs.empty() == false)&&((m_selector->execute(*iterDigit) == true))){
      if (m_oneRef == false ){
        std::vector<hitPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          myLink.link(*iterDigit,iterPair->first,iterPair->second);
          ++iterPair;
        } //iterPair
      }
      else{
        hitPair bestPair = selectedRefs.back();
        myLink.link(*iterDigit,bestPair.first,bestPair.second);
      }
    } // refsToRelate ! empty

  } // loop iterDigit

 
  return StatusCode::SUCCESS;
};


double STDigit2MCHitLinker::totalCharge(const std::map<const LHCb::MCHit*,double>& hitMap) const{

  double totCharge = 0.;
  std::map<const LHCb::MCHit*,double>::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()){
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  } // iterMap

  return totCharge;
}

StatusCode STDigit2MCHitLinker::refsToRelate(std::vector<hitPair>& selectedRefs,
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
