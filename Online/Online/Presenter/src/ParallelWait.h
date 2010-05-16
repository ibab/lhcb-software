// $Id: ParallelWait.h,v 1.7 2010-05-16 18:10:09 robbep Exp $

#ifndef PARALLELWAIT_H_
#define PARALLELWAIT_H_

class vector;
class DbRootHist;
class OnlineHistoOnPage;
class dbHistosOnPage;
class PresenterMainFrame;

class ParallelWait
{
 public:
  ParallelWait(PresenterMainFrame* presenter);
  virtual ~ParallelWait();								 
  void loadHistograms(const std::vector<OnlineHistoOnPage*> * onlineHistosOnPage,
		      std::vector<DbRootHist*> * dbHistosOnPage);
  void refreshHistograms(std::vector<DbRootHist*> * dbHistosOnPage);
 private:
  std::vector<std::string*> m_tasksNotRunning;
  std::vector<OnlineHistoOnPage*>::const_iterator m_onlineHistosOnPageIt;
  PresenterMainFrame* m_presenterApp;
};

// Declaration of call back functions
namespace Presenter {
  void getHistogramsFromLists(PresenterMainFrame * presenter, 
			      OnlineHistoOnPage* onlineHistosOnPage, 
			      std::vector<DbRootHist*> * dbHistosOnPage,
			      std::vector<std::string*> & tasksNotRunning) ;
  
  DbRootHist* getPageHistogram(PresenterMainFrame * presenter, 
			       const std::string & identifier,
			       const std::string & dimServiceName,
			       int refreshTime,
			       int instance,
			       OnlineHistDB* histogramDB,
			       OMAlib* analysisLib,
			       OnlineHistogram* onlineHist,
			       pres::MsgLevel verbosity,
			       DimBrowser* dimBrowser);
  
  void refreshHisto(DbRootHist* dbHistoOnPage) ;
};


#endif /*PARALLELWAIT_H_*/
