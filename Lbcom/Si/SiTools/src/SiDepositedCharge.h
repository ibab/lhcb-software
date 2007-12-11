// $Id: SiDepositedCharge.h,v 1.6 2007-12-11 10:14:25 mneedham Exp $
#ifndef SiDepositedCharge_H
#define SiDepositedCharge_H 1

// GSL
#include "gsl/gsl_math.h"

// Gaudi
#include "SiDepositedChargeBase.h"


// Interface
#include "MCInterfaces/ISiDepositedCharge.h"

/** @class SiDepositedCharge SiDepositedCharge.h
 *
 *  Landau convolved with a Gaussian - see <a href="http://cdsweb.cern.ch/record/690291">LHCb 2003-160</a>  
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiDepositedCharge : public SiDepositedChargeBase, virtual public ISiDepositedCharge {

public: 

  /** Constructor */
  SiDepositedCharge( const std::string& type, const std::string& name,
                     const IInterface* parent );

  /** destructor */
  virtual ~SiDepositedCharge();

  /** initialize */
  virtual StatusCode initialize();
     
  /** calculate deposited charge (in electrons)
  * @param  aHit hit 
  * @return deposited charge 
  */
  double charge(const LHCb::MCHit* aHit) const;

private:

 
  /// Calculate scale of the Landau function (scales with the thickness)
  double landauScale( const double beta, const double pathLength ) const;

  /// Calculate the density effect (needed to get the MPV of the Landau)
  double densityEffect( const double x ) const;

  /// Calculate the most probable value of a Landau function
  double landauMPV( const double beta, const double betaGamma, 
                    const double scale) const;

 
  SmartIF<IRndmGen> m_LandauDist;
 
};

inline double SiDepositedCharge::landauScale(const double beta, 
                                             const double pathLength) const
{
  return 0.017825*pathLength*Gaudi::Units::micrometer / 
          (gsl_pow_2(GSL_MAX(beta,0.1))*Gaudi::Units::keV);
}

#endif // SiDepositedCharge_H
