#include "Kernel/LHCbConstants.h"
#include "Kernel/SiLandauFun.h"
#include <math.h>

double SiLandauFun::MPV(const double beta,
                        const double gamma,
                        const double pathlength) {

  const double theScale = scale(beta, pathlength);
  return MPVFromScale(beta, gamma, theScale);
}

double SiLandauFun::densityEffect(const double x)
{
  // density effect due to Sternheimer 83
  double dEffect = 0.;
  if (x < 0.09666){
    // 0
  }
  else if ((x > 0.09666)&&(x<2.5)){
    dEffect = 4.606*x - 4.435 +(0.3755*(pow(2.5-x,2.72)));
  }
  else {
    dEffect = 4.606*x - 4.435;
  }
  return dEffect;
}

double SiLandauFun::MPVFromScale(const double beta,
                                      const double gamma,
                                      const double scale)
{
  // calculate density effect
  const double x = log10(beta*gamma);
  const double dEffect = densityEffect(x);

  const double beta2 = beta*beta;
  const double betaGamma2 = beta2*gamma*gamma;

  // MPV of landau
  double mpv = scale*( log(2*Gaudi::Units::electron_mass_c2*betaGamma2/
                       LHCbConstants::SiExcitationEnergy) +
                       log(scale/LHCbConstants::SiExcitationEnergy) +
                       0.2 - beta2 - dEffect);
  return mpv;
}
