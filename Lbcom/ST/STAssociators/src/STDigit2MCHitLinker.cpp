// $Id: STDigit2MCHitLinker.cpp,v 1.5 2007-01-09 15:05:00 jvantilb Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerWithKey.h"

// Event
#include "Event/STDigit.h"
#include "Event/MCTruth.h"
#include "Kernel/STDetSwitch.h"

// local
#include "STDigit2MCHitLinker.h"
#include "STTruthTool.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigit2MCHitLinker );

STDigit2MCHitLinker::STDigit2MCHitLinker( const std::string& name,
                                          ISvcLocator* pSvcLocator) :
  GaudiAlgorithm (name,pSvcLocator) 
{
  declareProperty("OutputData", m_outputData = STDigitLocation::TTDigits 
                  + "2MCHits" );
  declareProperty("InputData", m_inputData  = STDigitLocation::TTDigits);
  declareProperty("addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.05);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("detType", m_detType = "TT");
}

STDigit2MCHitLinker::~STDigit2MCHitLinker()
{
  // destructer
} 

StatusCode STDigit2MCHitLinker::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);
   
  m_hitLocation = MCHitLocation::TT;

  STDetSwitch::flip(m_detType,m_hitLocation);
  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;
}

StatusCode STDigit2MCHitLinker::execute()
{
  // get STDigits
  STDigits* digitCont = get<STDigits>(m_inputData);

  // get hits
  MCHits* mcHits = get<MCHits>(m_hitLocation); 

  // create an association table 
  LinkerWithKey<MCHit,STDigit> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STDigits to MC truth
  STDigits::const_iterator iterDigit;
  for(iterDigit = digitCont->begin(); 
      iterDigit != digitCont->end(); ++iterDigit){

    // find all hits
    std::map<const MCHit*,double> hitMap;
    double foundCharge = 0.0;
    STTruthTool::associateToTruth(*iterDigit,hitMap,foundCharge);
    hitMap[0] += ((*iterDigit)->depositedCharge()-foundCharge);
 
    // select references to add to table
    std::vector<hitPair> selectedRefs;
    double tCharge = this->totalCharge(hitMap);
    refsToRelate(selectedRefs,hitMap,tCharge,mcHits);

    if ((selectedRefs.empty() == false)){
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
}

double STDigit2MCHitLinker::totalCharge(const std::map<const MCHit*,double>&
                                        hitMap) const
{
  double totCharge = 0.;
  std::map<const MCHit*,double>::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()){
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  } // iterMap

  return totCharge;
}

StatusCode STDigit2MCHitLinker::refsToRelate(std::vector<hitPair>& selectedRefs,
                                    const std::map<const MCHit*,double>& hitMap,
                                             const double& totCharge,
                                             MCHits* hits) const
{
  // iterate over map
  std::map<const MCHit*,double>::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()){
    const MCHit* aHit = iterMap->first;
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
