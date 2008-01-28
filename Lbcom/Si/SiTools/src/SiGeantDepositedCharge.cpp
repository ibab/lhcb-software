// $Id: SiGeantDepositedCharge.cpp,v 1.5 2008-01-28 15:48:02 mneedham Exp $

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
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
 
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



