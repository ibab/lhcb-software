// $Id: LumiIntegrateFSR.h,v 1.5 2010-05-12 08:11:08 panmanj Exp $
#ifndef LUMIINTEGRATEFSR_H 
#define LUMIINTEGRATEFSR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// for DB
#include "GaudiKernel/IDetDataSvc.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/EventCountFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "LumiIntegrator.h"

/** @class LumiIntegrateFSR LumiIntegrateFSR.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-02-27
 */
class LumiIntegrateFSR : public GaudiAlgorithm {

public: 
  /// Standard constructor
  LumiIntegrateFSR( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiIntegrateFSR( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  virtual StatusCode add_file();            ///< add the FSRs of one input file
  virtual StatusCode add_fsr(LHCb::LumiIntegral* r, 
			     std::string addr, 
			     float factor);  ///< add/subtr one FSR for consistent group
  virtual std::vector< std::string > navigate(std::string rootname, std::string tag); 
  virtual void explore(IRegistry* pObj, std::string tag, std::vector< std::string >& a);
  virtual void add_to_xml();          ///< add counters to xmlfile at Algorithm finalization

  virtual StatusCode registerDB();    ///< register DB conditions
  std::vector<double> one_vector(std::vector<double> a, 
				 std::vector<double> b, int offset); ///< create one vector

protected:
  /// Reference to run records data service
  IDataProviderSvc* m_fileRecordSvc;
  SmartIF<IDetDataSvc> m_dds;                   ///< DetectorDataSvc

  std::string m_FileRecordName;                 ///< location of FileRecords
  std::string m_FSRName;                        ///< specific tag of summary data in FSR
  std::string m_EventCountFSRName;              ///< specific tag of event summary data in FSR
  std::string m_TimeSpanFSRName;                ///< specific tag of event summary data in FSR
  std::string m_ToolName;                       ///< name of tool for normalization
  std::string m_PrimaryBXType;                  ///< BXType to normalize
  std::vector<std::string> m_BXTypes;           ///< list of bunch crossing types
  std::vector<std::string> m_addBXTypes;        ///< list of bunch crossing types to be added
  std::vector<std::string> m_subtractBXTypes;   ///< list of bunch crossing types to be subtracted

private:
  ILumiIntegrator *m_integratorTool;            ///< tool to integrate luminosity
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
#endif // LUMIINTEGRATEFSR_H
