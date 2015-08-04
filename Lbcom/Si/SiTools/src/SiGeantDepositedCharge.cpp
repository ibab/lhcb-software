// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"

// MCEvent
#include "Event/MCHit.h"

// local
#include "SiGeantDepositedCharge.h"


DECLARE_TOOL_FACTORY( SiGeantDepositedCharge )

SiGeantDepositedCharge::SiGeantDepositedCharge(const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent): 
  SiDepositedChargeBase( type, name, parent )
{
  /// constructor
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
  
  /// add atomic binding smearing or not
  declareProperty("applySmearing", m_applySmearing = false);  
}

SiGeantDepositedCharge::~SiGeantDepositedCharge()
{ 
  // destructer
}


double SiGeantDepositedCharge::charge(const LHCb::MCHit* aHit) const
{

  double aBinding = 0.0;
  if (m_applySmearing == true){
    aBinding = atomicBinding(aHit->pathLength())*m_GaussDist->shoot();
  }
  return m_scalingFactor * (aHit->energy() + aBinding)/ LHCbConstants::SiEnergyPerIonPair;
}



