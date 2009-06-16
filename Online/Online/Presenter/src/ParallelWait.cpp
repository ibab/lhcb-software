// Separate file/class to prevent CINT crushing onto Boost headers via LinkDef.
#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

#include "dim/dic.hxx"
#include "presenter.h"
#include "PresenterMainFrame.h"
#include "Archive.h"
#include "ParallelWait.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "DbRootHist.h"
#include "TThread.h"

using namespace pres;
using namespace std;
using namespace boost;

boost::mutex listMutex;
boost::mutex oraMutex;
boost::mutex dimMutex;
boost::mutex rootMutex;
boost::mutex loadAnaMutex;

void getHisto(PresenterMainFrame * gui, OnlineHistoOnPage* onlineHistosOnPage, std::vector<DbRootHist*> * dbHistosOnPage)
{
	  DbRootHist* dbRootHist;
	  if((History == gui->presenterMode()) || (EditorOffline == gui->presenterMode())) { // no need for DIM
	    dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
	                                std::string(""),
	                                2, onlineHistosOnPage->instance,
	                                gui->histogramDB(), gui->analysisLib(),
	                                onlineHistosOnPage->histo,
	                                gui->verbosity(),
	                                NULL,
	                                oraMutex,
	                                dimMutex,
                                  rootMutex,
                                  loadAnaMutex);
	  } else if((Online == gui->presenterMode()) || (EditorOnline == gui->presenterMode())) {
//	    bool taskNotRunning(false);             
//	    for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
//	         m_tasksNotRunningIt != m_tasksNotRunning.end(); m_tasksNotRunningIt++) {
//	      if (0 == ((*m_tasksNotRunningIt).compare(onlineHistosOnPage->histo->task()))) {
//	        taskNotRunning = true;            
//	      }                          
//	    }
	    DimBrowser* dimBrowser = NULL;
	    std::string currentPartition("");
//	    if (taskNotRunning) {
//	      dimBrowser = NULL;
//	      currentPartition = "";
//	    } else {
	      dimBrowser = gui->dimBrowser();
	      currentPartition = gui->currentPartition();
//	    } 
	    dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
	                                currentPartition,
	                                2, onlineHistosOnPage->instance,
	                                gui->histogramDB(), gui->analysisLib(),
	                                onlineHistosOnPage->histo,
	                                gui->verbosity(),
	                                dimBrowser,
	                                oraMutex,
	                                dimMutex,
                                  rootMutex,
                                  loadAnaMutex);
	                            
//	    if (dbRootHist->isEmptyHisto()) {
//	      m_tasksNotRunning.push_back(onlineHistosOnPage->histo->task());
//	    } 
	  }
	
	  if (0 != gui->archive() &&
	      ((History == gui->presenterMode()) ||
	       (EditorOffline == gui->presenterMode() && gui->canWriteToHistogramDB()))) {
   boost::mutex::scoped_lock lock(rootMutex);
   if (lock) {
      gui->archive()->fillHistogram(dbRootHist,
                               gui->rw_timePoint,
                               gui->rw_pastDuration);
   }

	//              if (dbRootHist && dbRootHist->isEmptyHisto()) {
//	      gui->setResumeRefreshAfterLoading(false);
	//              }                                       
	  }
//	  dbHistosOnPage->push_back(dbRootHist);
   boost::mutex::scoped_lock lock(listMutex);
   if (lock) {
	   dbHistosOnPage->push_back(dbRootHist);
   }
}

void refreshHisto(DbRootHist* dbHistoOnPage)
{
	if (false == dbHistoOnPage->isEmptyHisto()) {
		dbHistoOnPage->fillHistogram();
		dbHistoOnPage->normalizeReference();
	} else {
		dbHistoOnPage->beRegularHisto();
		dbHistoOnPage->initHistogram();
		if (false == dbHistoOnPage->isEmptyHisto()) {
			dbHistoOnPage->fillHistogram();
		  dbHistoOnPage->setTH1FromDB();
		}
	}
}

ParallelWait::ParallelWait(PresenterMainFrame* gui):
  m_gui(gui)
{
}
ParallelWait::~ParallelWait()
{
}


void ParallelWait::loadHistograms(const std::vector<OnlineHistoOnPage*> * onlineHistosOnPage,
				      									  std::vector<DbRootHist*> * dbHistosOnPage) {
//  TThread::Lock();
	m_onlineHistosOnPageIt = onlineHistosOnPage->begin();
	
//  int num_threads = onlineHistosOnPage->nhs=();
  boost::thread_group thrds;

//	m_tasksNotRunning.clear();
	while (m_onlineHistosOnPageIt != onlineHistosOnPage->end()) {
//            startBenchmark((*m_onlineHistosOnPageIt)->histo->identifier());
    thrds.create_thread(boost::bind(&getHisto,
			    													m_gui,
			    													*m_onlineHistosOnPageIt,
			    													dbHistosOnPage));
	  m_onlineHistosOnPageIt++;
	}
  thrds.join_all();	
//	TThread::UnLock();
}

void ParallelWait::refreshHistograms(std::vector<DbRootHist*> * dbHistosOnPage)
{	
//  TThread::Lock();
  
  boost::thread_group thrds;
  
  std::vector<DbRootHist*>::iterator refresh_dbHistosOnPageIt;
  refresh_dbHistosOnPageIt = dbHistosOnPage->begin();
  while (refresh_dbHistosOnPageIt != dbHistosOnPage->end()) {
    thrds.create_thread(boost::bind(&refreshHisto,
			    													*refresh_dbHistosOnPageIt));
    refresh_dbHistosOnPageIt++;
  }
  
  thrds.join_all();	
  
//  TThread::UnLock();    
}


