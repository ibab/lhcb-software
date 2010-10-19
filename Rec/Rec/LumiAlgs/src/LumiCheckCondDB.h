// $Id: LumiCheckCondDB.h,v 1.5 2010-05-12 08:11:08 panmanj Exp $
#ifndef LUMICHECKCONDDB_H 
#define LUMICHECKCONDDB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// for DB
#include "GaudiKernel/IDetDataSvc.h"

/** @class LumiCheckCondDB LumiCheckCondDB.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2010-10-20
 */
class LumiCheckCondDB : public GaudiAlgorithm {

public: 
  /// Standard constructor
  LumiCheckCondDB( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiCheckCondDB( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  virtual StatusCode registerDB();    ///< register DB conditions

protected:
  /// Reference to run records data service
  SmartIF<IDetDataSvc> m_dds;                   ///< DetectorDataSvc

private:
  unsigned long m_startTime;                    ///< start probing database here
  unsigned long m_numberSteps;                  ///< number of months checked

  StatusCode i_cacheRelativeData();             ///< Function extracting data from Condition
  StatusCode i_cacheRelativeDataLog();          ///< Function extracting data from Condition
  StatusCode i_cacheAbsoluteData();             ///< Function extracting data from Condition
  StatusCode i_cacheCoefficientData();          ///< Function extracting data from Condition
  StatusCode i_cacheCoefficientDataLog();       ///< Function extracting data from Condition
  StatusCode i_cacheSamplingData();             ///< Function extracting data from Condition

  // database conditions and calibration factors
  Condition *m_condRelative;                    ///< Condition for relative calibration
  Condition *m_condRelativeLog;                 ///< Condition for relative calibration
  Condition *m_condAbsolute;                    ///< Condition for absolute scale
  Condition *m_condCoefficients;                ///< Condition for usage coefficients
  Condition *m_condCoefficientsLog;             ///< Condition for usage coefficients
  Condition *m_condSampling;                    ///< Condition for sampling coefficients
  Condition *m_condGUIDs;                       ///< Condition for GUID based constants
  
  std::vector<double> m_calibRelative;          ///< relative calibration factors
  std::vector<double> m_calibCoefficients;      ///< usage factors
  std::vector<double> m_calibRelativeLog;       ///< relative calibration factors
  std::vector<double> m_calibCoefficientsLog;   ///< usage factors
  double m_statusScale;                         ///< absolute scale  set to zero if no lumi
  double m_calibScale;                          ///< absolute scale
  double m_calibScaleError;                     ///< absolute scale error
  double m_calibRevolutionFrequency;            ///< revolution frequency (Hz)
  double m_calibRandomFrequencyBB;              ///< random lumi event frequency of BB crossings (Hz)
  int    m_calibCollidingBunches;               ///< number of colliding bunches

};
#endif // LUMICHECKCONDDB_H
