// $Id: ParallelWait.cpp,v 1.25 2010-08-08 15:13:33 robbep Exp $
// STL
#include <vector>

// Boost
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

// Online
#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"

// Local
#include "DbRootHist.h"
#include "presenter.h"
#include "PresenterMainFrame.h"
#include "Archive.h"
#include "ParallelWait.h"
#include "TrendingHistogram.h"

using namespace pres;
using namespace std;
using namespace boost;

namespace PresenterMutex {
  extern boost::mutex           listMutex;
  extern boost::mutex           archiveMutex;
  extern boost::recursive_mutex oraMutex;
  extern boost::recursive_mutex dimMutex;
  extern boost::recursive_mutex rootMutex;
};

//======================================================================
// call function for threads
//======================================================================
DbRootHist* Presenter::getPageHistogram(PresenterMainFrame * presenter, 
					const std::string & identifier,
					const std::string & currentPartition,
					int refreshTime,
					int instance,
					OnlineHistDB* histogramDB,
					OMAlib* analysisLib,
					OnlineHistogram* onlineHist,
					pres::MsgLevel verbosity,
					DimBrowser* dimBrowser) {
  std::vector<std::string*> tasksNotRunning;

  DbRootHist* dbRootHist = new DbRootHist(identifier,
                                          currentPartition,
                                          refreshTime, instance,
                                          histogramDB, analysisLib,
                                          onlineHist,
                                          verbosity,
                                          dimBrowser, &PresenterMutex::oraMutex,
                                          &PresenterMutex::dimMutex,
                                          tasksNotRunning,
                                          &PresenterMutex::rootMutex);
  if (0 != presenter) {
    dbRootHist->setPresenter(presenter);
    dbRootHist->initHistogram();
    if (histogramDB) { dbRootHist->setTH1FromDB();}
  }

  std::vector<std::string*>::const_iterator tasksNotRunningIt;
  for (tasksNotRunningIt = tasksNotRunning.begin();
       tasksNotRunningIt != tasksNotRunning.end();
       ++tasksNotRunningIt) {
    delete *tasksNotRunningIt;
  }
  tasksNotRunning.clear();                                  
  return dbRootHist;
}

//=================================================================================
// get histograms from list
//=================================================================================
void Presenter::getHistogramsFromLists(PresenterMainFrame * presenter,
				       OnlineHistoOnPage* onlineHistosOnPage,
				       std::vector<DbRootHist*> * dbHistosOnPage,
				       std::vector<std::string*> & tasksNotRunning) {

  DbRootHist* dbRootHist;
  
  if( ( History       == presenter->presenterMode() ) ||
      ( EditorOffline == presenter->presenterMode() ) ) { // no need for DIM
    if ( onlineHistosOnPage -> histo -> type() == OnlineHistDBEnv::TRE ) {
      dbRootHist = new TrendingHistogram(onlineHistosOnPage->histo->identifier(),
					 std::string(""),
					 2, onlineHistosOnPage->instance,
					 presenter->histogramDB(),
					 presenter->analysisLib(),
					 onlineHistosOnPage->histo,
					 presenter->verbosity(),
					 NULL,
					 &PresenterMutex::oraMutex,
					 &PresenterMutex::dimMutex,
					 tasksNotRunning,
					 &PresenterMutex::rootMutex,
					 0 , 0 , TrendingHistogram::History );
    } else { 
      dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
				  std::string(""),
				  2, onlineHistosOnPage->instance,
				  presenter->histogramDB(),
				  presenter->analysisLib(),
				  onlineHistosOnPage->histo,
				  presenter->verbosity(),
				  NULL,
				  &PresenterMutex::oraMutex,
				  &PresenterMutex::dimMutex,
				  tasksNotRunning,
				  &PresenterMutex::rootMutex);
    }
  } else if( (Online == presenter->presenterMode()) ||
	     (EditorOnline == presenter->presenterMode())) {
    DimBrowser* dimBrowser = NULL;    
    dimBrowser = presenter -> dimBrowser( ) ;

    if ( onlineHistosOnPage -> histo -> type() == OnlineHistDBEnv::TRE ) {
      dbRootHist = new TrendingHistogram( onlineHistosOnPage->histo->identifier(),
					  presenter->currentPartition(),
					  2, onlineHistosOnPage->instance,
					  presenter->histogramDB(),
					  presenter->analysisLib(),
					  onlineHistosOnPage->histo,
					  presenter->verbosity(),
					  dimBrowser,
					  &PresenterMutex::oraMutex,
					  &PresenterMutex::dimMutex,
					  tasksNotRunning,
					  &PresenterMutex::rootMutex , 
					  0 , 0 , TrendingHistogram::Last2Hours );
    } else {
      dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
				  presenter->currentPartition(),
				  2, onlineHistosOnPage->instance,
				  presenter->histogramDB(),
				  presenter->analysisLib(),
				  onlineHistosOnPage->histo,
				  presenter->verbosity(),
				  dimBrowser,
				  &PresenterMutex::oraMutex,
				  &PresenterMutex::dimMutex,
				  tasksNotRunning,
				  &PresenterMutex::rootMutex);
    }
  }
  
  if (0 != presenter) {
    dbRootHist->setPresenter(presenter);
    dbRootHist->initHistogram();
    dbRootHist->setTH1FromDB();
  }
  if (0 != presenter->archive() &&
      ( (History == presenter->presenterMode()) ||
	(EditorOffline == presenter->presenterMode() &&
	 presenter->canWriteToHistogramDB())) ) {
    boost::mutex::scoped_lock archiveLock( PresenterMutex::archiveMutex );
    presenter->archive()->fillHistogram(dbRootHist,
    					presenter->rw_timePoint,
    					presenter->rw_pastDuration);
  }
  
  boost::mutex::scoped_lock listLock( PresenterMutex::listMutex );
  // move all overlap histos at the end of the list
  if ( dbRootHist -> onlineHistogram() -> onpage() -> isOverlap() ) {
    dbHistosOnPage->push_back(dbRootHist);
  } else 
    dbHistosOnPage -> insert( dbHistosOnPage -> begin() , dbRootHist ) ;
}

//=======================================================================================
// refresh histos
//=======================================================================================
void Presenter::refreshHisto(DbRootHist* dbHistoOnPage) {
  if (false == gROOT->IsInterrupted()) { 
    dbHistoOnPage->fillHistogram();
    dbHistoOnPage->normalizeReference();
  }
}

ParallelWait::ParallelWait(PresenterMainFrame* presenter):
  m_presenterApp(presenter)
{
}
ParallelWait::~ParallelWait()
{
  std::vector<std::string*>::const_iterator m_tasksNotRunningIt;
  for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
       m_tasksNotRunningIt != m_tasksNotRunning.end();
       ++m_tasksNotRunningIt) {
    delete *m_tasksNotRunningIt;
  }
  m_tasksNotRunning.clear();  
}

//========================================================================================
// load histograms
//========================================================================================
void ParallelWait::loadHistograms(const std::vector< OnlineHistoOnPage * > * onlineHistosOnPage,
                                  std::vector< DbRootHist * > * dbHistosOnPage ) {
  m_onlineHistosOnPageIt = onlineHistosOnPage->begin();
  boost::thread_group thrds;

  for ( m_onlineHistosOnPageIt = onlineHistosOnPage -> begin() ; 
	onlineHistosOnPage -> end() != m_onlineHistosOnPageIt ; ++m_onlineHistosOnPageIt ) {
    thrds.create_thread( boost::bind( &Presenter::getHistogramsFromLists ,
    				      m_presenterApp ,
    				      *m_onlineHistosOnPageIt ,
    				      dbHistosOnPage ,
    				      m_tasksNotRunning ) ) ;
  }

  thrds.join_all();
}

//=========================================================================================
// refresh histograms
//=========================================================================================
void ParallelWait::refreshHistograms(std::vector<DbRootHist*>* dbHistosOnPage)
{
  std::vector<std::string*>::const_iterator m_tasksNotRunningIt;
  for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
       m_tasksNotRunningIt != m_tasksNotRunning.end();
       ++m_tasksNotRunningIt) {
    delete *m_tasksNotRunningIt;
  }
  m_tasksNotRunning.clear();
  
  // plot first non overlapping histograms
  boost::thread_group thrds;
  
  std::vector<DbRootHist*>::iterator refresh_dbHistosOnPageIt ;

  for ( refresh_dbHistosOnPageIt = dbHistosOnPage->begin() ; 
	dbHistosOnPage->end() != refresh_dbHistosOnPageIt ; 
	++refresh_dbHistosOnPageIt ) {
    if ( (pres::TCKinfo == (*refresh_dbHistosOnPageIt)->effServiceType()) &&
         (Batch != m_presenterApp->presenterMode()) ) {
      m_presenterApp -> setTCK( (*refresh_dbHistosOnPageIt) -> getTitle() ) ;
      std::string message("Current TCK: ");
      message = message + (*refresh_dbHistosOnPageIt) -> getTitle();
      m_presenterApp->setStatusBarText(message.c_str(), 2);
    }
    thrds.create_thread(boost::bind(&Presenter::refreshHisto,
                                    *refresh_dbHistosOnPageIt));
  }
  
  thrds.join_all();  

}


