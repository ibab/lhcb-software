// $Id: GetLumiParameters.h,v 1.1 2009-06-12 08:17:50 panmanj Exp $
#ifndef GETLUMIPARAMETERS_H 
#define GETLUMIPARAMETERS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IGetLumiParameters.h"            // Interface

// CondDB
#include "DetDesc/Condition.h"
#include "GaudiKernel/IDetDataSvc.h"

// TCK
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/TCK.h"
#include "GaudiKernel/SmartDataPtr.h" 

/** @class GetLumiParameters GetLumiParameters.h
 *
 *  @author Jaap Panman
 *  @date   2009-03-16
 */
class GetLumiParameters : public GaudiTool, virtual public IGetLumiParameters {
public: 
  /// Standard constructor
  GetLumiParameters( const std::string& type, 
		     const std::string& name,
		     const IInterface* parent);

  virtual ~GetLumiParameters( );                ///< Destructor

  StatusCode init( std::string propertyConfigSvcName,
		   std::string instanceName, 
		   bool useOnline );            ///< Init DB access
  
  unsigned int getTCK();              	        ///< retrieve TCK
  long CollidingBunches();                      ///< retrieve number of colliding bunches
  double OdinFraction();       	                ///< retrieve Odin Random BB fraction
  double HLTRandomRate();      	                ///< retrieve random rate in HLT
  double LHCFrequency();       	                ///< retrieve revolution frequency
  double RandomRateBB();       	                ///< retrieve random bunch-bunch rate in HLT
  std::vector<double> CalibRelative();          ///< relative calibration factors
  std::vector<double> CalibCoefficients();      ///< usage factors
  std::vector<double> CalibRelativeLog();       ///< relative calibration factors
  std::vector<double> CalibCoefficientsLog();   ///< usage factors
  double CalibScale();                          ///< absolute scale
  double CalibScaleError();                     ///< absolute scale error
  double StatusScale();                         ///< status scale flag

protected:
  virtual StatusCode registerDB();   	        ///< register DB conditions
  virtual StatusCode processDB();               ///< DB checking code
  virtual double rateFromTCK(unsigned int tck); ///< get the lumi rate from the TCK
  SmartIF<IDetDataSvc> m_dds;                   ///< DetectorDataSvc

private:
  std::string m_ToolName;                       ///< name of tool 
  bool m_initialized;                           ///< flag

  StatusCode i_cacheRelativeData();             ///< Function extracting data from Condition
  StatusCode i_cacheRelativeDataLog();          ///< Function extracting data from Condition
  StatusCode i_cacheAbsoluteData();             ///< Function extracting data from Condition
  StatusCode i_cacheCoefficientData();          ///< Function extracting data from Condition
  StatusCode i_cacheCoefficientDataLog();       ///< Function extracting data from Condition
  StatusCode i_cacheSamplingData();             ///< Function extracting data from Condition
  StatusCode i_cacheTriggerData();              ///< Function extracting data from Condition
  StatusCode i_cacheFillingData();              ///< Function extracting data from Condition

  // database conditions and calibration factors
  Condition *m_condRelative;                    ///< Condition for relative calibration
  Condition *m_condRelativeLog;                 ///< Condition for relative calibration
  Condition *m_condAbsolute;                    ///< Condition for absolute scale
  Condition *m_condCoefficients;                ///< Condition for usage coefficients
  Condition *m_condCoefficientsLog;             ///< Condition for usage coefficients
  Condition *m_condSampling;                    ///< Condition for sampling coefficients
  Condition *m_condTrigger;                     ///< Condition for sampling coefficients
  Condition *m_condFilling;                     ///< Condition for LHC filling scheme
  
  std::vector<double> m_calibRelative;          ///< relative calibration factors
  std::vector<double> m_calibCoefficients;      ///< usage factors
  std::vector<double> m_calibRelativeLog;       ///< relative calibration factors
  std::vector<double> m_calibCoefficientsLog;   ///< usage factors
  double m_statusScale;                         ///< absolute scale  set to zero if no lumi
  double m_calibScale;                          ///< absolute scale
  double m_calibScaleError;                     ///< absolute scale error
  double m_calibRevolutionFrequency;            ///< revolution frequency (Hz)
  double m_calibRandomFrequencyBB;              ///< random lumi event frequency of BB crossings (Hz)
  long   m_calibCollidingBunches;               ///< number of colliding bunches
  unsigned int m_triggerTCK;                    ///< tck for these data
  unsigned int m_knownTCK;                      ///< previous value
  std::vector<double> m_lumiPars;               ///< trigger LumiPars
  double m_odinTotalRate;                       ///< total ODIN random rate 
  double m_odinFraction;                        ///< fraction of total spent during BB
  double m_rateHLT;                             ///< random lumi rate set by HLT line
  double m_rateBB;                              ///< random lumi rate set by HLT line for BB
  long 	 m_B1NBunches;            		///< filling scheme data
  long 	 m_B2NBunches;            		///< filling scheme data
  long 	 m_NCollidingBunches;     		///< filling scheme data
  long 	 m_B1WrongBucketFlag;     		///< filling scheme data
  long 	 m_B2WrongBucketFlag;     		///< filling scheme data
  long 	 m_onlineCollidingBunches;              ///< number of colliding bunches

  mutable IPropertyConfigSvc * m_propertyConfigSvc;
  std::string m_propertyConfigSvcName;
  std::string m_instanceName;
  bool m_useOnline;                             ///< flag to use online partition of DB

};
#endif // GETLUMIPARAMETERS_H
