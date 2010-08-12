// $Id: ParallelWait.cpp,v 1.26 2010-08-12 15:43:00 robbep Exp $
#include "ParallelWait.h"

// STL
#include <vector>

// ROOT include
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif
#include <TGStatusBar.h>
#ifdef WIN32
#pragma warning( pop )
#endif

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
#include "PresenterInformation.h"
#include "Archive.h"
#include "TrendingHistogram.h"

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
DbRootHist* Presenter::getPageHistogram(PresenterInformation * presInfo, 
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
  
  dbRootHist->setPresenterInfo( presInfo );
  dbRootHist->initHistogram();
  if ( histogramDB ) dbRootHist->setTH1FromDB() ; 

  std::vector<std::string*>::const_iterator tasksNotRunningIt;
  for (tasksNotRunningIt = tasksNotRunning.begin();
       tasksNotRunningIt != tasksNotRunning.end();
       ++tasksNotRunningIt) 
    delete *tasksNotRunningIt;
 
  tasksNotRunning.clear();                                  
  return dbRootHist;
}

//=================================================================================
// get histograms from list
//=================================================================================
void Presenter::getHistogramsFromLists(PresenterInformation * presInfo,
				       DimBrowser * dimBrowser ,
				       OnlineHistDB * histogramDB ,
				       OMAlib * analysisLib , 
				       std::pair< const pres::MsgLevel , bool > vPair ,
				       Archive * archive ,
				       OnlineHistoOnPage* onlineHistosOnPage,
				       std::vector<DbRootHist*> * dbHistosOnPage,
				       std::vector<std::string*> & tasksNotRunning) {

  DbRootHist* dbRootHist;
  
  if( ( pres::History       == presInfo ->presenterMode() ) ||
      ( pres::EditorOffline == presInfo ->presenterMode() ) ) { // no need for DIM
    if ( onlineHistosOnPage -> histo -> type() == OnlineHistDBEnv::TRE ) {
      dbRootHist = new TrendingHistogram(onlineHistosOnPage->histo->identifier(),
					 std::string(""),
					 2, onlineHistosOnPage->instance,
					 histogramDB , analysisLib ,
					 onlineHistosOnPage->histo ,
					 vPair.first , NULL,
					 &PresenterMutex::oraMutex,
					 &PresenterMutex::dimMutex,
					 tasksNotRunning,
					 &PresenterMutex::rootMutex,
					 0 , 0 , TrendingHistogram::History );
    } else { 
      dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
				  std::string(""),
				  2, onlineHistosOnPage->instance,
				  histogramDB , analysisLib ,
				  onlineHistosOnPage->histo,
				  vPair.first , NULL ,
				  &PresenterMutex::oraMutex,
				  &PresenterMutex::dimMutex,
				  tasksNotRunning,
				  &PresenterMutex::rootMutex);
    }
  } else if( ( pres::Online == presInfo -> presenterMode()) ||
	     ( pres::EditorOnline == presInfo -> presenterMode())) {

    if ( onlineHistosOnPage -> histo -> type() == OnlineHistDBEnv::TRE ) {
      dbRootHist = new TrendingHistogram( onlineHistosOnPage->histo->identifier(),
					  presInfo -> currentPartition(),
					  2, onlineHistosOnPage->instance,
					  histogramDB , analysisLib ,
					  onlineHistosOnPage->histo,
					  vPair.first , dimBrowser ,
					  &PresenterMutex::oraMutex,
					  &PresenterMutex::dimMutex,
					  tasksNotRunning,
					  &PresenterMutex::rootMutex , 
					  0 , 0 , TrendingHistogram::Last2Hours );
    } else {
      dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
				  presInfo ->currentPartition(),
				  2, onlineHistosOnPage->instance,
				  histogramDB , analysisLib ,
				  onlineHistosOnPage->histo,
				  vPair.first , dimBrowser,
				  &PresenterMutex::oraMutex,
				  &PresenterMutex::dimMutex,
				  tasksNotRunning,
				  &PresenterMutex::rootMutex);
    }
  }
  
  if (0 != presInfo ) {
    dbRootHist->setPresenterInfo( presInfo );
    dbRootHist->initHistogram() ;
    dbRootHist->setTH1FromDB()  ;
  }
  if ( ( 0 != archive ) &&
       ( ( pres::History == presInfo -> presenterMode()) ||
	 ( pres::EditorOffline == presInfo -> presenterMode() &&
	   vPair.second ) ) ) {
    boost::mutex::scoped_lock archiveLock( PresenterMutex::archiveMutex );
    archive -> fillHistogram( dbRootHist,
			      presInfo->rwTimePoint(),
			      presInfo->rwPastDuration() ) ;
  }
  
  boost::mutex::scoped_lock listLock( PresenterMutex::listMutex );
  // move all overlap histos at the end of the list
  if ( dbRootHist -> onlineHistogram() -> onpage() -> isOverlap() )
    dbHistosOnPage->push_back(dbRootHist);
  else 
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

//=============================================================================
// Constructor 
//=============================================================================
ParallelWait::ParallelWait( ) { } ;

//=============================================================================
// Destructor 
//=============================================================================
ParallelWait::~ParallelWait() {
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
                                  std::vector< DbRootHist * > * dbHistosOnPage , 
				  PresenterInformation * presenterInfo , 
				  DimBrowser * dimBrowser , 
				  OnlineHistDB * histogramDB , 
				  OMAlib * analysisLib , 
				  const pres::MsgLevel verbosity , bool canWrite , 
				  Archive * archive ) {
  m_onlineHistosOnPageIt = onlineHistosOnPage->begin();
  boost::thread_group thrds;

  for ( m_onlineHistosOnPageIt = onlineHistosOnPage -> begin() ; 
	onlineHistosOnPage -> end() != m_onlineHistosOnPageIt ; ++m_onlineHistosOnPageIt ) {
    thrds.create_thread( boost::bind( &Presenter::getHistogramsFromLists ,
    				      presenterInfo ,
				      dimBrowser , 
				      histogramDB ,
				      analysisLib , 
				      std::make_pair( verbosity , canWrite ) ,
				      archive ,
    				      *m_onlineHistosOnPageIt ,
    				      dbHistosOnPage ,
    				      m_tasksNotRunning ) ) ;
  }

  thrds.join_all();
}

//=========================================================================================
// refresh histograms
//=========================================================================================
void ParallelWait::refreshHistograms(std::vector<DbRootHist*>* dbHistosOnPage,
				     PresenterInformation * presInfo ,
				     TGStatusBar * statusBar ) {
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
         (pres::Batch != presInfo -> presenterMode() ) ) {
      presInfo -> setTCK( (*refresh_dbHistosOnPageIt) -> getTitle() ) ;
      std::string message("Current TCK: ");
      message = message + (*refresh_dbHistosOnPageIt) -> getTitle();
      statusBar -> SetText( message.c_str() , 2 ) ;
    }
    thrds.create_thread(boost::bind(&Presenter::refreshHisto,
                                    *refresh_dbHistosOnPageIt));
  }
  
  thrds.join_all();  
}


