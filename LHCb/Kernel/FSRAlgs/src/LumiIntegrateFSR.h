// $Id: LumiIntegrateFSR.h,v 1.5 2010-05-12 08:11:08 panmanj Exp $
#ifndef LUMIINTEGRATEFSR_H 
#define LUMIINTEGRATEFSR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// for DB
#include "GaudiKernel/IDetDataSvc.h"
#include "Kernel/IAccept.h"

// for TCK
#include "Kernel/IPropertyConfigSvc.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/EventCountFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "GetLumiParameters.h"
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
  virtual StatusCode stop      ();    ///< Algorithm stopping
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  virtual StatusCode add_file();            ///< add the FSRs of one input file
  virtual StatusCode add_fsr(LHCb::LumiIntegral* r, 
			     std::string addr, 
			     float factor, unsigned int key);           ///< add/subtr one FSR for consistent group
  LHCb::TimeSpanFSR* trigger_event(std::string addr, unsigned int key); ///< trigger database update
  virtual void add_to_xml();          ///< add counters to xmlfile at Algorithm finalization

  std::vector<double> one_vector(std::vector<double> a, 
				 std::vector<double> b, int offset); ///< create one vector

protected:
  /// Reference to run records data service
  IDataProviderSvc* m_fileRecordSvc;            ///< file record service
  SmartIF<IDetDataSvc> m_dds;                   ///< DetectorDataSvc
  ILumiIntegrator *m_integratorTool;            ///< tool to integrate luminosity
  IGetLumiParameters *m_databaseTool;           ///< tool to query luminosity database
  IAccept *m_acceptTool;                        ///< Pointer to the IAccept tool

  std::string m_FileRecordName;                 ///< location of FileRecords
  std::string m_FSRName;                        ///< specific tag of summary data in FSR
  std::string m_EventCountFSRName;              ///< specific tag of event summary data in FSR
  std::string m_TimeSpanFSRName;                ///< specific tag of event summary data in FSR
  std::string m_ToolName;                       ///< name of tool for normalization
  std::string m_acceptToolName;                 ///< Name of the (public) IAccept data quality tool
  std::string m_PrimaryBXType;                  ///< BXType to normalize
  std::vector<std::string> m_BXTypes;           ///< list of bunch crossing types
  std::vector<std::string> m_addBXTypes;        ///< list of bunch crossing types to be added
  std::vector<std::string> m_subtractBXTypes;   ///< list of bunch crossing types to be subtracted
  bool m_accumulateMu;                          ///< flag to accumulate mu information
  std::string m_muKeyName;                      ///< name of key for mu calculation
  int m_MuKey;                                  ///< int value of key for mu calculation
  bool m_ignoreDQFlags;                         ///< flag to ignore data quality  

private:
  // database calibration factors
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
  bool   m_DQaccepted;                          ///< DQ flag accepted 
  
};
#endif // LUMIINTEGRATEFSR_H
