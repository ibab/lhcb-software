// $Id: SiGeantDepositedCharge.cpp,v 1.3 2007-01-09 14:57:21 jvantilb Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"

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
  GaudiTool( type, name, parent )
{
  /// constructor
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
 
  // need a line here to get the interface correct !
  declareInterface<ISiDepositedCharge>(this);
}

SiGeantDepositedCharge::~SiGeantDepositedCharge()
{ 
  // destructer
}

double SiGeantDepositedCharge::charge(const LHCb::MCHit* aHit) const
{
  return m_scalingFactor * aHit->energy() / LHCbConstants::SiEnergyPerIonPair;
}
