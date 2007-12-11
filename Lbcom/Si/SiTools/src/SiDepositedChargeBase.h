// $Id: SiDepositedChargeBase.h,v 1.1 2007-12-11 10:14:25 mneedham Exp $
#ifndef SiDepositedChargeBase_H
#define SiDepositedChargeBase_H 1

// GSL
#include "gsl/gsl_math.h"

// Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"
#include "GaudiKernel/SystemOfUnits.h"

// Interface
#include "MCInterfaces/ISiDepositedCharge.h"

/** @class SiDepositedChargeBase SiDepositedChargeBase.h
 *
 * Base class for deposited charge tools
 *
 *  @author M.Needham
 *  @date   11/12/2007
 */

class SiDepositedChargeBase : public GaudiTool, virtual public ISiDepositedCharge {

public: 

  /** Constructor */
  SiDepositedChargeBase( const std::string& type, const std::string& name,
                     const IInterface* parent );


  /** destructor */ 
  virtual ~SiDepositedChargeBase();

  /** initialize */
  virtual StatusCode initialize();

 
protected:

  /// Calculate the atomic binding effect
  double atomicBinding( const double pathLength ) const;


  // Random number generators
  SmartIF<IRndmGen> m_GaussDist;

  // job options
  double m_delta2;        ///< parameter for the atomic binding effect
  double m_scalingFactor; ///< Scaling factor for path length through sensor 
 
};

inline double SiDepositedChargeBase::atomicBinding(const double pathLength) const
{
  return sqrt(m_delta2*pathLength);
}



#endif // SiDepositedChargeBase_H
