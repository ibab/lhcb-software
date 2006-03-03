// $Id: STDigit2MCParticleLinker.cpp,v 1.3 2006-03-03 17:01:11 mneedham Exp $
// Include files 

#include "Event/STDigit.h"
#include "Event/STDigit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"

#include "Linker/LinkerWithKey.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "STDigit2MCParticleLinker.h"
#include "STTruthTool.h"

#include "Kernel/STDetSwitch.h"

#include <algorithm>

//-----------------------------------------------------------------------------
// Implementation file for class : STDigit2MCParticleChi2Alg
//
// 25/04/2002 : M. Needham
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<STDigit2MCParticleLinker>          s_factory ;
const        IAlgFactory& STDigit2MCParticleLinkerFactory = s_factory ; 

STDigit2MCParticleLinker::STDigit2MCParticleLinker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name,pSvcLocator) 
{

  // constructer
  declareProperty("OutputData", m_outputData = LHCb::STDigitLocation::TTDigits);
  declareProperty("InputData",  m_inputData  = LHCb::STDigitLocation::TTDigits);

  declareProperty( "addSpillOverHits",m_addSpillOverHits = false); 
  declareProperty("minfrac", m_minFrac = 0.3);
  declareProperty("oneRef",m_oneRef = false);
  declareProperty("detType", m_detType = "TT");
}

StatusCode STDigit2MCParticleLinker::initialize() {

  // initialize
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }


  STDetSwitch::flip(m_detType,m_inputData);
  STDetSwitch::flip(m_detType,m_outputData);

  return StatusCode::SUCCESS;
}

STDigit2MCParticleLinker::~STDigit2MCParticleLinker() {
  // destructer
}; 

StatusCode STDigit2MCParticleLinker::execute() {

  // get LHCb::STDigits
  LHCb::STDigits* digitCont = get<LHCb::STDigits>(m_inputData);

  // get LHCb::MCParticles
  LHCb::MCParticles* mcParts = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
 
  // creating a linker which is the bastard spawn of satan
  LinkerWithKey<LHCb::MCParticle,LHCb::STDigit> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link LHCb::STDigits to MC truth
  LHCb::STDigits::const_iterator iterDigit = digitCont->begin() ;
  for( ; iterDigit != digitCont->end(); ++iterDigit){

    // find all particles
    std::map<const LHCb::MCParticle*,double> partMap;
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
	  // std::cout << "link it" << std::endl;
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


double STDigit2MCParticleLinker::totalCharge(const std::map<const LHCb::MCParticle*,double>& partMap) const{

  double totCharge = 0.0;
  std::map<const LHCb::MCParticle*,double>::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  } // iterMap

  return totCharge;
}

StatusCode STDigit2MCParticleLinker::refsToRelate(std::vector<partPair>& selectedRefs,
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








