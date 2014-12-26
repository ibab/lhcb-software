#ifndef SiLandauFun_H
#define SiLandauFun_H 1

#include "GaudiKernel/SystemOfUnits.h"
#include <algorithm>

/** @file
 *  All the fun you can have with Landaus in Silicon
 *  Functions for MPV, scale , density effect
 *  @author M Needham
 *  @date 20100622
 */


namespace SiLandauFun {


  /** Calculate scale of the Landau function (scales with the thickness)
   * @param beta v/c
   * @param pathLength  how much silicon traversed
   */
  double scale( const double beta, const double pathLength ) ;

  /** Calculate the density effect (needed to get the MPV of the Landau)
   * @param x log10(betaGamma)
   */
  double densityEffect( const double x ) ;

  /** MPV of the landau
  * @param beta v/c
  * @param betaGamma P/m
  * @param pathLength  how much silicon traversed
  */
  double MPV(const double beta, const double betaGamma,
             const double pathlength);

  /** Calculate the most probable value of a Landau function
  * This is a CPU optimization of the MPV function if you know the scale
  * @param beta v/c
  * @param betaGamma P/m
  * @param scale
  */
  double MPVFromScale( const double beta, const double betaGamma,
                    const double scale);

}

inline double SiLandauFun::scale(const double beta,
                                             const double pathLength)
{

  // avoid division by zero at beta = 0
  const double betaTrunc = std::max(beta,0.1);
  return 0.017825*pathLength*Gaudi::Units::micrometer /
    (betaTrunc*betaTrunc*Gaudi::Units::keV);
}


#endif // HISTFUN_H
