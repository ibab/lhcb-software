// STL
#include <map>
// LHCb
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
// Event/MCEvent
#include "Event/MCHit.h"
#include "Event/MCVPDigit.h"
// Event/DigiEvent
#include "Event/VPDigit.h"
// Local
#include "VPDigitLinker.h"

//------------------------------------------------------------
// Implementation file for class : VPDigitLinker
//
//------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPDigitLinker)

using namespace LHCb;

VPDigitLinker::VPDigitLinker(const std::string& name,
                             ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator) {

  declareProperty("DigitLocation", m_digitLocation = VPDigitLocation::Default);
  declareProperty("MCDigitLocation", m_mcdigitLocation = 
                  MCVPDigitLocation::Default);
  declareProperty("HitLinkLocation", m_hitLinkLocation =  
                  VPDigitLocation::Default + "2MCHits");
  declareProperty("ParticleLinkLocation", m_particleLinkLocation = 
                  VPDigitLocation::Default);

}

//=============================================================================
// Destructor
//=============================================================================
VPDigitLinker::~VPDigitLinker() {}

//=============================================================================
// Execution
//=============================================================================
StatusCode VPDigitLinker::execute() {

  // Get digits.
  const VPDigits* digits = getIfExists<VPDigits>(m_digitLocation);
  if (!digits) {
    error() << "No digits in " << m_digitLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Get MC digits.
  const MCVPDigits* mcdigits = getIfExists<MCVPDigits>(m_mcdigitLocation);
  if (!mcdigits) {
    error() << "No MC digits in " << m_mcdigitLocation << endmsg;
    return StatusCode::FAILURE;
  }
  // Create association tables. 
  LinkerWithKey<MCHit> hitLinker(evtSvc(), msgSvc(), m_hitLinkLocation);
  LinkerWithKey<MCParticle> particleLinker(evtSvc(), msgSvc(), m_particleLinkLocation);

  // Loop over the digits.
  VPDigits::const_iterator it;
  for (it = digits->begin(); it != digits->end(); ++it) {
    // Get the MC digit with the same channel ID.
    const MCVPDigit* mcDigit = mcdigits->object((*it)->key());
    if (!mcDigit) continue;
    // Get the MC hits associated to the MC digit and their weights.
    SmartRefVector<MCHit> hits = mcDigit->mcHits();
    const std::vector<double>& deposits = mcDigit->deposits();
    const unsigned int nDeposits = deposits.size();
    // Make a map of the MC hits and weights.
    std::map<const MCHit*, double> hitMap;
    for (unsigned int i = 0; i < nDeposits; ++i) {
      const MCHit* hit = hits[i];
      const MCParticle* particle = hit->mcParticle();
      // Check if the hit originates from a delta ray.
      if (particle && particle->originVertex() &&
          particle->originVertex()->type() == LHCb::MCVertex::DeltaRay) {
        // Check if there is another hit from the mother particle of the delta.
        const MCParticle* mother = particle->mother();
        SmartRefVector<MCHit>::iterator ith;
        bool foundMother = false;
        for (ith = hits.begin(); ith != hits.end(); ++ith) {
          if ((*ith)->mcParticle() == mother) {
            // Add the deposit to the hit of the mother particle.
            hitMap[*ith] += deposits[i];
            foundMother = true;
            break;
          }
        } 
        // If no other hit from the mother particle is found,
        // add the delta ray hit as a separate entry.
        if (!foundMother) hitMap[hit] += deposits[i];
      } else {
        hitMap[hit] += deposits[i];
      }
    }
    std::map<const LHCb::MCHit*, double>::const_iterator itm;
    for (itm = hitMap.begin(); itm != hitMap.end(); ++itm) {
      SmartRef<LHCb::MCHit> hit = (*itm).first;
      const double weight = (*itm).second;
      const MCParticle* particle = hit->mcParticle();
      // Do the associations.
      hitLinker.link((*it)->key(), hit, weight);
      particleLinker.link((*it)->key(), particle, weight);
    } 
  }
  return StatusCode::SUCCESS;
}

