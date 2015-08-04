// Linker
#include "Linker/LinkerWithKey.h"

// Event
#include "Event/STDigit.h"
#include "Event/MCParticle.h"


// local
#include "STDigit2MCParticleLinker.h"
#include "STTruthTool.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( STDigit2MCParticleLinker )

STDigit2MCParticleLinker::STDigit2MCParticleLinker( const std::string& name,
                                                    ISvcLocator* pSvcLocator) :
  ST::AlgBase (name,pSvcLocator)
{
  declareSTConfigProperty("OutputData", m_outputData , STDigitLocation::TTDigits);
  declareSTConfigProperty("InputData",  m_inputData  , STDigitLocation::TTDigits);
  declareProperty("AddSpillOverHits", m_addSpillOverHits = false); 
  declareProperty("Minfrac", m_minFrac = 0.05);
  declareProperty("OneRef", m_oneRef = false);

}


STDigit2MCParticleLinker::~STDigit2MCParticleLinker()
{
  // destructer
} 

StatusCode STDigit2MCParticleLinker::execute()
{
  // get STDigits
  const STDigits* digitCont = get<STDigits>(m_inputData);

  // get MCParticles
  MCParticles* mcParts = get<MCParticles>(MCParticleLocation::Default);
 
  // creating a linker
  LinkerWithKey<MCParticle,STDigit> myLink( evtSvc(), msgSvc(), outputData() );

  // loop and link STDigits to MC truth
  STDigits::const_iterator iterDigit = digitCont->begin() ;
  for( ; iterDigit != digitCont->end(); ++iterDigit){

    // find all particles
    ParticleMap partMap;
    STTruthTool::associateToTruth(*iterDigit,partMap);
     
    // select references to add to table
    std::vector<PartPair> selectedRefs;
    double tCharge = this->totalCharge(partMap);
    refsToRelate(selectedRefs,partMap,tCharge,mcParts);

    if (selectedRefs.empty() == false){

      if (m_oneRef == false){
        std::vector<PartPair>::iterator iterPair = selectedRefs.begin();
        while (iterPair != selectedRefs.end()){
          myLink.link(*iterDigit,iterPair->first,iterPair->second);
          ++iterPair;
        } //iterPair
      }
      else {
        PartPair bestPair = selectedRefs.back();
        myLink.link(*iterDigit,bestPair.first,bestPair.second);
      } 
    } // refsToRelate != empty
  } // loop iterDigit
   
  return StatusCode::SUCCESS;
}


double STDigit2MCParticleLinker::totalCharge(const ParticleMap& partMap) const
{
  double totCharge = 0.0;
  ParticleMap::const_iterator iterMap = partMap.begin();
  while (iterMap != partMap.end()){
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  } // iterMap

  return totCharge;
}

void STDigit2MCParticleLinker::refsToRelate(std::vector<PartPair>& selRefs,
                                            const ParticleMap& partMap,
                                            const double totCharge,
                                            MCParticles* particles) const
{
  // iterate over map
  ParticleMap::const_iterator iterMap = partMap.begin();
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
  
  return;
}
