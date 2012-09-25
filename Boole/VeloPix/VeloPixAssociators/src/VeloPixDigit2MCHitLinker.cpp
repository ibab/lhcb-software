// $Id: VeloPixDigit2MCHitLinker.cpp,v 1.1.1.1 2009-12-04 14:34:46 marcin Exp $
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Linker
#include "Linker/LinkerWithKey.h"
// Event
#include "Event/VeloPixDigit.h"
#include "Event/MCTruth.h"
// Local
#include "VeloPixDigit2MCHitLinker.h"
#include "VeloPixMCLinkTool.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VeloPixDigit2MCHitLinker
//
// 06/11/2009 : Marcin Kucharczyk
// Based on ST code
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VeloPixDigit2MCHitLinker);

VeloPixDigit2MCHitLinker::VeloPixDigit2MCHitLinker(const std::string& name,
                                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator),
    m_hitLocation("MC/VP/Hits") 
{
  declareProperty("InputData", m_inputData = 
                  LHCb::VeloPixDigitLocation::VeloPixDigitLocation);
  declareProperty("OutputData", m_outputData = 
                  LHCb::VeloPixDigitLocation::VeloPixDigitLocation+"2MCHits");
  declareProperty("AddSpillOverHits", m_addSpillOverHits = false); 
  declareProperty("MinFraction", m_minFrac = 0.05);
  declareProperty("OneRef",m_oneRef = false);
  declareProperty("Conversion", m_conversion = 2100.0);
}

//=============================================================================
// Destructor
//=============================================================================
VeloPixDigit2MCHitLinker::~VeloPixDigit2MCHitLinker() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VeloPixDigit2MCHitLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  if(sc) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VeloPixDigit2MCHitLinker::execute()
{ // printf("VeloPixDigit2MCHitLinker::execute():\n");
  // Get VeloPixDigits
  const VeloPixDigits* digitCont = get<VeloPixDigits>(m_inputData);
  // Get MCVeloPixDigits
  const MCVeloPixDigits* digitsMC = get<MCVeloPixDigits>("MC/VP/Digits");
  // Get MC hits
  MCHits* mcHits = get<MCHits>(m_hitLocation); 
  // Create an association table 
  LinkerWithKey<MCHit,VeloPixDigit> myLink(evtSvc(),msgSvc(),outputData());
  // Loop and link VeloPixDigits to MC truth
  VeloPixDigits::const_iterator iterDigit = digitCont->begin();
  for(; iterDigit != digitCont->end(); ++iterDigit) {
    // Find all hits
    HitMap hitMap;                                 // HitMap = std::map<const LHCb::MCHit*,double> HitMap;
    double foundCharge = 0.0;
    VeloPixMCLinkTool::associateToTruth(*iterDigit,digitsMC,
                                        hitMap,foundCharge);
    hitMap[0] += ((*iterDigit)->ToTValue() * m_conversion - foundCharge);
    // Select references to add to table
    std::vector<HitPair> selectedRefs;             // HitPair = std::pair<const LHCb::MCHit*,double>
    double tCharge = this->totalCharge(hitMap);
    refsToRelate(selectedRefs,hitMap,tCharge,mcHits);
    // printf(" ToTValue=%d hitMap[%d] foundCharge=%8.1f tCharge=%8.1f selectedRefs[%d]\n",
    //       (*iterDigit)->ToTValue(), hitMap.size(), foundCharge, tCharge, selectedRefs.size() );
    if((selectedRefs.empty() == false)) {
      if(m_oneRef == false) {
        std::vector<HitPair>::iterator iterPair = selectedRefs.begin();
        while(iterPair != selectedRefs.end()){
          myLink.link(*iterDigit,iterPair->first,iterPair->second);
          ++iterPair;
        }
      }
      else {
        HitPair bestPair = selectedRefs.back();
        myLink.link(*iterDigit,bestPair.first,bestPair.second);
      }
    }
  }

  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate total charge
//============================================================================
double VeloPixDigit2MCHitLinker::totalCharge(const HitMap& hitMap) const
{ // printf("VeloPixDigit2MCHitLinker::totalCharge():");
  double totCharge = 0.0;
  HitMap::const_iterator iterMap = hitMap.begin();
  while (iterMap != hitMap.end()) {
    totCharge += fabs((*iterMap).second);
    // printf(" %+3.1f", (*iterMap).second);
    ++iterMap;
  }
  // printf(" =%3.1f\n", totCharge);
  return totCharge;
}


//============================================================================
// Reference to related hits
//============================================================================
void VeloPixDigit2MCHitLinker::refsToRelate(std::vector<HitPair>& selectedRefs,
                                            const HitMap& hitMap,
                                            const double& totCharge,
                                            MCHits* hits) const
{ // printf("VeloPixDigit2MCHitLinker::refsToRelate():");
  // Iterate over map
  HitMap::const_iterator iterMap = hitMap.begin();
  while(iterMap != hitMap.end()) {
    const MCHit* aHit = iterMap->first;
    double frac = -1.0;
    if(totCharge >0.0) {
      frac = fabs(iterMap->second / totCharge);
    }
    if((0 != aHit) && (frac > m_minFrac)) {
      if((m_addSpillOverHits == true) || (hits == aHit->parent())) {
        selectedRefs.push_back(std::make_pair(aHit,frac));
      }
    }
    // printf(" %+3.1f/%3.1f", iterMap->second, totCharge);
    ++iterMap;
  }
  // printf("\n");
  return;
}


//============================================================================
StatusCode VeloPixDigit2MCHitLinker::finalize() {

  return GaudiAlgorithm::finalize();

}
