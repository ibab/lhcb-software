//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OnlineRootHist.h,v 1.7 2007-07-17 15:54:13 ggiacomo Exp $
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
  /// updates ROOT TH1 display properties from Histogram DB (via OnlineHistogram object) 
  /// called by constructor
  void setFromDB();
  /// updates current drawing options from Histogram DB (via OnlineHistogram object)
  void setDrawOptions();
  /// saves current ROOT display options to OnlineHistogram object and to Histogram DB
  bool saveToDB();
  // overloaded OnlineHistogram methods for setting display options  
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

  // overloaded TH1 drawing methods
  /// overloaded TH1 Draw method, calls setDrawOptions()
  virtual void Draw(Option_t* option="");
  /// overloaded TH1 DrawPanel method, calls setDrawOptions()
  virtual void DrawPanel();
  /// calls TH1 DrawCopy method and setDrawOptions()
  virtual TH1* myDrawCopy(Option_t* option="");

};


class OnlineRootHistStorage
{
 public:
  OnlineRootHistStorage(OnlineHistDBEnv* Env);
  virtual ~OnlineRootHistStorage();
  /// gets a pointer to an OnlineRootHist object that can be used to view/edit an histogram record. If Page
  /// is specified, the default display options for the histogram are those associated to the page (if available).
  /// Uses cached histogram objects if available
  OnlineRootHist* getRootHist(std::string Identifier,
			       std::string Page="_NONE_",
			       int Instance = 1);
  /// same as getRootHist, but a new object is always created (no caching)
  OnlineRootHist* getNewRootHist(std::string Identifier,
				 std::string Page="_NONE_",
				 int Instance = 1);
  virtual bool removeHistogram(OnlineRootHist* h,
			       bool RemoveWholeSet = false);
 protected:
  void updateRootHistograms();  
 private:
  OnlineHistDBEnv* m_Histenv;
  std::vector<OnlineRootHist*> m_myHist;
  bool m_avoid_dup;
};

#endif // ONLINEROOTHIST_H
