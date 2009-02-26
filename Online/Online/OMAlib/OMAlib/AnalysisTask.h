// $Id: AnalysisTask.h,v 1.9 2009-02-26 13:59:18 ggiacomo Exp $
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

  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task);

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
};
#endif // OMALIB_ANALYSISTASK_H
