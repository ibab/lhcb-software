#ifndef LUMIMERGEFSR_H 
#define LUMIMERGEFSR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/TimeSpanFSR.h"

// local
#include "FSRNavigator.h"

/** @class LumiMergeFSR LumiMergeFSR.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2010-10-05
 */
class LumiMergeFSR : public GaudiAlgorithm {

public: 
  /// Standard constructor
  LumiMergeFSR( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiMergeFSR( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  virtual StatusCode merge();         ///< merge the FSRs of one input file

protected:
  /// Reference to run records data service
  IDataProviderSvc* m_fileRecordSvc;

  std::string m_FileRecordName;                 ///< location of FileRecords
  std::string m_FSRName;                        ///< specific tag of summary data in FSR
  std::string m_TimeSpanFSRName;                ///< specific tag of time summary data in FSR
  std::string m_EventCountFSRName;              ///< specific tag of event summary data in FSR
  std::string m_ToolName;                       ///< name of tool for normalization
  std::string m_PrimaryBXType;                  ///< BXType to normalize
  std::vector<std::string> m_BXTypes;           ///< list of bunch crossing types
  std::vector<std::string> m_subtractBXTypes;   ///< list of bunch crossing types to be subtracted

  std::vector<LHCb::LumiFSRs*> m_lumiFSRsVec;   ///< TDS containers
  std::vector<std::string> m_FSRNameVec;        ///< TDS container names
  LHCb::TimeSpanFSRs* m_timeSpanFSRs;           ///< TDS container

private:
  IFSRNavigator *m_navigatorTool;               ///< tool navigate FSRs

};
#endif // LUMIMERGEFSR_H
