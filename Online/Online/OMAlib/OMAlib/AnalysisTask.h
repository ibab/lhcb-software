// $Id: AnalysisTask.h,v 1.5 2008-08-19 22:45:31 ggiacomo Exp $
#ifndef OMALIB_ANALYSISTASK_H 
#define OMALIB_ANALYSISTASK_H 1

/** @class AnalysisTask AnalysisTask.h OMAlib/AnalysisTask.h
 *  
 *
 *  @author Giacomo Graziani
 *  @date   2008-02-18
 */
#include "OMAlib/OMAlib.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class SavesetFinder;

class AnalysisTask : public OMAlib, public GaudiAlgorithm {
public:
  AnalysisTask( const std::string& name, 
                ISvcLocator* pSvcLocator );

  virtual ~AnalysisTask( );

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task) = 0;

protected:
  bool m_useDB;
  std::string m_DB;
  std::string m_DBuser;
  std::string m_DBpw;
  std::string m_myRefRoot;
  std::vector<std::string> m_inputFiles;
  std::vector<std::string> m_inputTasks;
  std::string m_MessageTool;
  std::vector<SavesetFinder*> m_saveset;
};
#endif // OMALIB_ANALYSISTASK_H
