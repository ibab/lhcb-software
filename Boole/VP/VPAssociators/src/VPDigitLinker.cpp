// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
// LHCb
// Kernel/LHCbMath
#include "LHCbMath/GeomFun.h"
#include "LHCbMath/LineTypes.h"
// Event/LinkerEvent
#include "Linker/LinkerWithKey.h"
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

  declareProperty("DigitLocation", m_digitLocation = 
                  VPDigitLocation::VPDigitLocation);
  declareProperty("MCDigitLocation", m_mcdigitLocation = 
                  MCVPDigitLocation::MCVPDigitLocation);
  declareProperty("HitLinkLocation", m_hitLinkLocation =  
                  VPDigitLocation::VPDigitLocation + "2MCHits");
  declareProperty("ParticleLinkLocation", m_particleLinkLocation = 
                  VPDigitLocation::VPDigitLocation + "2MCParticles");

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
  LinkerWithKey<MCHit, VPDigit> hitLinker(evtSvc(), msgSvc(), 
                                          m_hitLinkLocation);
  LinkerWithKey<MCParticle, VPDigit> particleLinker(evtSvc(), msgSvc(), 
                                                    m_particleLinkLocation);
  // Loop over the digits.
  VPDigits::const_iterator it;
  for (it = digits->begin(); it != digits->end(); ++it) {
    // Get the associated hits and their weights. 
    std::map<const LHCb::MCHit*, double> hitMap;
    if (!associateToTruth(*it, mcdigits, hitMap)) {
      warning() << "Association failed" << endmsg;
      continue;
    }
    std::map<const LHCb::MCHit*, double>::const_iterator itm;
    for (itm = hitMap.begin(); itm != hitMap.end(); ++itm) {
      SmartRef<LHCb::MCHit> hit = (*itm).first;
      const double weight = (*itm).second;
      const MCParticle* particle = hit->mcParticle();
      // Do the associations.
      hitLinker.link(*it, hit, weight);
      particleLinker.link(*it, particle, weight);
    } 
  }
  return StatusCode::SUCCESS;
}

bool VPDigitLinker::associateToTruth(const VPDigit* digit,
                                     const MCVPDigits* mcdigits,
                                     std::map<const MCHit*, double>& hitMap) {

  // Find the corresponding MC digit.  
  const MCVPDigit* mcDigit = 0;
  MCVPDigits::const_iterator it;
  for (it = mcdigits->begin(); it != mcdigits->end(); ++it) {
    if ((*it)->channelID() == digit->key()) {
      mcDigit = *it;
      break;
    }
  }
  if (!mcDigit) {
    warning() << "No MC digit with channel ID " << digit->key() << endmsg;
    return false;
  }
  // Loop over the deposits and get the hits and their weights.
  std::map<const MCHit*, double> tempMap;
  SmartRefVector<MCVPDeposit> deposits = mcDigit->mcDeposit();
  SmartRefVector<MCVPDeposit>::iterator itd;
  for (itd = deposits.begin(); itd != deposits.end(); ++itd) {
    const MCHit* hit = (*itd)->mcHit();
    tempMap[hit] += (*itd)->depositedCharge();
  }
  // Clean out the delta-rays
  mergeDeltaRays(tempMap, hitMap);
  return true;

}

void VPDigitLinker::mergeDeltaRays(const std::map<const MCHit*, double>& inputMap,
                                   std::map<const MCHit*,double>& hitMap) {

  // Separate into hits from delta rays and normal hits.
  std::map<const MCHit*, double> deltaRays;
  std::map<const MCHit*, double>::const_iterator it;
  for (it = inputMap.begin(); it != inputMap.end(); ++it) {
    const MCHit* hit = it->first;
    if (!hit) continue;
    const MCParticle* particle = hit->mcParticle();
    if (particle && particle->originVertex() &&
        particle->originVertex()->type() == LHCb::MCVertex::DeltaRay) {
      deltaRays.insert(*it);
    } else {
      hitMap.insert(*it);
    }
  }
  // Tolerance for merging hits.
  const double tol = 0.02 * Gaudi::Units::mm;
  std::map<const MCHit*, double>::const_iterator itd;
  for (itd = deltaRays.begin(); itd != deltaRays.end(); ++itd) {
    // Check if a related hit is in the table.
    const MCHit* hit = 0;
    for (it = hitMap.begin(); it != hitMap.end() ; ++it) {
      if (it->first->mcParticle() == itd->first->mcParticle()) {
        hit = it->first;
        break;
      }
    }
    if (!hit) {
      // No existing candidate found.
      hitMap.insert(*itd);
    } else {
      Gaudi::XYZPoint dPoint = itd->first->midPoint();
      Gaudi::Math::XYZLine hitLine = Gaudi::Math::XYZLine(it->first->entry(), 
                                                          it->first->exit());
      const double dist = impactParameter(dPoint, hitLine);
      if (fabs(dist) < tol) {
        // Merge with the existing hit.
        hitMap[it->first] += itd->second;
      } else {
        hitMap.insert(*itd);
      }
    }
  }

}
