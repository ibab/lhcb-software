//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineRootHist.h,v 1.5 2007-07-13 17:19:27 ggiacomo Exp $
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
  virtual bool setHistDisplayOption(std::string ParameterName, 
				    void* value);
  virtual bool setHistoPageDisplayOption(std::string ParameterName, 
					 void* value,
					 std::string PageName = "_DEFAULT_",
					 int Instance=-1);
  virtual bool setDisplayOption(std::string ParameterName, 
				void* value);

  virtual void Draw(Option_t* option="");
  virtual void DrawPanel();
  virtual TH1* myDrawCopy(Option_t* option="");

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
  OnlineRootHist* getNewRootHist(std::string Identifier,
				 std::string Page="_NONE_",
				 int Instance = 1);
  /// remove an histogram (TEMPORARY METHOD TO BE REMOVED AT PRODUCTION STAGE)
  bool removeRootHistogram(OnlineRootHist* h,
			   bool RemoveWholeSet = false);
 protected:
  void updateRootHistograms();  
 private:
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineRootHist*> m_myHist;
  bool m_avoid_dup;
};

#endif // ONLINEROOTHIST_H
