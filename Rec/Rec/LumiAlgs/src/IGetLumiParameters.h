// $Id: IGetLumiParameters.h,v 1.1 2010-11-20 08:17:50 panmanj Exp $
#ifndef IGETLUMIPARAMETERS_H 
#define IGETLUMIPARAMETERS_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IGetLumiParameters ( "IGetLumiParameters", 1, 0 );

/** @class IGetLumiParameters IGetLumiParameters.h
 *
 *  @author Jaap Panman
 *  @date   2010-11-20
 */
class IGetLumiParameters : virtual public IAlgTool {
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IGetLumiParameters; }

  virtual StatusCode init( std::string propertyConfigSvcName,
			   std::string instanceName, 
			   bool useOnline ) = 0;                ///< Init DB access

  virtual unsigned int getTCK() = 0;                            ///< retrieve TCK
  virtual long CollidingBunches() = 0;                          ///< retrieve number of colliding bunches
  virtual double OdinFraction() = 0;       		        ///< retrieve Odin Random BB fraction
  virtual double HLTRandomRate() = 0;      		        ///< retrieve random rate in HLT
  virtual double LHCFrequency() = 0;       		        ///< retrieve revolution frequency
  virtual double RandomRateBB() = 0;       		        ///< retrieve random bunch-bunch rate in HLT
  virtual std::vector<double> CalibRelative() = 0;          	///< relative calibration factors
  virtual std::vector<double> CalibCoefficients() = 0;      	///< usage factors
  virtual std::vector<double> CalibRelativeLog() = 0;       	///< relative calibration factors
  virtual std::vector<double> CalibCoefficientsLog() = 0;   	///< usage factors
  virtual double CalibScale() = 0;                          	///< absolute scale
  virtual double CalibScaleError() = 0;                     	///< absolute scale error
  virtual double StatusScale() = 0;                          	///< status scale flag
};
#endif // IGETLUMIPARAMETERS_H
