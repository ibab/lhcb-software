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

DECLARE_ALGORITHM_FACTORY(VPDigitLinker)

//=============================================================================
// Constructor
//=============================================================================
VPDigitLinker::VPDigitLinker(const std::string& name,
                             ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator) {

  declareProperty("DigitLocation", m_digitLocation = 
                  LHCb::VPDigitLocation::Default);
  declareProperty("MCDigitLocation", m_mcdigitLocation = 
                  LHCb::MCVPDigitLocation::Default);
  declareProperty("HitLinkLocation", m_hitLinkLocation =  
                  LHCb::VPDigitLocation::Default + "2MCHits");
  declareProperty("ParticleLinkLocation", m_particleLinkLocation = 
                  LHCb::VPDigitLocation::Default);

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
  const auto digits = getIfExists<LHCb::VPDigits>(m_digitLocation);
  if (!digits) {
    return Error("No digits in " + m_digitLocation, StatusCode::FAILURE);
  }
  // Get MC digits.
  const auto mcdigits = getIfExists<LHCb::MCVPDigits>(m_mcdigitLocation);
  if (!mcdigits) {
    return Error("No MC digits in " + m_mcdigitLocation, StatusCode::FAILURE);
  }
  // Create association tables. 
  LinkerWithKey<LHCb::MCHit> hitLinker(evtSvc(), msgSvc(), m_hitLinkLocation);
  LinkerWithKey<LHCb::MCParticle> particleLinker(evtSvc(), msgSvc(), 
                                                 m_particleLinkLocation);

  // Loop over the digits.
  for (const LHCb::VPDigit* digit : *digits) {
    // Get the MC digit with the same channel ID.
    const LHCb::MCVPDigit* mcDigit = mcdigits->object(digit->key());
    if (!mcDigit) continue;
    // Get the MC hits associated to the MC digit and their weights.
    const auto hits = mcDigit->mcHits();
    const auto deposits = mcDigit->deposits();
    const unsigned int nDeposits = deposits.size();
    // Make a map of the MC hits/particles and weights.
    std::map<const LHCb::MCHit*, double> hitMap;
    std::map<const LHCb::MCParticle*, double> particleMap;
    for (unsigned int i = 0; i < nDeposits; ++i) {
      const double weight = deposits[i];
      const LHCb::MCHit* hit = hits[i];
      const LHCb::MCParticle* particle = hit->mcParticle();
      // Check if the hit originates from a delta ray.
      if (particle && particle->originVertex() &&
          particle->originVertex()->type() == LHCb::MCVertex::DeltaRay) {
        // Check if there is another hit from the mother particle of the delta.
        const LHCb::MCParticle* mother = particle->mother();
        bool foundMother = false;
        for (auto ith = hits.cbegin(), end = hits.cend(); ith != end; ++ith) {
          if ((*ith)->mcParticle() == mother) {
            // Add the deposit to the hit of the mother particle.
            hitMap[*ith] += weight;
            particleMap[(*ith)->mcParticle()] += weight;
            foundMother = true;
            break;
          }
        } 
        // If no other hit from the mother particle is found,
        // add the delta ray hit as a separate entry.
        if (!foundMother) {
          hitMap[hit] += weight;
          particleMap[particle] += weight;
        }
      } else {
        hitMap[hit] += weight;
        particleMap[particle] += weight;
      }
    }
    for (const auto& element : hitMap) {
      // Make the association.
      hitLinker.link(digit->key(), element.first, element.second);

    }
    for (const auto& element : particleMap) {
      // Make the association.
      particleLinker.link(digit->key(), element.first, element.second);
    }
  }
  return StatusCode::SUCCESS;
}

