// $Id: SiDepositedCharge.h,v 1.4 2007-03-23 14:49:53 cattanem Exp $
#ifndef SiDepositedCharge_H
#define SiDepositedCharge_H 1

// GSL
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/SystemOfUnits.h"

// Interface from MCEvent
#include "Kernel/ISiDepositedCharge.h"

/** @class SiDepositedCharge SiDepositedCharge.h
 *
 *  Landau convolved with a Gaussian - see <a href="http://cdsweb.cern.ch/record/690291">LHCb 2003-160</a>  
 *
 *  @author M.Needham
 *  @date   13/3/2002
 */

class SiDepositedCharge : public GaudiTool, virtual public ISiDepositedCharge {

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

  /// Calculate the atomic binding effect
  double atomicBinding( const double pathLength ) const;

  /// Calculate scale of the Landau function (scales with the thickness)
  double landauScale( const double beta, const double pathLength ) const;

  /// Calculate the density effect (needed to get the MPV of the Landau)
  double densityEffect( const double x ) const;

  /// Calculate the most probable value of a Landau function
  double landauMPV( const double beta, const double betaGamma, 
                    const double scale) const;

  // Random number generators
  SmartIF<IRndmGen> m_GaussDist;
  SmartIF<IRndmGen> m_LandauDist;

  // job options
  double m_delta2;        ///< parameter for the atomic binding effect
  double m_scalingFactor; ///< Scaling factor for path length through sensor 
 
};

inline double SiDepositedCharge::atomicBinding(const double pathLength) const
{
  return sqrt(m_delta2*pathLength);
}

inline double SiDepositedCharge::landauScale(const double beta, 
                                             const double pathLength) const
{
  return 0.017825*pathLength*Gaudi::Units::micrometer / 
          (gsl_pow_2(GSL_MAX(beta,0.1))*Gaudi::Units::keV);
}

#endif // SiDepositedCharge_H
