// $Id: SiGeantDepositedCharge.cpp,v 1.4 2007-12-11 10:14:25 mneedham Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"

// MCEvent
#include "Event/MCHit.h"

// local
#include "SiGeantDepositedCharge.h"


DECLARE_TOOL_FACTORY( SiGeantDepositedCharge );

SiGeantDepositedCharge::SiGeantDepositedCharge(const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent): 
  SiDepositedChargeBase( type, name, parent )
{
  /// constructor
  declareProperty("scalingFactor", m_scalingFactor = 0.95);
 
}

SiGeantDepositedCharge::~SiGeantDepositedCharge()
{ 
  // destructer
}


double SiGeantDepositedCharge::charge(const LHCb::MCHit* aHit) const
{

  double aBinding = atomicBinding(aHit->pathLength())*m_GaussDist->shoot();
  return m_scalingFactor * (aHit->energy() + aBinding)/ LHCbConstants::SiEnergyPerIonPair;
}



