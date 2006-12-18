// $Id: SiDepositedCharge.h,v 1.2 2006-12-18 10:15:27 cattanem Exp $
#ifndef _SiDepositedCharge_H
#define _SiDepositedCharge_H

#include <vector>
#include "gsl/gsl_math.h"

#include "Kernel/ISiDepositedCharge.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiAlg/GaudiTool.h"

#include "GaudiKernel/SystemOfUnits.h"


/** @class SiDepositedCharge SiDepositedCharge.h SiTools/SiDepositedCharge.h
 *
 * Landau convolved with a Gaussian - see LHCb 2003-160  
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */


class SiDepositedCharge : public GaudiTool, virtual public ISiDepositedCharge {

public: 

  /** Constructer */
  SiDepositedCharge(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /** destructer */
  virtual ~SiDepositedCharge();

  /** initialize */
  virtual StatusCode initialize();

     
  /** calculate deposited charge (in electrons)
  * @param hit 
  * @return deposited charge 
  */
  double charge(const LHCb::MCHit* aHit) const;

private:

  double atomicBinding(const double pathLength) const;
  double landauScale(const double beta, const double pathLength) const;
  double densityEffect(const double x) const;
  double landauMPV(const double beta, const double betaGamma, const double scale) const;

  SmartIF<IRndmGen> m_GaussDist;
  SmartIF<IRndmGen> m_LandauDist;

  double m_delta2;
  double m_scalingFactor;
 
};

inline double SiDepositedCharge::atomicBinding(const double pathLength) const{

  return sqrt(m_delta2*pathLength);
}

inline double SiDepositedCharge::landauScale(const double beta, const double pathLength) const{
  return (0.017825*pathLength*Gaudi::Units::micrometer/(gsl_pow_2(GSL_MAX(beta,0.1))*Gaudi::Units::keV));
}

#endif // _SiDepositedCharge_H
