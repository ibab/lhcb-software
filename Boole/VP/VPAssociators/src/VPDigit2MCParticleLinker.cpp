// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// Linker
#include "Linker/LinkerWithKey.h"
// Event
#include "Event/VPDigit.h"
#include "Event/MCParticle.h"
// local
#include "VPDigit2MCParticleLinker.h"
#include "VPMCLinkTool.h"

using namespace LHCb;

//--------------------------------------------------------------
// Implementation file for class : VPDigit2MCParticleLinker
//
// 06/11/2009 : Marcin Kucharczyk
// Based on ST code
//--------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY(VPDigit2MCParticleLinker)

VPDigit2MCParticleLinker::VPDigit2MCParticleLinker(
                               const std::string& name,
                               ISvcLocator* pSvcLocator)
  : GaudiAlgorithm(name, pSvcLocator)
{
  declareProperty("InputData", m_inputData = 
                  LHCb::VPDigitLocation::VPDigitLocation);
  declareProperty("OutputData", m_outputData = 
                  LHCb::VPDigitLocation::VPDigitLocation+"2MCParticles");
  declareProperty("AddSpillOverHits", m_addSpillOverHits = false); 
  declareProperty("Minfrac", m_minFrac = 0.05);
  declareProperty("OneRef", m_oneRef = false);
}

//=============================================================================
// Destructor
//=============================================================================
VPDigit2MCParticleLinker::~VPDigit2MCParticleLinker() {}

//=============================================================================
// Initialisation
//=============================================================================
StatusCode VPDigit2MCParticleLinker::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if(sc.isFailure()) return sc;
  if(sc) debug() << "==> Initialise" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Execution
//=============================================================================
StatusCode VPDigit2MCParticleLinker::execute()
{
  // Get VPDigits
  const VPDigits* digitCont = get<VPDigits>(m_inputData);
  // Get MCVPDigits
  const MCVPDigits* digitsMC = get<MCVPDigits>("MC/VP/Digits");
  // Get MCParticles
  MCParticles* mcParts = get<MCParticles>(MCParticleLocation::Default);
  // Creating linker
  LinkerWithKey<MCParticle,VPDigit> 
               myLink(evtSvc(),msgSvc(),outputData());
  // Loop and link VPDigits to MC truth
  VPDigits::const_iterator iterDigit = digitCont->begin();
  for(; iterDigit != digitCont->end(); ++iterDigit) {
    // Find all particles
    ParticleMap partMap;
    VPMCLinkTool::associateToTruth(*iterDigit,digitsMC,partMap);
    // Select references to add to table
    std::vector<PartPair> selectedRefs;
    double tCharge = this->totalCharge(partMap);
    refsToRelate(selectedRefs,partMap,tCharge,mcParts);
    if (selectedRefs.empty() == false) {
      if (m_oneRef == false) {
        std::vector<PartPair>::iterator iterPair = selectedRefs.begin();
        while(iterPair != selectedRefs.end()) {
          myLink.link(*iterDigit,iterPair->first,iterPair->second);
          ++iterPair;
        }
      }
      else {
        PartPair bestPair = selectedRefs.back();
        myLink.link(*iterDigit,bestPair.first,bestPair.second);
      } 
    }
  }
   
  return StatusCode::SUCCESS;
}


//============================================================================
// Calculate total charge
//============================================================================
double VPDigit2MCParticleLinker::totalCharge(const ParticleMap& partMap) 
                                      const
{
  double totCharge = 0.0;
  ParticleMap::const_iterator iterMap = partMap.begin();
  while(iterMap != partMap.end()) {
    totCharge += fabs((*iterMap).second);
    ++iterMap;
  }

  return totCharge;
}


//============================================================================
// Reference to related MC particles
//============================================================================
void VPDigit2MCParticleLinker::refsToRelate(
                                    std::vector<PartPair>& selRefs,
                                    const ParticleMap& partMap,
                                    const double totCharge,
                                    MCParticles* particles) const
{
  // Iterate over map
  ParticleMap::const_iterator iterMap = partMap.begin();
  while(iterMap != partMap.end()) {
    const MCParticle* aParticle = iterMap->first;
    double frac = -1.0;
    if(totCharge > 0.0) {
     frac = iterMap->second / totCharge;
    }
    if((0 != aParticle) && (frac>m_minFrac)) {
      if((m_addSpillOverHits == true) || (particles == aParticle->parent())) {
        selRefs.push_back(std::make_pair(aParticle,frac));
      }
    }
    ++iterMap;
  }
  return;
}
