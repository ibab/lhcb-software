// $Id: AnalysisTask.h,v 1.16 2010-09-15 07:53:45 ggiacomo Exp $
#ifndef OMALIB_ANALYSISTASK_H 
#define OMALIB_ANALYSISTASK_H 1

/** @class AnalysisTask AnalysisTask.h OMAlib/AnalysisTask.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-18
 */
#include "OMAlib/OMAlib.h"
#include "GaudiAlg/GaudiHistoAlg.h"

class SavesetFinder;
class DimInfo;
class RunInfo;

class AnalysisTask : public OMAlib, public GaudiHistoAlg {
public:
  AnalysisTask( const std::string& name, 
                ISvcLocator* pSvcLocator );

  virtual ~AnalysisTask( );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// performs the analysis on the ROOT file in path Saveset for the given Task
  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task);
  /// opens DB session
  void openDBSession();
  /// check detector status at beginning of run
  void checkRunStatus(int run);
  /// returns current detector status bits
  long checkStatus();
  /// return run status mask
  inline long runStatus() {return m_runStatus;}
  /// list of available status conditions
  const std::vector<std::string>& conditionNames() {return m_conditionNames;}

protected:
  bool m_useDB;
  std::string m_DB;
  std::string m_DBuser;
  std::string m_DBpw;
  std::string m_myRefRoot;
  std::vector<std::string> m_inputFiles;
  std::vector<std::string> m_inputTasks;
  std::string m_partition;
  bool m_forceOnlineEnv;

  std::vector<SavesetFinder*> m_saveset;
  std::string m_RICHClDir;
  bool m_stayHere;
  bool m_checkStatus;
  bool m_messageCleanup;

  std::vector<DimInfo*> m_statusInfo;
  RunInfo* m_runInfo;
  long m_runStatus;
  bool m_runAvailable;
  std::vector<std::string> m_conditionNames;

private:
  void getAllTasks();
};
#endif // OMALIB_ANALYSISTASK_H
