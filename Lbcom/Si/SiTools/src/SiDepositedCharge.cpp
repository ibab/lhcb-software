// $Id: SiDepositedCharge.cpp,v 1.3 2007-01-09 14:57:21 jvantilb Exp $

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// LHCbKernel
#include "Kernel/LHCbConstants.h"

// MCEvent
#include "Event/MCHit.h"

// local
#include "SiDepositedCharge.h"

using namespace LHCb;
using namespace Gaudi::Units;

DECLARE_TOOL_FACTORY( SiDepositedCharge );

SiDepositedCharge::SiDepositedCharge(const std::string& type, 
                                     const std::string& name, 
                                     const IInterface* parent) : 
  GaudiTool( type, name, parent )
{
  // constructer
  declareProperty("delta2", m_delta2 = 1800*keV*keV/cm);
  declareProperty("scalingFactor", m_scalingFactor = 1.0);
 
  // need a line here to get the interface correct 
  declareInterface<ISiDepositedCharge>(this);
}

SiDepositedCharge::~SiDepositedCharge()
{
  // destructer
}

StatusCode SiDepositedCharge::initialize()
{ 
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize generators .
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator(Rndm::Gauss(0.,1.0),m_GaussDist.pRef());
  if (sc.isFailure()) return Error( "failed to init generator ", sc);

  sc = tRandNumSvc->generator(Rndm::Landau(0.2226,1.0),m_LandauDist.pRef());
  if (sc.isFailure()) return Error( "failed to init generator ", sc);

  release(tRandNumSvc);
  
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
  double scale = landauScale(beta,pathLength);  

  return ( landauMPV(beta,betaGamma,scale) + 
           (m_LandauDist->shoot()*scale) + 
           (m_GaussDist->shoot()*atomicBinding(pathLength)) ) /
    LHCbConstants::SiEnergyPerIonPair ;
}


double SiDepositedCharge::densityEffect(const double x) const
{
  // density effect due to Sternheimer 83
  double dEffect = 0.;
  if (x < 0.09666){
    // 0
  }
  else if ((x > 0.09666)&&(x<2.5)){
    dEffect = 4.606*x - 4.435 +(0.3755*(pow(3.0-x,2.72)));
  }
  else {
    dEffect = 4.606*x - 4.435;
  }
  return dEffect;
}

double SiDepositedCharge::landauMPV(const double beta, 
                                    const double betaGamma,
                                    const double scale) const
{
  // calculate density effect
  double dEffect = densityEffect(log10(betaGamma));  

  // MPV of landau
  double mpv = scale*( log(2*electron_mass_c2*gsl_pow_2(betaGamma)/
                           LHCbConstants::SiExcitationEnergy) + 
                       log(scale/LHCbConstants::SiExcitationEnergy) +
                       0.2 - gsl_pow_2(beta) - dEffect );
  return mpv;
}
