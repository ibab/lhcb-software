// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Linker
#include "Linker/LinkerWithKey.h"
// Event
#include "Event/VPDigit.h"
#include "Event/MCTruth.h"
// Local
#include "VPDigit2MCHitLinker.h"
#include "VPMCLinkTool.h"

using namespace LHCb;

//------------------------------------------------------------
// Implementation file for class : VPDigit2MCHitLinker
//
// 06/11/2009 : Marcin Kucharczyk
// Based on ST code
//------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPDigit2MCHitLinker);

VPDigit2MCHitLinker::VPDigit2MCHitLinker(const std::string& name,
                                                   ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator),
    m_hitLocation("MC/VP/Hits") 
{
  declareProperty("InputData", m_inputData = 
                  LHCb::VPDigitLocation::VPDigitLocation);
  declareProperty("OutputData", m_outputData = 
                  LHCb::VPDigitLocation::VPDigitLocation+"2MCHits");
  declareProperty("AddSpillOverHits", m_addSpillOverHits = false); 
  declareProperty("MinFraction", m_minFrac = 0.05);
  declareProperty("OneRef",m_oneRef = false);
  declareProperty("Conversion", m_conversion = 2100.0);
}

//=============================================================================
// Destructor
//=============================================================================
VPDigit2MCHitLinker::~VPDigit2MCHitLinker() {};

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPDigit2MCHitLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  if(sc) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPDigit2MCHitLinker::execute()
{ // printf("VPDigit2MCHitLinker::execute():\n");
  // Get VPDigits
  const VPDigits* digitCont = get<VPDigits>(m_inputData);
  // Get MCVPDigits
  const MCVPDigits* digitsMC = get<MCVPDigits>("MC/VP/Digits");
  // Get MC hits
  MCHits* mcHits = get<MCHits>(m_hitLocation); 
  // Create an association table 
  LinkerWithKey<MCHit,VPDigit> myLink(evtSvc(),msgSvc(),outputData());
  // Loop and link VPDigits to MC truth
  VPDigits::const_iterator iterDigit = digitCont->begin();
  for(; iterDigit != digitCont->end(); ++iterDigit) {
    // Find all hits
    HitMap hitMap;                                 // HitMap = std::map<const LHCb::MCHit*,double> HitMap;
    double foundCharge = 0.0;
    VPMCLinkTool::associateToTruth(*iterDigit,digitsMC,
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
double VPDigit2MCHitLinker::totalCharge(const HitMap& hitMap) const
{ // printf("VPDigit2MCHitLinker::totalCharge():");
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
void VPDigit2MCHitLinker::refsToRelate(std::vector<HitPair>& selectedRefs,
                                            const HitMap& hitMap,
                                            const double& totCharge,
                                            MCHits* hits) const
{ // printf("VPDigit2MCHitLinker::refsToRelate():");
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
