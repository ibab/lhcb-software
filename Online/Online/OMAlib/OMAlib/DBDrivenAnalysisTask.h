// $Id: DBDrivenAnalysisTask.h,v 1.7 2010-06-11 13:00:10 ggiacomo Exp $
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
                       ISvcLocator* pSvcLocator );
  virtual ~DBDrivenAnalysisTask ();
  virtual StatusCode initialize();
  virtual StatusCode analyze(std::string& SaveSet,
                             std::string Task = "any");

 private:
  void sendNoGoMessage(TH1* h);
  std::map<std::string, TH1*> m_ownedHisto;
  std::map<std::string, TH1*> m_ownedRefs;
};



#endif // OMALIB_DBDRIVENANALYSISTASK_H
