// $Id: SiGeantDepositedCharge.cpp,v 1.1.1.1 2006-05-16 08:33:54 mneedham Exp $
#include "GaudiKernel/ToolFactory.h"

#include "SiGeantDepositedCharge.h"
#include "Kernel/LHCbConstants.h"
#include "Event/MCHit.h"

using namespace LHCb;

static const ToolFactory<SiGeantDepositedCharge>  s_factory;
const IToolFactory& SiGeantDepositedChargeFactory = s_factory;

/// constructor
SiGeantDepositedCharge::SiGeantDepositedCharge(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface* parent): 
  GaudiTool( type, name, parent )
{
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
 
  // need a line here to get the interface correct !!!!
  declareInterface<ISiDepositedCharge>(this);
}

/// destructer
SiGeantDepositedCharge::~SiGeantDepositedCharge() {
}

/// calc charge
double SiGeantDepositedCharge::charge(const MCHit* aHit) const{
 return ((m_scalingFactor*aHit->energy())/LHCbConstants::SiEnergyPerIonPair);
}



