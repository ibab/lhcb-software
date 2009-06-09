// $Id: AnalysisTask.h,v 1.12 2009-06-09 17:34:10 ggiacomo Exp $
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

class AnalysisTask : public OMAlib, public GaudiHistoAlg {
public:
  AnalysisTask( const std::string& name, 
                ISvcLocator* pSvcLocator );

  virtual ~AnalysisTask( );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  /// performs the analysis on the ROOT file in path Saveset for the given Task
  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task);
  /// opens DB session
  void openDBSession();

protected:
  bool m_useDB;
  std::string m_DB;
  std::string m_DBuser;
  std::string m_DBpw;
  std::string m_myRefRoot;
  std::vector<std::string> m_inputFiles;
  std::vector<std::string> m_inputTasks;
  std::vector<SavesetFinder*> m_saveset;
  std::string m_partition;
  std::string m_RICHClDir;
  

private:
  void getAllTasks();
};
#endif // OMALIB_ANALYSISTASK_H
