// $Id: $
// Include files

#include <TFile.h>
#include "OnlineHistDB/OnlineHistoOnPage.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"
#include "Archive.h"
#include <TH2D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TText.h>
#include <TGraph.h>

#include "Gaucho/HistTask.h"

// local
#include "Presenter/PresenterPage.h"

// Trending
#include "Trending/ITrendingTool.h"
// global variable
namespace PresenterGaudi {
  extern ITrendingTool * trendingTool;
}

//-----------------------------------------------------------------------------
// Implementation file for class : PresenterPage
//
// 2011-02-09 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PresenterPage::PresenterPage( ) :
  m_bannerPad( NULL),
  m_bannerPageName(NULL),
  m_bannerSource( NULL ),
  m_lastName(""),
  m_lastTimePoint(""),
  m_lastDuration("")
{
  TH1D::SetDefaultSumw2();
  TH2D::SetDefaultSumw2();
  m_dimBrowser = NULL;
  m_onlineHistosOnPage.clear();
  m_tasks.clear();
  m_analysis.clear();
  m_trends.clear();
  m_pads.clear();
}
//=============================================================================
// Destructor
//=============================================================================
PresenterPage::~PresenterPage() {}


//=========================================================================
//  Reset and clear. remove all objects created and owned by the page.
//=========================================================================
void PresenterPage::clear ( ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    (*itT).histos.clear();
  }
  m_tasks.clear();
  m_onlineHistosOnPage.clear();
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    delete (*itA).displayHisto;
  }
  m_analysis.clear();
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    (*itF).histos.clear();
  }
  m_trends.clear();
  m_pads.clear();
  m_bannerPad      = NULL;
  m_bannerPageName = NULL;
  m_bannerSource   = NULL;
  m_lastName       = "";
  m_lastTimePoint  = "";
  m_lastDuration   = "";
}

//=========================================================================
//  Add a simple histo to the page
//=========================================================================
void PresenterPage::addSimpleHisto ( std::string dimName, OnlineHistogram* onlH, OnlineHistDB* histDB, std::string partition ) {
  m_partition = partition;
  std::vector<OnlineHistogram*> anaHistos;
  anaHistos.reserve( 100 ); // avoid relocation
  std::cout << "addSimpleHisto: name " << dimName << std::endl;
  std::string task = dimName.substr( 0, dimName.find( '/' ) );
  task = task.substr( task.find('_')+1 );  // remove partition
  task = task.substr( task.find('_')+1 );  // remove node
  task = task.substr( 0, task.find('_') ); // remove trailing _00

  std::string ident = task + "/" + dimName.substr( dimName.find('/' )+1 );

  DisplayHistogram temp( onlH );
  if ( NULL != onlH && onlH->type() == OnlineHistDBEnv::TRE ) {
    std::string file = onlH->algorithm();
    if ( file.find( ".trend" ) < file.size() ) file = file.substr( 0, file.find( ".trend" ) );
    /*
    unsigned indx = file.size();
    unsigned underscore = 0;
    while ( 0 < indx && file[indx-1] != '/' ) {
      --indx;
      if ( file[indx] == '_' ) underscore = indx;
    }
    if ( underscore == 0 ) underscore = indx;
    file = file.substr(0,indx) + partition + "_" + file.substr( underscore );
    */
    file = partition + "_" + file;
    std::string variable = onlH->hname();
    bool existed = false;
    for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
      if ( (*itF).fileName == file ) {
        std::cout << "  ++ add variable " << variable << " to file " << file << std::endl;
        (*itF).histos.push_back( temp );
        existed = true;
        break;
      }
    }
    if ( !existed ) {
      TrendingFile tFile;
      tFile.fileName = file;
      tFile.startTime = 0;
      tFile.endTime = 0;
      tFile.histos.push_back( temp );
      m_trends.push_back( tFile );
      std::cout << "  ** new variable " << variable << " to file " << file << std::endl;
    }
    return;
  }

  if ( NULL != onlH && onlH->isAnaHist() ) {
    anaHistos.push_back( onlH );
    processAnalysisHistos( anaHistos, histDB );
  } else {
    if ( NULL == onlH ) temp.setIdentifier( ident );
    bool existed = false;
    for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
      if ( (*itT).name == task ) {
        for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
              (*itT).histos.end() != itH; ++itH ) {
          if ( ident == (*itH).identifier() ) {
            std::cout << "              already existing " << ident << std::endl;
            existed = true;
            break;
          }
        }
        if ( !existed ) {
          (*itT).histos.push_back( temp );
          existed = true;
          std::cout << task << " ++ Histo   " << ident << std::endl;
          break;
        }
      }
    }
    if ( !existed ) {
      TaskHistos newTask;
      newTask.name = task;
      newTask.dead = false;
      newTask.histos.push_back( temp );
      m_tasks.push_back( newTask );
      std::cout << task << " ** Histo   " << ident << std::endl;
    }
  }

}
//=========================================================================
//  Prepare the histogram descriptions for access
//=========================================================================
void PresenterPage::prepareAccess( OnlineHistDB* histDB, std::string& partition  ) {
  m_partition = partition;
  std::cout << "** Preparing access for " << m_onlineHistosOnPage.size() << " online histos on page" << std::endl;
  std::vector<OnlineHistogram*> anaHistos;
  anaHistos.reserve( 100 ); // avoid relocation

  int order = 0;
  std::vector<OnlineHistoOnPage*> temp;
  while ( temp.size() < m_onlineHistosOnPage.size() ) {
    for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
          m_onlineHistosOnPage.end() != itHP; ++itHP ) {
      int myIndex;
      (*itHP)->getOverlap( &myIndex );
      if ( order == myIndex ) temp.push_back( *itHP );
    }
    ++order;
  }
  m_onlineHistosOnPage = temp;

  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {
    OnlineHistogram* myHist = (*itHP)->histo;

    //== Is it a trend plot ?

    if ( myHist->type() == OnlineHistDBEnv::TRE ) {
      std::string file = myHist->algorithm();
      if ( file.find( ".trend" ) < file.size() ) file = file.substr( 0, file.find( ".trend" ) );
      /*
      unsigned indx = file.size();
      unsigned underscore = 0;
      while ( 0 < indx && file[indx-1] != '/' ) {
        --indx;
        if ( file[indx] == '_' ) underscore = indx;
      }
      if ( underscore == 0 ) underscore = indx;
      file = file.substr(0,indx) + partition + "_" + file.substr( underscore );
      */
      file = partition + "_" + file;
      std::string variable = myHist->hname();
      bool existed = false;
      for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
        if ( (*itF).fileName == file ) {
          std::cout << "  ++ add variable " << variable << " to file " << file << std::endl;

          OnlineHistoOnPage* mother = (*itHP)->getOverlap();
          int prev = -1;
          if ( NULL != mother ) {
            for ( std::vector<DisplayHistogram>::iterator itDH = (*itF).histos.begin();
                  (*itF).histos.end() != itDH; ++itDH ) {
              if ( mother->histo == (*itDH).histo() ) prev = itDH - (*itF).histos.begin();
            }
          }
          (*itF).histos.push_back( DisplayHistogram( myHist ) );
          (*itF).histos.back().setPrevious( prev );
          existed = true;
          break;
        }
      }
      if ( !existed ) {
        TrendingFile tFile;
        tFile.fileName = file;
        tFile.startTime = 0;
        tFile.endTime = 0;
        tFile.histos.push_back( DisplayHistogram( myHist ) );
        m_trends.push_back( tFile );
        std::cout << "  ** new variable " << variable << " to file " << file << std::endl;
      }

      //== Is it a normal histogram ?

    } else if ( !myHist->isAnaHist() ) {
      bool existed = false;
      for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
        if ( (*itT).name == myHist->task() ) {
          for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
                (*itT).histos.end() != itH; ++itH ) {
            if ( (*itH).identifier() == myHist->identifier() ) {
              std::cout << "              already existing " << myHist->identifier() << std::endl;
              existed = true;
              break;
            }
          }
          if ( !existed ) {
            (*itT).histos.push_back( DisplayHistogram( myHist ) );
            existed = true;
            std::cout << myHist->task() << " ++ Histo   " << myHist->identifier() << " myHist " << myHist << std::endl;
            break;
          }

        }
      }
      if ( !existed ) {
        TaskHistos newTask;
        newTask.name = myHist->task();
        newTask.dead = false;
        newTask.histos.push_back( DisplayHistogram( myHist ) );
        m_tasks.push_back( newTask );
        std::cout << myHist->task() << " ** Histo   " << myHist->identifier() << " myHist " << myHist << std::endl;
      }
    } else {    //== Analysis histograms: Get the source histograns
      anaHistos.push_back( myHist );
    }
  }
  processAnalysisHistos( anaHistos, histDB );
}

//=========================================================================
//  Process teh analysis histogram, recursively
//=========================================================================
void PresenterPage::processAnalysisHistos ( std::vector<OnlineHistogram*>& anaHistos, OnlineHistDB* histDB ) {

  //== Post process the analysis histograms as they can be nested

  for ( std::vector<OnlineHistogram*>::iterator itOH = anaHistos.begin(); anaHistos.end() != itOH; ++itOH ) {
    AnalysisHisto myAna;
    myAna.displayHisto = new DisplayHistogram( *itOH );

    try {
      (*itOH)->getCreationDirections( myAna.algorithm, myAna.histoNames, myAna.params );
    } catch ( std::string sqlException ) {
      std::cout << "SQL error, unknown algorithm  = " << myAna.algorithm << std::endl;
      myAna.algorithm = "Scale";
      myAna.histoNames.clear();
      myAna.params.clear();
    }

    for (unsigned int i=0; i< myAna.histoNames.size(); ++i) {
      OnlineHistogram* histo = histDB->getHistogram( myAna.histoNames[i] );
      std::string taskName = histo->task();
      if ( taskName.find( "_ANALYSIS" ) != std::string::npos ) {
        taskName = taskName.substr( 0, taskName.find( "_ANALYSIS" ) );
      }
      myAna.onlineHistos.push_back( histo );
      if ( histo->isAnaHist() ) {
        anaHistos.push_back( histo );
        std::cout << "  .. Ana hist as source of ana hist... new size " << anaHistos.size() << std::endl;
        continue;
      }
      bool existed = false;
      for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
        if ( (*itT).name == taskName ) {
          for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
                (*itT).histos.end() != itH; ++itH ) {
            if ( (*itH).identifier() == histo->identifier() ) {
              std::cout << "              already existing " << histo->identifier() << std::endl;
              existed = true;
              break;
            }
          }
          if ( !existed ) {
            (*itT).histos.push_back( DisplayHistogram( histo ) );
            existed = true;
            std::cout << taskName << " ++++ Histo " << histo->identifier() << std::endl;
          }
          break;
        }
      }
      if ( !existed ) {
        TaskHistos newTask;
        newTask.name = taskName;
        newTask.dead = false;
        newTask.histos.push_back( DisplayHistogram( histo ) );
        m_tasks.push_back( newTask );
        std::cout << taskName << " **** Histo " << histo->identifier() << std::endl;
      }
    }
    m_analysis.insert( m_analysis.begin(), myAna );   //== put dependent before...
    std::cout << "Inserted analysis title: " << myAna.displayHisto->histo()->htitle() << std::endl;
  }

  std::cout << ".. Now we have " <<  m_tasks.size() << " tasks on page" << std::endl;
}
//=========================================================================
//  Load from the monitoring service
//=========================================================================
void PresenterPage::loadFromDIM( std::string& partition, bool update, std::string& message ) {
  m_partition = partition;
  message = "";
  if ( m_tasks.size() == 0 ) return;

  std::vector<std::string> knownTasks;
  HistTask::TaskList( "", knownTasks );

  std::cout << "Found " << knownTasks.size() << " tasks with 2011 publication." << std::endl;

  /*
  std::sort( knownTasks.begin(), knownTasks.end() );
  for ( std::vector<std::string>::iterator itS = knownTasks.begin();
        knownTasks.end() != itS; ++itS ) {
    std::cout << "   " << (*itS ) << std::endl;
  }
  */

  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    bool foundTheTask = false;
    std::string taskName = (*itT).name;
    taskName = "_" + taskName + "_";

    for ( std::vector<std::string>::iterator itS = knownTasks.begin();
          knownTasks.end() != itS; ++itS ) {
      if ( (*itS).find( partition ) != 0 ) continue;
      if ( (*itS).find( taskName ) == std::string::npos ) continue;
      (*itT).location = *itS;
      foundTheTask = true;
      break;
    }
    if ( foundTheTask ) {
      int timeout = 10 + (*itT).histos.size()/10;
      std::cout << "Search for services of task " << (*itT).location << " with timeout " << timeout << std::endl;
      HistTask myHists( (*itT).location, "", timeout );

      std::vector<std::string> knownNames;
      //== Get the list of services...
      int kk = myHists.Directory( knownNames );
      bool debugExist = false;
      if ( debugExist ) {
        std::cout << "Directory returned status " << kk << " with " << knownNames.size() << " histograms" << std::endl;
        for ( std::vector<std::string>::iterator itS = knownNames.begin(); knownNames.end() != itS  ; ++itS ) {
          std::cout << "      -" << *itS << "-" << std::endl;
        }
      }

      std::vector<std::string> histNames;
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        if ( !update && NULL != (*itH).rootHist() ) {   // cleanup before access...
          //delete (*itH).rootHist();
          (*itH).setRootHist( NULL );
        }
        std::string histoName = (*itH).identifier();
        unsigned int pos = histoName.find( "/" );   // remove the task name prefix
        if ( pos < histoName.size() ) {
          histoName.erase( 0, pos+1 );
        }
        (*itH).setShortName( histoName );
        if ( std::find( knownNames.begin(), knownNames.end(), histoName ) != knownNames.end() ) {
          std::cout << "  ++ ask for histo '" << histoName << "'" << std::endl;
          histNames.push_back( histoName );
        } else {
          std::cout << "  -- Not found '" << histoName << "'" << std::endl;
        }
      }

      std::vector<TObject*> results;
      std::cout << "before calling Histos, size " << histNames.size() << std::endl;
      int status = myHists.Histos( histNames, results );
      std::cout << "Load result  = " << status << " result size " << results.size() << std::endl;
      if ( 0 != status ) {
        std::cout << "  => Retry once" << std::endl;
        status = myHists.Histos( histNames, results );
      }

      if ( results.size() != histNames.size() ) {
        std::cout << "*** Some histograms missing: Got " << results.size() << " for " << histNames.size() << std::endl;
        while ( 0 < results.size() ) {   // Delete all temporary results
          delete results.back();
          results.pop_back();
        }
        while ( results.size() < histNames.size() ) results.push_back( 0 );
        //== Try to inform the operator
        message +=  "Failure to access the histograms of task " + (*itT).location
          + "\n If task is not consuming events (see farm monitoring display), restart the task."
          + "\n If the problem persists, try to restart the Presenter."
          + "\n If this is not enough, restart the task."
          + "\n REFRESH is stopped. You have to ask again for a page, and turn refresh ON if wanted\n";
      }

      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        bool found = false;
        for ( unsigned int indx = 0 ; histNames.size() > indx ; ++indx ) {
          if ( (*itH).shortName() == histNames[indx] ) {
            if ( 0 != results[indx] ) {
              found = true;
              if ( !update ) (*itH).setRootHist( NULL );
              if ( ( update || NULL != (*itH).rootHist() ) &&
                   ( (*itH).rootHist()->GetNbinsX() == ((TH1*)results[indx])->GetNbinsX() ) ) {
                std::cout << "++ upd histo " << histNames[indx] << std::endl;
                (*itH).copyFrom(  (TH1*)results[indx] );
                delete results[indx];
                (*itH).setDisplayOptions();  // Pass the DB flags to the root histogram
                (*itH).prepareForDisplay();
                if ( 0 != (*itH).hostingPad() ) {
                  (*itH).setDrawingOptions( (*itH).hostingPad() );
                  (*itH).hostingPad()->Modified();
                }
              } else {
                std::cout << "++ set histo " << histNames[indx] << std::endl;
                if ( NULL != (*itH).rootHist() ) delete (*itH).rootHist();
                (*itH).setRootHist( (TH1*) results[indx] );
                (*itH).setDisplayOptions();  // Pass the DB flags to the root histogram
                (*itH).prepareForDisplay();
              }
            }
          }
        }
        if ( !found ) {
          std::cout << "-- not found " << (*itH).shortName() << std::endl;
          (*itH).createDummyHisto();
        }
      }
    } else {
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        (*itH).createDummyHisto( "Task not found" );
      }
    }
  }
  std::cout << " ... histograms loaded from DIM" << std::endl;
}
//=========================================================================
// Clear the reference on all plots of the page
//=========================================================================
void PresenterPage::clearReference ( ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).setReferenceHistogram( NULL );
      (*itH).setShowReference( false );
    }
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
          (*itF).histos.end() != itH; ++itH ) {
      (*itH).setShowReference( false );
    }
  }
}

//=========================================================================
//  Upload all the references in one go...
//=========================================================================
void PresenterPage::uploadReference ( OMAlib* analysisLib, PresenterInformation& presInfo ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    char startRunStr[30];
    sprintf( startRunStr, "%d", presInfo.referenceRun() );

    std::string root = analysisLib->refRoot() + "/" + (*itT).name;
    if ( presInfo.offlineContext() ) {
      root = analysisLib->refRoot() + "/" + presInfo.eventType() + "/" + presInfo.processing();
    }
    std::string fileName = root + "/" + presInfo.currentTCK() + "_" + startRunStr + ".root";
    std::cout << "*** Try reference file " << fileName << std::endl;

    TFile* f = new TFile( fileName.c_str(),"READ");
    if ( f ) {
      if ( f->IsZombie() ) {
        delete f;
        f = NULL;
      }
    }
    if ( !f ) {
      if ( "1" != std::string( startRunStr ) ) {
        fileName = root + "/" + presInfo.currentTCK() + "_1.root";
        std::cout << "*** Try reference file " << fileName << std::endl;
        f = new TFile( fileName.c_str(),"READ");
        if ( f ) {
          if ( f->IsZombie() ) {
            delete f;
            f = NULL;
          }
        }
      }
      if ( !f ) {
        fileName = root + "/default_1.root";
        std::cout << "*** Try reference file " << fileName << std::endl;
        f = new TFile( fileName.c_str(),"READ");
      }
    }
    if( f ) {
      if ( f->IsZombie() ) {
        delete f;
        f = NULL;
      }
    }
    if ( f ) {
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        TH1* tmp = analysisLib->findRootHistogram( (*itH).histo(), f );
        if ( 0 != tmp ) {
          TH1* out = (TH1*)tmp->Clone();
          (*itH).setReferenceHistogram( out );
          (*itH).setShowReference( true );
          delete tmp;
          //std::cout << "Reference set for " << (*itH).shortName() << " pointer " << &(*itH) << std::endl;
        } else {
          std::cout << "No reference for " << (*itH).shortName() << std::endl;
        }
      }
      std::cout << "*** References are set ***" << std::endl;
      f->Close();
      delete f;
    }
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
          (*itF).histos.end() != itH; ++itH ) {
      (*itH).setShowReference( true );
    }
  }
}
//=========================================================================
//  Build the analysis histograms
//=========================================================================
bool PresenterPage::buildAnalysisHistos (OMAlib* analysisLib, bool update ) {
  if ( m_analysis.size() == 0 ) return true;
  std::cout << "Build analysis histograms " << std::endl;
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    std::cout << "Get creator for algorithm '" << (*itA).algorithm << "'" << std::endl;
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>( analysisLib->getAlg( (*itA).algorithm ) );
    if ( creator ) {
      std::string htitle( (*itA).displayHisto->histo()->htitle());
      std::cout << "  Title: " << htitle << std::endl;
      TH1* ref=NULL;
      if ( creator->needRef() && (*itA).histoNames.size() > 0 ) {
        std::cout << "   .. get ref for " << (*itA).histoNames[0] << " pointer " << (*itA).onlineHistos[0] << std::endl;
        DisplayHistogram* myDisplay = displayHisto( (*itA).onlineHistos[0] );
        if ( NULL == myDisplay ) {
          std::cout << "   corresponding DisplayHisto not found" << std::endl;
        } else {
          ref = myDisplay->referenceHist();
          if ( NULL == ref ) {
            std::cout << "*** Reference not loaded ! ***" << std::endl;
            return false;
          }
        }
      }
      std::vector<TH1*> rootHists;
      std::vector<TH1*> refHists;
      bool isDummy = false;
      for ( std::vector<OnlineHistogram*>::iterator itOH = (*itA).onlineHistos.begin();
            (*itA).onlineHistos.end() != itOH; ++itOH ) {
        DisplayHistogram* dispH = displayHisto( (*itOH) );
        std::cout << "  get OnlineHistogram " << itOH-(*itA).onlineHistos.begin()
                  << " hname " << (*itOH)->hname()
                  << " pointer " << dispH ;
        if ( 0 != dispH ) std::cout << "  Reference pointer " << dispH->referenceHist();
        std::cout << std::endl;

        if ( NULL != dispH ) {
          rootHists.push_back( dispH->rootHist() );
          if ( NULL != dispH->referenceHist() ) {
            refHists.push_back(  dispH->referenceHist() );
          }
          isDummy = isDummy || dispH->isDummy();
        }
      }
      TH1* rootH = NULL;
      TH1* refH  = NULL;
      if ( rootHists.size() == (*itA).onlineHistos.size() ) {
        rootH = creator->exec( &rootHists,
                               &(*itA).params,
                               htitle,
                               htitle,
                               NULL,
                               ref );
        if ( !update && refHists.size() == (*itA).onlineHistos.size() ) {
          refH = creator->exec( &refHists,
                                &(*itA).params,
                                htitle,
                                htitle,
                                NULL,
                                ref );
        }

        std::cout << "Created analysis histogram " << htitle << " pointer " << rootH << " dummy " << isDummy;
        if ( rootH ) std::cout << " integral " << rootH->Integral();
        if ( refH ) std::cout << " and reference";
        std::cout << std::endl;
      }
      if ( rootH ) {
        if ( update ) {
          (*itA).displayHisto->copyFrom( rootH );
          (*itA).displayHisto->setDummy( isDummy );
          delete rootH;
         } else {
          (*itA).displayHisto->setRootHist( rootH );
          (*itA).displayHisto->setDisplayOptions( );
          if ( refH ) (*itA).displayHisto->setReferenceHistogram( refH );
          (*itA).displayHisto->setDummy( isDummy );
        }
      } else {
        std::cout<< "creator algorithm failed!"<<std::endl;
        (*itA).displayHisto->createDummyHisto();
      }
    } else {
      std::cout << "creator algorithm for virtual histogram not found!" << std::endl;
      (*itA).displayHisto->createDummyHisto();
    }
  }
  std::cout << "Analysis prepared, size " << m_analysis.size() << std::endl;
  return true;
}

//=========================================================================
//  Load the histograms from Archives, i.e. from history files.
//=========================================================================
void PresenterPage::loadFromArchive( Archive* archive,
                                     const std::string & timePoint,
                                     const std::string & pastDuration,
                                     bool hasChanged) {
  std::cout << "Load from Archive, time  Point " << timePoint
            << " pastDuration " << pastDuration << std::endl;
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).deleteRootHist( );
    }
  }
  bool keepOpen = ( m_tasks.size()          == 1 &&
                    (*m_tasks.begin()).name == m_lastName &&
                    timePoint               == m_lastTimePoint &&
                    pastDuration            == m_lastDuration );
  if ( hasChanged ) keepOpen = false;

  if ( m_tasks.size() > 0 ) {
    m_lastName  =  (*m_tasks.begin()).name;
  } else {
    m_lastName = "";
  }
  m_lastTimePoint = timePoint;
  m_lastDuration  = pastDuration;

  if ( !keepOpen ) archive->closeFiles();

  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    if ( !keepOpen ) archive->setFiles( (*itT).name, timePoint, pastDuration );
    archive->fillHistogramsFromFiles( (*itT).histos );
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).rootHist()->SetStats( false );
      (*itH).setDisplayOptions();
      (*itH).prepareForDisplay();
    }
  }
  std::cout << "Loaded from Archive" << std::endl;
}

//=========================================================================
//
//=========================================================================
DisplayHistogram*  PresenterPage::displayHisto( OnlineHistoOnPage* onl ) {
  return displayHisto( onl->histo );
}


//=========================================================================
//== Give the (first) DisplayHistogram for the specified OnlineHistogram
//=========================================================================
DisplayHistogram* PresenterPage::displayHisto( OnlineHistogram* onlHist ) {
  if ( onlHist->type() == OnlineHistDBEnv::TRE ) {
    for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
      for ( std::vector<DisplayHistogram>::iterator itDH = (*itF).histos.begin(); (*itF).histos.end() != itDH; ++itDH ) {
        if ( (*itDH).histo() == onlHist ) return &(*itDH);
      }
    }
    return NULL;
  }
  if ( onlHist->isAnaHist() ) {
    for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
      if ( (*itA).displayHisto->histo() == onlHist ) return (*itA).displayHisto;
    }
    return NULL;
  }
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      if ( (*itDH).identifier() == onlHist->identifier() ) return &(*itDH);
    }
  }
  return NULL;
}

//=========================================================================
//== Give the (first) DisplayHistogram for the specified TH1 histogram
//=========================================================================
DisplayHistogram* PresenterPage::displayHisto( TH1* hist ) {
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    if ( (*itA).displayHisto->rootHist()      == hist ) return (*itA).displayHisto;
    if ( (*itA).displayHisto->referenceHist() == hist ) return (*itA).displayHisto;
  }
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      if ( (*itDH).rootHist()      == hist ) return &(*itDH);
      if ( (*itDH).referenceHist() == hist ) return &(*itDH);
    }
  }
  return NULL;
}

//=========================================================================
//== Give the (first) DisplayHistogram for the specified TGraph
//=========================================================================
DisplayHistogram* PresenterPage::displayHisto( TGraph* hist ) {
  for ( std::vector<TrendingFile>::iterator itT = m_trends.begin(); m_trends.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      if ( (*itDH).graph()      == hist ) return &(*itDH);
    }
  }
  return NULL;
}

//=========================================================================
// set OffsetHistogram
//=========================================================================
void PresenterPage::setOffsetHistograms ( ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).setOffsetHistogram();
    }
  }
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    (*itA).displayHisto->setOffsetHistogram();
  }
}
//=========================================================================
// reset OffsetHistogram
//=========================================================================
void PresenterPage::resetOffsetHistograms ( ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).resetOffsetHistogram();
    }
  }
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    (*itA).displayHisto->resetOffsetHistogram();
  }
}

//=========================================================================
//  Simple display, with automatic n x m layout
//=========================================================================
void PresenterPage::simpleDisplay (  TCanvas* editorCanvas,  OMAlib* analysisLib ) {
  int nbPlots = 0;
  if ( m_analysis.empty() ) {
    for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
      nbPlots += (*itT).histos.size();
    }
  } else {
    nbPlots += m_analysis.size();
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    nbPlots += (*itF).histos.size();
  }
  int nRow = 1;
  while ( nRow < nbPlots/nRow ) nRow += 1;
  int nCol = nbPlots / nRow;
  if ( nRow * nCol < nbPlots ) nCol++;

  std::cout << "For " << nbPlots << " plots, nCol " << nCol << " nRow " << nRow << std::endl;

  int row = nRow-1;
  int col = 0;
  double rSize = 1./nRow;
  double cSize = 1./nCol;

  if ( m_analysis.empty() ) {
    for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin(); (*itT).histos.end() != itH; ++itH ) {
        double xLow = col * cSize;
        double xHig = xLow + cSize;
        double yLow = row * rSize;
        double yHig = yLow + rSize;
        if ( NULL != m_bannerPad ) {
          yLow = 0.95 * yLow;
          yHig = 0.95 * yHig;
        }
        (*itH).draw( editorCanvas, xLow, yLow, xHig, yHig, NULL, false, NULL );
        (*itH).setDisplayOptions();
        col += 1;
        if ( nCol == col ) {
          col = 0;
          row--;
        }
      }
    }
  } else {
    for ( std::vector<AnalysisHisto>::iterator itH = m_analysis.begin(); m_analysis.end() != itH; ++itH ) {
      double xLow = col * cSize;
      double xHig = xLow + cSize;
      double yLow = row * rSize;
      double yHig = yLow + rSize;
      if ( NULL != m_bannerPad ) {
        yLow = 0.95 * yLow;
        yHig = 0.95 * yHig;
      }
      (*itH).displayHisto->draw( editorCanvas, xLow, yLow, xHig, yHig, analysisLib, false, NULL );
      (*itH).displayHisto->setDisplayOptions();
      col += 1;
      if ( nCol == col ) {
        col = 0;
        row--;
      }
    }
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin(); (*itF).histos.end() != itH; ++itH ) {
      double xLow = col * cSize;
      double xHig = xLow + cSize;
      double yLow = row * rSize;
      double yHig = yLow + rSize;
      if ( NULL != m_bannerPad ) {
        yLow = 0.95 * yLow;
        yHig = 0.95 * yHig;
      }
      (*itH).draw( editorCanvas, xLow, yLow, xHig, yHig, NULL, false, NULL );
      (*itH).setDisplayOptions();
      col += 1;
      if ( nCol == col ) {
        col = 0;
        row--;
      }
    }
  }
  editorCanvas->Update();
}
//=========================================================================
//  Draw the full page
//=========================================================================
OnlineHistogram *GetTopMother(OnlineHistogram* h)
{
  OnlineHistoOnPage *m;
  m = h->onpage()->getOverlap();
  if (m == 0)
  {
    return h;
  }
  else
  {
    return GetTopMother(m->histo);
  }
}
void PresenterPage::rescaleHists()
{
  std::map<OnlineHistogram*,std::list<OnlineHistogram*>*> PadMap;
  PadMap.clear();
  for ( auto itHP =  m_onlineHistosOnPage.begin(); m_onlineHistosOnPage.end() != itHP; ++itHP )
  {

    OnlineHistogram* onlHist = (*itHP)->histo;
    DisplayHistogram *dhist = displayHisto(onlHist);
    OnlineHistogram *topmother = GetTopMother(onlHist);
    auto padit = PadMap.find(topmother);
    if (padit == PadMap.end())
    {
      padit = PadMap.insert(std::make_pair(topmother,new std::list<OnlineHistogram*>)).first;
      padit->second->push_back(topmother);
    }
    std::cout << "adding histogram " << dhist->rootName() << " to 'pad' "<< displayHisto(topmother)->rootName() <<std::endl;
    padit->second->push_back(onlHist);

  }
  for (auto padit = PadMap.begin();padit != PadMap.end();padit++)
  {
    std::list<OnlineHistogram*> *histsonpad;
    histsonpad = padit->second;
    double ymin = 1.0e100;
    double ymax = -1.0e100;
    DisplayHistogram *dhist;
    TH1 *rh;
    TGraph *tg;
    TAxis *ax;
    for (auto hpit = histsonpad->begin();hpit!= histsonpad->end();hpit++)
    {
      OnlineHistogram *oh;
      oh = (*hpit);
      dhist = displayHisto(oh);
      rh = dhist->rootHist();
      tg=dhist->graph();
      if (tg != 0)
      {
        ax=tg->GetYaxis();
      }
      else if (rh != 0)
      {
        ax = rh->GetYaxis();
      }
      double axymin = ax->GetXmin();
      double axymax = ax->GetXmax();
      if (ymin > axymin) ymin = axymin;
      if (ymax < axymax) ymax = axymax;
    }
    for (auto hpit = histsonpad->begin();hpit!= histsonpad->end();hpit++)
    {
      dhist = displayHisto(*hpit);
      rh = dhist->rootHist();
      tg=dhist->graph();
      if (tg != 0)
      {
        tg->SetMinimum(ymin);
        tg->SetMaximum(ymax);
        ax=tg->GetYaxis();
      }
      else if (rh != 0)
      {
        rh->SetMinimum(ymin);
        rh->SetMaximum(ymax);
        ax = rh->GetYaxis();
      }
      std::cout << "Setting user Range ("<<ymin<<","<<ymax << ") for "<<dhist->rootName()<<std::endl;
      ax->SetRangeUser(ymin,ymax);
      ax->SetLimits(ymin,ymax);
    }
  }
  for (auto padit = PadMap.begin();padit != PadMap.end();padit++)
  {
    std::list<OnlineHistogram*>* histsonpad;
    histsonpad = padit->second;
    delete histsonpad;
  }
  PadMap.clear();
  for (auto it=m_pads.begin();it!=m_pads.end();it++)
  {
    std::vector<DisplayHistogram*> *v;
    v = &(it->objects);
    DisplayHistogram *dh;
    double pxmin;
    pxmin = 1.0e100;
    double pxmax;
    pxmax = -1.0e100;
    double xmin,xmax;
    for (auto jt=v->begin();jt!=v->end();jt++)
    {
      TH1 *rh;
      TGraph *tg;
      TAxis *ax;
      dh = *jt;
      rh = dh->rootHist();
      tg=dh->graph();
      if (tg != 0)
      {
        ax=tg->GetYaxis();
      }
      else if (rh != 0)
      {
        ax = rh->GetYaxis();
      }
      xmin = ax->GetXmin();
      xmax = ax->GetXmax();
      if (pxmin > xmin)pxmin=xmin;
      if (pxmax < xmax)pxmax=xmax;
      std::cout << "Min/Max Y for "<<dh->rootName()<<" on Pad "<<ax->GetXmin()<<" "<<ax->GetXmax()<<std::endl;
    }
    for (auto jt=v->begin();jt!=v->end();jt++)
    {
      TH1 *rh;
      TGraph *tg;
      TAxis *ax;
      dh = *jt;
      rh = dh->rootHist();
      tg=dh->graph();
      if (tg != 0)
      {
        tg->SetMinimum(pxmin);
        tg->SetMaximum(pxmax);
        ax=tg->GetYaxis();
      }
      else if (rh != 0)
      {
        rh->SetMinimum(pxmin);
        rh->SetMaximum(pxmax);
        ax = rh->GetYaxis();
      }
      std::cout << "Setting user Range ("<<pxmin<<","<<pxmax << ") for "<<dh->rootName()<<std::endl;
      ax->SetRangeUser(pxmin,pxmax);
      ax->SetLimits(pxmin,pxmax);
    }
    for (auto jt=v->begin();jt!=v->end();jt++)
    {
      TH1 *rh;
      TGraph *tg;
      TAxis *ax;
      dh = *jt;
      rh = dh->rootHist();
      tg=dh->graph();
      if (tg != 0)
      {
        ax=tg->GetYaxis();
      }
      else if (rh != 0)
      {
        ax = rh->GetYaxis();
      }
      xmin = ax->GetXmin();
      xmax = ax->GetXmax();
      std::cout << "Min/Max Y for "<<dh->rootName()<<" on Pad "<<ax->GetXmin()<<" "<<ax->GetXmax()<<std::endl;
    }
  }
}
void PresenterPage::drawPage ( TCanvas* editorCanvas, OMAlib* analysisLib, bool fastHitMapDraw ) {

  // Build the Pad structure, to know the size of the legend if any!
  std::vector<OnlineHistoOnPage*>::iterator itHP;
  for ( itHP =  m_onlineHistosOnPage.begin(); m_onlineHistosOnPage.end() != itHP; ++itHP ) {
    if ( 0 == displayHisto( *itHP )->myObject() ) continue;
    OnlineHistogram* onlHist = (*itHP)->histo;
    if ( onlHist->onpage() ) {
      if ( onlHist->onpage()->isOverlap() ) {
        OnlineHistoOnPage* mother = onlHist->onpage()->getOverlap();
        for ( std::vector<OnlineHistoOnPage*>::iterator itPrev =  m_onlineHistosOnPage.begin();
              m_onlineHistosOnPage.end() != itPrev; ++itPrev ) {
          if ( *itPrev == mother ) {
            displayHisto( mother )->increaseOverlap();
            break;
          }
        }
      }
    }
  }
  rescaleHists();
  for ( itHP =  m_onlineHistosOnPage.begin(); m_onlineHistosOnPage.end() != itHP; ++itHP )
  {

    TPad* overlayOnPad = NULL;
    OnlineHistogram* onlHist = (*itHP)->histo;
    double xlow(0.0);
    double ylow(0.0);
    double xup(0.0);
    double yup(0.0);
    if ( onlHist->onpage() ) {
      xlow = onlHist->onpage()->xmin;
      ylow = onlHist->onpage()->ymin;
      xup  = onlHist->onpage()->xmax;
      yup  = onlHist->onpage()->ymax;

      // histogram must be overdrawn on previous hist:
      // look for the corresponding TPad
      if ( onlHist->onpage()->isOverlap() ) {
        OnlineHistoOnPage* mother = onlHist->onpage()->getOverlap();

        for ( std::vector<OnlineHistoOnPage*>::iterator itPrev =  m_onlineHistosOnPage.begin();
              m_onlineHistosOnPage.end() != itPrev; ++itPrev ) {
          if ( (*itPrev)->histo == mother->histo) {
            overlayOnPad = displayHisto( (*itPrev) )->hostingPad();
            break;
          }
        }
      }
    }

    DisplayHistogram* dispH =  displayHisto( (*itHP)  );
    if ( NULL != m_bannerPad ) {
      ylow = 0.95 * ylow;
      yup  = 0.95 * yup;
    }
    if (  dispH ) {
      std::cout << "-- display histo " << dispH->identifier()
                << " on pad X "<< xlow << " to "<< xup
                << "  Y " << ylow << " to " << yup
                << " overlay " << overlayOnPad << " nOverlap " << dispH->nOverlap() << std::endl;
      dispH->draw( editorCanvas, xlow, ylow, xup, yup, analysisLib, fastHitMapDraw, overlayOnPad );
      bool found = false;
      for ( std::vector<PadContent>::iterator itP = m_pads.begin(); m_pads.end() != itP; ++itP ) {
        if ( (*itP).pad == dispH->hostingPad() ) {
          if ( 0 != dispH->myObject() ) (*itP).objects.push_back( dispH );
          found = true;
          break;
        }
      }
      if ( !found ) {
        if ( 0 != dispH->myObject() ) {
          PadContent newPad;
          newPad.pad = dispH->hostingPad();
          newPad.hasTitle = dispH->hasTitle();
          newPad.objects.push_back( dispH );
          m_pads.push_back( newPad );
        }
      }
    }
  }
  std::cout << "Set the legend" << std::endl;
  for ( std::vector<PadContent>::iterator itP = m_pads.begin(); m_pads.end() != itP; ++itP ) {
    if ( 2 > (*itP).objects.size() || (*itP).hasTitle ) continue;
    (*itP).pad->cd();
    float yMin =  1.0f - 0.05f * ( ((*itP).objects.size()+1)/2 );
    std::cout << "Legend for " << (*itP).objects.size() << " plots, ymin " << yMin << std::endl;
    TLegend* leg = new TLegend( 0.0, yMin, 1.0, 1.0 );
    leg->SetNColumns( 2 );
    for ( std::vector<DisplayHistogram*>::iterator itO = (*itP).objects.begin(); (*itP).objects.end() != itO; ++itO ) {
      leg->AddEntry( (*itO)->myObject(), (*itO)->title().c_str(), "L" );
    }
    leg->Draw();
  }

  std::cout << "Update the drawing options" << std::endl;
  updateDrawingOptions();
  editorCanvas->Update();
  std::cout << "Update of the canvas OK" << std::endl;
}

//=========================================================================
//  Update the pad
//=========================================================================
void PresenterPage::updateDrawingOptions ( ) {
  rescaleHists();
  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {
    DisplayHistogram* dispH =  displayHisto( (*itHP)  );
    if ( NULL != dispH ) {
      dispH->normalizeReference();
      TPad* pad = dispH->hostingPad();
      if ( NULL != pad ) dispH->setDrawingOptions( pad );
    }
  }
  rescaleHists();
}

//=========================================================================
//  Fill the trend plots. Start and end time in integer. If endTime is zero, startTime is an offset
//=========================================================================
void PresenterPage::fillTrendingPlots ( unsigned int startTime, unsigned int endTime, bool update ) {
  std::cout << "  ++ Trending size " << m_trends.size() << std::endl;
  if ( m_trends.size() == 0 ) return;

  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin();
        m_trends.end() != itF; ++itF ) {
    bool status = PresenterGaudi::trendingTool -> openRead( (*itF).fileName ) ;
    if ( !status ) {
      std::cerr << "Trend file does not exist : " << (*itF).fileName << std::endl ;
      for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
            (*itF).histos.end() != itH; ++itH ) {
        (*itH).createDummyHisto();
      }
      continue ;
    }
    //== Handle the time offset case
    if ( 0 == endTime ) {
      endTime   = (int)::time( 0 );
      startTime = endTime - startTime;
    }
    std::vector<std::string> tags;
    PresenterGaudi::trendingTool->tags( tags );

    int lastTagVersion   = PresenterGaudi::trendingTool->tagVersion();
    unsigned int firstTimeThisTag = PresenterGaudi::trendingTool->firstTimeThisTag();
    std::cout << "Select for time range: " << startTime << " to " << endTime
              << " last Tag version " << lastTagVersion << " starts at " << firstTimeThisTag << std::endl;

    std::vector<TrendData> allTrends;
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
          (*itF).histos.end() != itH; ++itH ) {
      TrendData tmp;
      tmp.histo = &(*itH);
      tmp.tag = (*itH).shortName();
      tmp.min =  1.e100;
      tmp.max = -1.e100;
      allTrends.push_back( tmp );
    }

    //== Check that the whole time range is in the current Tag version. Else go down in version.
    int currVersion = lastTagVersion;
    if ( firstTimeThisTag > startTime && lastTagVersion > 1 ) {
      currVersion = lastTagVersion - 1;
      while( !PresenterGaudi::trendingTool->tags( tags, currVersion ) &&
             currVersion > 1 ) {
        --currVersion;
      }
      firstTimeThisTag = PresenterGaudi::trendingTool->firstTimeThisTag();
    }

    unsigned int theTime = startTime;
    std::vector<TrendData>::iterator itT;

    while ( currVersion <= lastTagVersion ) {
      for ( itT = allTrends.begin(); allTrends.end() != itT; ++itT ) {
        //== Handle ratios: The tag name is a pair, separated by a '|'
        if ( std::find( tags.begin(), tags.end(), (*itT).tag ) == tags.end() ) {
          std::cout << "Tag not found : " << (*itT).tag << " try to split." << std::endl;
          std::string myTag = (*itT).tag;
          unsigned int barIndx = myTag.find( "|" );
          if ( barIndx < myTag.size() ) {
            (*itT).tagDen = myTag.substr( barIndx+1 );
            (*itT).tag    = myTag.substr( 0, barIndx );
            std::cout << "  num='" << (*itT).tag << "'";
            if ( 0 <= (*itT).tagDen ) std::cout << " den='" << (*itT).tagDen << "'";
            std::cout << std::endl;
          }
        }
        (*itT).index = -1;
        (*itT).den   = -1;
        for ( unsigned int kk = 0; tags.size() > kk ; ++kk ) {
          if ( tags[kk] == (*itT).tag    ) (*itT).index = kk;
          if ( tags[kk] == (*itT).tagDen ) (*itT).den   = kk;
        }
        std::cout << "Index " << (*itT).index << " indxDen " << (*itT).den << std::endl;
      }
      //== Select for this version
      status = PresenterGaudi::trendingTool->select( theTime , endTime );
      if ( status ) {
        std::vector<float> theValues ;
        // Read the values in the selected time interval and fill a vector
        while ( PresenterGaudi::trendingTool->nextEvent( theTime , theValues ) ) {
          if ( theTime < startTime ) continue;
          if ( theTime > endTime   ) continue;
          for ( itT = allTrends.begin(); allTrends.end() != itT; ++itT ) {
            double theValue = 0.;
            if ( (*itT).den >= 0 ) {
              if ( fabs( theValues[(*itT).den] ) > 0.0001 ) theValue = theValues[(*itT).index] / theValues[(*itT).den];
            } else if ( 0 <= (*itT).index ) {
              theValue = theValues[(*itT).index];
            }
            if ( theValue > -1.e8 && theValue < 1.e8 ) {
              (*itT).values.push_back( std::pair<int,double>( theTime, theValue ) );
              if ( theValue < (*itT).min ) (*itT).min = theValue;
              if ( theValue > (*itT).max ) (*itT).max = theValue;
            } else {
              std::cout << "Tag " << (*itT).tag << " time " << PresenterGaudi::trendingTool->timeString( theTime )
                        << " ABSURD value " << theValue << " ignored" << std::endl;
            }
          }
        }
      }

      //== Increase the version as long as needed
      while ( currVersion <= lastTagVersion ) {
        ++currVersion;
        if ( currVersion > lastTagVersion ) break;
        std::vector<std::string> tags;
        if ( PresenterGaudi::trendingTool->tags( tags, currVersion ) ) break;
      }
      firstTimeThisTag = PresenterGaudi::trendingTool->firstTimeThisTag();
    }
    PresenterGaudi::trendingTool->closeFile( ) ;

    //== final adjustment : first/last point, min/max in superimposed plots
    for ( itT = allTrends.begin(); allTrends.end() != itT; ++itT ) {
      if ( (*itT).values.size() > 0 ) {
        (*itT).values.push_back( std::pair<int,double>( endTime, (*itT).values.back().second  ) ) ;
      } else {
        (*itT).values.push_back( std::pair<int,double>( startTime, 0. ) );
        (*itT).values.push_back( std::pair<int,double>( endTime, 0. ) );
        (*itT).min = 0.;
        (*itT).max = 0.;
      }
    }
    for ( unsigned int kk = allTrends.size(); kk > 0 ;  ) {
      kk--;
      int jj = allTrends[kk].histo->previous();
      if ( jj >= 0 ) {
        if ( allTrends[jj].min > allTrends[kk].min ) allTrends[jj].min = allTrends[kk].min;
        if ( allTrends[jj].max < allTrends[kk].max ) allTrends[jj].max = allTrends[kk].max;
      }
    }
    for ( itT = allTrends.begin(); allTrends.end() != itT; ++itT ) {
      std::cout << "For tag '" << (*itT).tag << "' size " << (*itT).values.size()
                << " min " << (*itT).min << " max " << (*itT).max
                << " prev " << (*itT).histo->previous() << std::endl;
    }

    for ( unsigned int kk = 0; (*itF).histos.size() > kk; ++kk ) {
      (*itF).histos[kk].createGraph( allTrends[kk], update );
    }
  }
}

//=========================================================================
//
//=========================================================================
void PresenterPage::prepareDisplayHistos ( ) {
  m_displayHistograms.clear();
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      m_displayHistograms.push_back( &(*itDH) );
    }
  }
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    m_displayHistograms.push_back( (*itA).displayHisto );
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itF).histos.begin(); (*itF).histos.end() != itDH; ++itDH ) {
      m_displayHistograms.push_back( &(*itDH) );
    }
  }
}

//=========================================================================
//  Check that all DisplayHistos have an OnlineHisto
//=========================================================================
bool PresenterPage::okForSave ( ) {
  bool ok = true;
  prepareDisplayHistos();
  for ( DisplayHistograms::iterator itH = m_displayHistograms.begin(); m_displayHistograms.end() != itH ; ++itH ) {
    if ( NULL == (*itH)->histo() ) ok = false;
  }
  return ok;
}

//=========================================================================
//
//=========================================================================
void PresenterPage::drawBanner ( std::string name, std::string source ) {
  m_bannerPad = new TPad( "Banner", TString(""), 0.00, 0.95, 1.00, 1.00, 10 );
  m_bannerPad->Draw();
  m_bannerPad->cd();
  m_bannerPageName = new TText(  0.02, 0.5, name.c_str() );
  m_bannerPageName->SetTextAlign( 12 );
  m_bannerPageName->SetTextFont(  42 );
  m_bannerPageName->SetTextSize( 0.5 );
  m_bannerPageName->Draw();

  updateBanner( source );
}

//=========================================================================
//
//=========================================================================
void PresenterPage::updateBanner ( std::string source ) {
  if ( NULL == m_bannerPad ) drawBanner( "", source );
  if ( "" == source || source.find("&") == source.size()-1 ) {
    char buf[40];
    time_t now = ::time(0);
    ::strftime( buf, 40, "%d %B %Y %H:%M:%S", ::localtime(&now) );
    std::string today( buf );
    if ( "" == source ) {
      source = today + " Online";
      if ( "LHCb" == m_partition ) {  //== Current run number ONLY for LHCb
        int def = -1;
        char runChar[12]     = "";
        DimCurrentInfo getRun( "LHCbStatus/RunNumber", def );
        sprintf( runChar, "%d", getRun.getInt() );
        source = source + " Run " + std::string( runChar );
      }
    } else {
      source = source.substr(0,source.size()-1) + " " + today;
    }
    std::cout << "Update banner as " << today << std::endl;
  } else {
    while ( source.size() > 80 ) {
      unsigned int kk = source.find("/");
      if ( kk < source.size() ) {
        source = source.substr( source.find("/")+1 );
      } else {
        break;
      }
    }
  }

  m_bannerPad->cd();
  if ( NULL != m_bannerSource ) delete m_bannerSource;
  m_bannerSource = new TText(  0.98, 0.5, source.c_str() );
  m_bannerSource->SetTextAlign( 32 );
  m_bannerSource->SetTextFont(  42 );
  m_bannerSource->SetTextSize( 0.5 );
  m_bannerSource->Draw();
  m_bannerPad->Update();
}
//=============================================================================
