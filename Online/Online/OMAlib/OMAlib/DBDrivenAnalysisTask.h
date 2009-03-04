// $Id: DBDrivenAnalysisTask.h,v 1.6 2009-03-04 14:42:35 ggiacomo Exp $
#ifndef OMALIB_DBDRIVENANALYSISTASK_H
#define OMALIB_DBDRIVENANALYSISTASK_H 1
#include <map>
#include <string>
#include "OMAlib/AnalysisTask.h"
class OnlineHistogram;
class TH1;
class TFile;

class DBDrivenAnalysisTask : public AnalysisTask 
{
 public:
  DBDrivenAnalysisTask(const std::string& name, 
                       ISvcLocator* pSvcLocator ) :
    AnalysisTask( name, pSvcLocator ) {}
  virtual ~DBDrivenAnalysisTask ();
  virtual StatusCode initialize();
  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task = "any");

 private:
  std::map<std::string, TH1*> m_ownedHisto;
  std::map<std::string, TH1*> m_ownedRefs;
};



#endif // OMALIB_DBDRIVENANALYSISTASK_H
