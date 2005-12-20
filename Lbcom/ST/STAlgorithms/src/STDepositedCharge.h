// $Id: STDepositedCharge.h,v 1.2 2005-12-20 15:50:25 cattanem Exp $
#ifndef _STDepositedCharge_H
#define _STDepositedCharge_H

#include <vector>
#include "gsl/gsl_math.h"

#include "ISTDepositedCharge.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/SystemOfUnits.h"


/** @class STDepositedCharge STDepositedCharge.h ITAlgorithms/STDepositedCharge.h
 *
 *  Class for estimating noise in IT Channel
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */


class STDepositedCharge : public GaudiTool, virtual public ISTDepositedCharge {

public: 

  /// Constructer
  STDepositedCharge(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /// destructer
  virtual ~STDepositedCharge();

  /// init
  virtual StatusCode initialize();

  /// calc noise
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

inline double STDepositedCharge::atomicBinding(const double pathLength) const{

  return sqrt(m_delta2*pathLength);
}

inline double STDepositedCharge::landauScale(const double beta, const double pathLength) const{
  return (0.017825*pathLength*micrometer/(pow(GSL_MAX(beta,0.1),2.)*keV));
}

#endif // _STDepositedCharge_H
