#ifndef DBDRIVENANALYSISTASK_H
#define DBDRIVENANALYSISTASK_H
#include <map>
#include "OMAlib/AnalysisTask.h"
class OnlineHistogram;
class TH1;
class TFile;
class DBDrivenAnalysisTask : public AnalysisTask 
{
 public:
  DBDrivenAnalysisTask() {};
  virtual ~DBDrivenAnalysisTask ();  
  virtual bool exec(std::string SaveSet);

 private:
  std::map<std::string, TH1*> m_ownedHisto;
  TH1* onTheFlyHistogram(OnlineHistogram* h,
			 TFile* f);
};



#endif
