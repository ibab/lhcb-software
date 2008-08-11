// $Id: DBDrivenAnalysisTask.h,v 1.4 2008-08-11 08:05:15 ggiacomo Exp $
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
  TH1* onTheFlyHistogram(OnlineHistogram* h,
			 TFile* f);
};



#endif // OMALIB_DBDRIVENANALYSISTASK_H
