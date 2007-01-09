// $Id: STDigit2MCParticleLinker.cpp,v 1.5 2007-01-09 15:05:00 jvantilb Exp $

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerWithKey.h"

// Event
#include "Event/STDigit.h"
#include "Event/MCParticle.h"
#include "Kernel/STDetSwitch.h"

// local
#include "STDigit2MCParticleLinker.h"
#include "STTruthTool.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigit2MCParticleLinker );

STDigit2MCParticleLinker::STDigit2MCParticleLinker( const std::string& name,
                                                    ISvcLocator* pSvcLocator) :
  GaudiAlgorithm (name,pSvcLocator)
{
  declareProperty("OutputData", m_outputData = STDigitLocation::TTDigits);
  declareProperty("InputData",  m_inputData  = STDigitLocation::TTDigits);
  declareProperty("addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.3);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("detType", m_detType = "TT");
}

StatusCode STDigit2MCParticleLinker::initialize()
{
  // initialize
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;
}

STDigit2MCParticleLinker::~STDigit2MCParticleLinker()
{
  // destructer
} 

StatusCode STDigit2MCParticleLinker::execute()
{
  // get STDigits
  STDigits* digitCont = get<STDigits>(m_inputData);

  // get MCParticles
  MCParticles* mcParts = get<MCParticles>(MCParticleLocation::Default);
 
  // creating a linker
  LinkerWithKey<MCParticle,STDigit> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STDigits to MC truth
  STDigits::const_iterator iterDigit = digitCont->begin() ;
  for( ; iterDigit != digitCont->end(); ++iterDigit){

    // find all particles
    std::map<const MCParticle*,double> partMap;
    STTruthTool::associateToTruth(*iterDigit,partMap);
     
    // select references to add to table
    std::vector<partPair> selectedRefs;
    double tCharge = this->totalCharge(partMap);
    refsToRelate(selectedRefs,partMap,tCharge,mcParts);

    if (selectedRefs.empty() == false){

      if (m_oneRef == false){
        std::vector<partPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          myLink.link(*iterDigit,iterPair->first,iterPair->second);
          ++iterPair;
        } //iterPair
      }
      else {
        partPair bestPair = selectedRefs.back();
        myLink.link(*iterDigit,bestPair.first,bestPair.second);
      } 
    } // refsToRelate != empty
  } // loop iterDigit
   
  return StatusCode::SUCCESS;
};


double STDigit2MCParticleLinker::totalCharge(const std::map<const MCParticle*,
                                             double>& partMap) const
{
  double totCharge = 0.0;
  std::map<const MCParticle*,double>::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  } // iterMap

  return totCharge;
}

StatusCode
STDigit2MCParticleLinker::refsToRelate(std::vector<partPair>& selRefs,
                              const std::map<const MCParticle*,double>& partMap,
                                       const double totCharge,
                                       MCParticles* particles) const
{
  // iterate over map
  std::map<const MCParticle*,double>::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    const MCParticle* aParticle = iterMap->first;
    double frac = -1.0;
    if (totCharge > 0.0){
     frac = iterMap->second/totCharge;
    }
    if ((0 != aParticle)&&(frac>m_minFrac)){
      if ((m_addSpillOverHits == true)||(particles == aParticle->parent())){
        selRefs.push_back(std::make_pair(aParticle,frac));
      }
    }
    ++iterMap;
  } // iterMap
  
  return StatusCode::SUCCESS;
}
