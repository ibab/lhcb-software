// Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"
#include "Kernel/SiLandauFun.h"

// MCEvent
#include "Event/MCHit.h"

// local
#include "SiDepositedCharge.h"

using namespace LHCb;
using namespace Gaudi::Units;

DECLARE_TOOL_FACTORY( SiDepositedCharge )

SiDepositedCharge::SiDepositedCharge(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface* parent) : 
  SiDepositedChargeBase( type, name, parent )
{
  // constructor
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
}

SiDepositedCharge::~SiDepositedCharge()
{
  // destructer
}

StatusCode SiDepositedCharge::initialize()
{ 
  StatusCode sc = SiDepositedChargeBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize generators .
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator(Rndm::Landau(0.2226,1.0),m_LandauDist.pRef());
  if (sc.isFailure()) return Error( "Failed to init generator ", sc);

  sc = release(tRandNumSvc);
  if (sc.isFailure()) return Error( "Failed to release RndmSvc ", sc);
 
  return sc;
}

double SiDepositedCharge::charge(const MCHit* aHit) const
{
  // calculate - deposited charge Landau convolved with Gauss
 // see for example Bichsel '88 
  double pathLength = m_scalingFactor * aHit->pathLength();
  const MCParticle* aParticle = aHit->mcParticle();
  double beta = aHit->p() /
    sqrt(gsl_pow_2(aParticle->virtualMass())+gsl_pow_2(aHit->p()));
  beta = GSL_MAX(0.2,beta);
  double betaGamma = aHit->p()/aParticle->virtualMass();

  // calculate scale of landau
  double scale = SiLandauFun::scale(beta,pathLength);  

  return ( SiLandauFun::MPVFromScale(beta,betaGamma,scale) + 
           (m_LandauDist->shoot()*scale) + 
           (m_GaussDist->shoot()*atomicBinding(pathLength)) ) /
    LHCbConstants::SiEnergyPerIonPair ;
}


