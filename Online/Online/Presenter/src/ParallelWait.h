#ifndef PARALLELWAIT_H_
#define PARALLELWAIT_H_

// #include <boost/thread.hpp>
// class mutex;

class vector;
class DbRootHist;
class OnlineHistoOnPage;
class dbHistosOnPage;
class PresenterMainFrame;

DbRootHist* getHistogram(PresenterMainFrame * presenter, 
                         const std::string & identifier,
                         const std::string & dimServiceName,
                         int refreshTime,
                         int instance,
                         OnlineHistDB* histogramDB,
                         OMAlib* analysisLib,
                         OnlineHistogram* onlineHist,
                         pres::MsgLevel verbosity,
                         DimBrowser* dimBrowser);
void getHistogramsFromLists(PresenterMainFrame * presenter, OnlineHistoOnPage* onlineHistosOnPage, std::vector<DbRootHist*> * dbHistosOnPage);

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

#endif /*PARALLELWAIT_H_*/
