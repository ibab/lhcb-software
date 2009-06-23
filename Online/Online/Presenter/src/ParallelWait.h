#ifndef PARALLELWAIT_H_
#define PARALLELWAIT_H_

#include <boost/thread.hpp>

class mutex;
class vector;
class DbRootHist;
class OnlineHistoOnPage;
class dbHistosOnPage;
class PresenterMainFrame;

void getHisto(PresenterMainFrame * gui, OnlineHistoOnPage* onlineHistosOnPage, std::vector<DbRootHist*> * dbHistosOnPage);

class ParallelWait
{
public:
	ParallelWait(PresenterMainFrame* gui);
	virtual ~ParallelWait();								 
void loadHistograms(const std::vector<OnlineHistoOnPage*> * onlineHistosOnPage,
                    std::vector<DbRootHist*> * dbHistosOnPage);
void refreshHistograms(std::vector<DbRootHist*> * dbHistosOnPage);
private:
  std::vector<std::string*> m_tasksNotRunning;
  std::vector<OnlineHistoOnPage*>::const_iterator m_onlineHistosOnPageIt;
  PresenterMainFrame* m_gui;
};

#endif /*PARALLELWAIT_H_*/
