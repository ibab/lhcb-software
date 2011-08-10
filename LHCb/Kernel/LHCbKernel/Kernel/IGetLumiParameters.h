// $Id: IGetLumiParameters.h,v 1.1 2010-11-20 08:17:50 panmanj Exp $
#ifndef IGETLUMIPARAMETERS_H 
#define IGETLUMIPARAMETERS_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IGetLumiParameters ( "IGetLumiParameters", 1, 1 );

/** @class IGetLumiParameters IGetLumiParameters.h
 *
 *  Interface for Luminosity parameters tool
 *
 *  @author Jaap Panman
 *  @date   2010-11-20
 */
class IGetLumiParameters : virtual public IAlgTool {
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGetLumiParameters; }

  virtual unsigned int getTCK() const = 0;                            ///< retrieve TCK
  virtual long CollidingBunches() = 0;                                ///< retrieve number of colliding bunches
  virtual double OdinFraction() const  = 0;                           ///< retrieve Odin Random BB fraction
  virtual double HLTRandomRate() const  = 0;                          ///< retrieve random rate in HLT
  virtual double LHCFrequency()  const = 0;                           ///< retrieve revolution frequency
  virtual double RandomRateBB() = 0;                                  ///< retrieve random bunch-bunch rate in HLT
  virtual std::vector<double> CalibRelative() const = 0;          	  ///< relative calibration factors
  virtual std::vector<double> CalibCoefficients()  const= 0;      	  ///< usage factors
  virtual std::vector<double> CalibRelativeLog() const = 0;       	  ///< relative calibration factors
  virtual std::vector<double> CalibCoefficientsLog() const = 0;   	  ///< usage factors
  virtual double CalibScale()  const= 0;                          	  ///< absolute scale
  virtual double CalibScaleError()  const= 0;                     	  ///< absolute scale error
  virtual double StatusScale() const = 0;                          	  ///< status scale flag
};
#endif // IGETLUMIPARAMETERS_H
