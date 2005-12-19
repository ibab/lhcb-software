// $Id: STGeantDepositedCharge.cpp,v 1.1.1.1 2005-12-19 15:43:15 mneedham Exp $
#include "GaudiKernel/ToolFactory.h"

#include "STGeantDepositedCharge.h"
#include "Kernel/LHCbConstants.h"
#include "Event/MCHit.h"

using namespace LHCb;

static const ToolFactory<STGeantDepositedCharge>  s_factory;
const IToolFactory& STGeantDepositedChargeFactory = s_factory;

/// constructor
STGeantDepositedCharge::STGeantDepositedCharge(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface* parent): 
  GaudiTool( type, name, parent )
{
  declareProperty("scalingFactor", m_scalingFactor = 0.8);
 
  // need a line here to get the interface correct !!!!
  declareInterface<ISTDepositedCharge>(this);
}

/// destructer
STGeantDepositedCharge::~STGeantDepositedCharge() {
}

/// calc charge
double STGeantDepositedCharge::charge(const MCHit* aHit) const{
 return ((m_scalingFactor*aHit->energy())/LHCbConstants::SiEnergyPerIonPair);
}



