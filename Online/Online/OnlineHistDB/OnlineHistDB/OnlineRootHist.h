//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineRootHist.h,v 1.1 2007-07-09 10:17:42 ggiacomo Exp $
#ifndef ONLINEROOTHIST_H
#define ONLINEROOTHIST_H 1
#include "OnlineHistDB/OnlineHistogram.h"
#include "TH1.h"
#include "TStyle.h"

class OnlineRootHist : public OnlineHistogram,  public TH1
{
 public:
  OnlineRootHist(OnlineHistDBEnv& Env,
		 std::string Identifier,
		 std::string Page="_NONE_",
		 int Instance=1);
  /// update ROOT TH1 display properties from Histogram DB (via OnlineHistogram object) 
  void setFromDB();
  /// update current drawing options from Histogram DB (via OnlineHistogram object)
  void setDrawOptions();
  /// save current ROOT display options to OnlineHistogram object and to Histogram DB
  bool saveToDB();
  /// overloaded methods for setting display options  
  virtual bool setHistoSetDisplayOption(std::string ParameterName, 
					void* value);
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);
  virtual bool setHistoPageDisplayOption(std::string ParameterName, 
					 void* value,
					 std::string PageName = "_DEFAULT_",
					 int Instance=-1);
};


class OnlineRootHistStorage
{
 public:
  OnlineRootHistStorage(OnlineHistDBEnv* Env);
  virtual ~OnlineRootHistStorage();
  /// get an OnlineHistogram object, holding informations of an existing histogram, that can be used to view/edit an histogram record
  OnlineRootHist* getRootHist(std::string Identifier,
			       std::string Page="_NONE_",
			       int Instance = 1);
  /// remove an histogram (TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE)
  bool removeHistogram(OnlineRootHist* h,
		       bool RemoveWholeSet = false);
 protected:
  void updateRootHistograms();  
 private:
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineRootHist*> m_myHist;
};

#endif // ONLINEROOTHIST_H
