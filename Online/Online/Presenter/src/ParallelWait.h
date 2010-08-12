// $Id: ParallelWait.h,v 1.8 2010-08-12 15:43:00 robbep Exp $
#ifndef PARALLELWAIT_H_
#define PARALLELWAIT_H_

#include <vector>
#include <string>

#include "presenter.h"

class DbRootHist;
class OnlineHistoOnPage;
class dbHistosOnPage;
class PresenterInformation;
class OnlineHistDB ;
class DimBrowser ; 
class OMAlib ; 
class Archive ;
class OnlineHistogram ;
class TGStatusBar ;

class ParallelWait
{
 public:
  /// Constructor 
  ParallelWait( ) ;

  virtual ~ParallelWait(); ///< Destructor 
								 
  void loadHistograms( const std::vector<OnlineHistoOnPage*> * onlineHistosOnPage,
		       std::vector<DbRootHist*> * dbHistosOnPage, 
		       PresenterInformation * presenterInfo , DimBrowser * dimBrowser , 
		       OnlineHistDB * histogramDB , OMAlib * analysisLib , 
		       const pres::MsgLevel verbosity , bool canWrite , Archive * archive ) ;
  
  void refreshHistograms(std::vector<DbRootHist*> * dbHistosOnPage ,
			 PresenterInformation * presInfo , 
			 TGStatusBar * statusBar ) ;

 private:
  std::vector<std::string*> m_tasksNotRunning;
  std::vector<OnlineHistoOnPage*>::const_iterator m_onlineHistosOnPageIt;
};

// Declaration of call back functions
namespace Presenter {
  void getHistogramsFromLists(PresenterInformation * presInfo, 
			      DimBrowser * dimBrowser , 
			      OnlineHistDB * histogramDB ,
			      OMAlib * analysisLib , 
			      std::pair< const pres::MsgLevel , bool > vPair ,
			      Archive * archive ,
			      OnlineHistoOnPage* onlineHistosOnPage, 
			      std::vector<DbRootHist*> * dbHistosOnPage,
			      std::vector<std::string*> & tasksNotRunning) ;
  
  DbRootHist* getPageHistogram(PresenterInformation * presInfo, 
			       const std::string & identifier,
			       const std::string & dimServiceName,
			       int refreshTime,
			       int instance,
			       OnlineHistDB * histogramDB,
			       OMAlib* analysisLib,
			       OnlineHistogram* onlineHist,
			       pres::MsgLevel verbosity,
			       DimBrowser* dimBrowser);
  
  void refreshHisto(DbRootHist* dbHistoOnPage ) ;
};


#endif /*PARALLELWAIT_H_*/
