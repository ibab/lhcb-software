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

#include "Gaucho/HistTask.h"

// local
#include "PresenterPage.h"
#include "HistogramIdentifier.h"

// Trending
#include "Trending/ITrendingTool.h" 
// global variable
namespace PresenterGaudi { 
  extern ITrendingTool * trendingTool ;
};

//-----------------------------------------------------------------------------
// Implementation file for class : PresenterPage
//
// 2011-02-09 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PresenterPage::PresenterPage( ) {
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
}

//=========================================================================
//  Add a simple histo to the page
//=========================================================================
void PresenterPage::addSimpleHisto ( std::string dimName, OnlineHistogram* onlH, std::string partition ) {
  std::cout << "addSimpleHIsto: name " << dimName << std::endl;
  std::string task = dimName.substr( 0, dimName.find( '/' ) );
  task = task.substr( task.find('_')+1 );  // remove partition
  task = task.substr( task.find('_')+1 );  // remove node
  task = task.substr( 0, task.find('_') ); // remove trailing _00
  
  std::string ident = task + "/" + dimName.substr( dimName.find('/' )+1 );

  DisplayHistogram temp( onlH );
  if ( NULL != onlH && onlH->type() == OnlineHistDBEnv::TRE ) {
    std::string file = onlH->algorithm();
    if ( file.find( ".trend" ) < file.size() ) file = file.substr( 0, file.find( ".trend" ) );
    unsigned indx = file.size();
    unsigned underscore = 0;
    while ( 0 < indx && file[indx-1] != '/' ) {
      --indx;
      if ( file[indx] == '_' ) underscore = indx;
    }
    if ( underscore == 0 ) underscore = indx;
    file = file.substr(0,indx) + partition + "_" + file.substr( underscore );
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
//=========================================================================
//  Prepare the histogram descriptions for access
//=========================================================================
void PresenterPage::prepareAccess( OnlineHistDB* histDB, std::string& partition  ) {
  std::cout << "** Preparing access for " << m_onlineHistosOnPage.size() << " online histos on page" << std::endl;
  std::vector<OnlineHistogram*> anaHistos;
  anaHistos.reserve( 100 ); // avoid relocation
  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {
    OnlineHistogram* myHist = (*itHP)->histo;

    //== Is it a trend plot ?
 
    if ( myHist->type() == OnlineHistDBEnv::TRE ) {
      std::string file = myHist->algorithm();
      if ( file.find( ".trend" ) < file.size() ) file = file.substr( 0, file.find( ".trend" ) );
      unsigned indx = file.size();
      unsigned underscore = 0;
      while ( 0 < indx && file[indx-1] != '/' ) {
        --indx;
        if ( file[indx] == '_' ) underscore = indx;
      }
      if ( underscore == 0 ) underscore = indx;
      file = file.substr(0,indx) + partition + "_" + file.substr( underscore );
      std::string variable = myHist->hname();
      bool existed = false;
      for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
        if ( (*itF).fileName == file ) {
          std::cout << "  ++ add variable " << variable << " to file " << file << std::endl;
          (*itF).histos.push_back( DisplayHistogram( myHist ) );
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
      int timeout = 0.1 * (*itT).histos.size();
      if ( 3 > timeout ) timeout = 3;
      std::cout << "Search for services of task " << (*itT).location << " with timeout " << timeout << std::endl;
      HistTask myHists( (*itT).location, "", timeout );
      /*
        std::vector<std::string> knownNames;
        //== Get the list of services...
        int kk = myHists.Directory( knownNames );
        std::cout << "Directory returned status " << kk << " with " << knownNames.size() << " histograms" << std::endl;
        for ( std::vector<std::string>::iterator itS = knownNames.begin(); knownNames.end() != itS  ; ++itS ) {
          std::cout << "      -" << *itS << "-" << std::endl;
        }
      */

      std::vector<std::string> histNames;
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        if ( !update && NULL != (*itH).rootHist() ) {   // cleanup before access...
          delete (*itH).rootHist();
          (*itH).setRootHist( NULL );
        }
        std::string histoName = (*itH).identifier();
        unsigned int pos = histoName.find( "/" );   // remove the task name prefix
        if ( pos < histoName.size() ) {
          histoName.erase( 0, pos+1 );
        }
        
        (*itH).setShortName( histoName );
        //if ( std::find( knownNames.begin(), knownNames.end(), histoName ) != knownNames.end() ) {
          std::cout << "  ++ ask for histo '" << histoName << "'" << std::endl;
          histNames.push_back( histoName );
          //} else {
          //std::cout << "  -- Not found '" << histoName << "'" << std::endl;
          //}
      }
      std::vector<TObject*> results;
      std::cout << "before calling Histos, size " << histNames.size() << std::endl;
      int status = myHists.Histos( histNames, results );
      std::cout << "Load result  = " << status << " result size " << results.size() << std::endl;
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
          + "\n If this si not enough, restart the task. \n";
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
      //=== Try the 2010 implementation...
    } else {
      (*itT).location = "";
      std::string dimServiceQuery = "*/" + partition + "_*_" + (*itT).name + "_*";
      m_dimBrowser->getServices( dimServiceQuery.c_str() );
      char* dimService;
      char* dimFormat;
      bool foundTask = false;
      while( m_dimBrowser->getNextService(dimService, dimFormat) ) {
        std::string task( dimService );
        task = task.substr( task.find( "/")+1 );
        task = task.substr( 0, task.find( "/" ) );
        (*itT).location = task;
        std::cout << "Found task " << (*itT).name << " as " << (*itT).location << std::endl;
        foundTask = true;
        break;
      }
      if ( !foundTask ) {
        std::cout << "  --- no services for task " << (*itT).name << std::endl;
        message += "\nFound no DIM service for task " + (*itT).name + ", is it running ?";
      }
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        if ( foundTask ) {
          (*itH).loadFromMonObject( (*itT).location, update );
        } else {
          (*itH).createDummyHisto();
        }
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
void PresenterPage::uploadReference ( OMAlib* analysisLib, int startRun, std::string tck ) {
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    char startRunStr[30];
    sprintf( startRunStr, "%d", startRun );

    std::string root = analysisLib->refRoot() + "/" + (*itT).name;
    std::string fileName = root + "/" + tck + "_" + startRunStr + ".root";
    std::cout << "*** Try reference file " << fileName << std::endl;

    TFile* f = new TFile( fileName.c_str(),"READ");
    if ( f ) {
      if ( f->IsZombie() ) {
        delete f;
        f = NULL;
      }
    }
    if ( !f ) {
      if ( 1 != startRun ) {
        fileName = root + "/" + tck + "_1.root";
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
                                     const std::string & pastDuration ) {
  std::cout << "Load from Archive, time  Point " << timePoint
            << " pastDuration " << pastDuration << std::endl;
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      (*itH).deleteRootHist( );
    }
  }
  archive->closeFiles();
  
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    archive->setFiles( (*itT).name, timePoint, pastDuration );
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      archive->fillHistogramFromFiles( &(*itH) );
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
void PresenterPage::simpleDisplay (  TCanvas* editorCanvas ) {
  int nbPlots = 0;
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    nbPlots += (*itT).histos.size();
  }
  for ( std::vector<TrendingFile>::iterator itF = m_trends.begin(); m_trends.end() != itF; ++itF ) {
    nbPlots += (*itF).histos.size();
  }
  int nCol = 1;
  while ( nCol < nbPlots/nCol ) nCol += 1;
  int nRow = nbPlots / nCol;
  if ( nRow * nCol < nbPlots ) nRow++;

  std::cout << "For " << nbPlots << " plots, nCol " << nCol << " nRow " << nRow << std::endl;
  editorCanvas->Clear();

  int row = nRow-1;
  int col = 0;
  double rSize = 1./nRow;
  double cSize = 1./nCol;
  
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin(); (*itT).histos.end() != itH; ++itH ) {
      double xLow = col * cSize;
      double xHig = xLow + cSize;
      double yLow = row * rSize;
      double yHig = yLow + rSize;
      (*itH).draw( editorCanvas, xLow, yLow, xHig, yHig, NULL, false, false );
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
      (*itH).draw( editorCanvas, xLow, yLow, xHig, yHig, NULL, false, false );
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
  
  for ( itHP =  m_onlineHistosOnPage.begin(); m_onlineHistosOnPage.end() != itHP; ++itHP ) {

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
    float yMin =  1.0 - 0.05 * ( ((*itP).objects.size()+1)/2 );
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
  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {
    DisplayHistogram* dispH =  displayHisto( (*itHP)  );
    if ( NULL != dispH ) {
      TPad* pad = dispH->hostingPad();
      if ( NULL != pad ) dispH->setDrawingOptions( pad );
    }
  }
}

//=========================================================================
//  Fill the trend plots. Start and end time in integer. If endTime is zero, startTime is an offset
//=========================================================================
void PresenterPage::fillTrendingPlots ( int startTime, int endTime, bool update ) {
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
    int firstTimeThisTag = PresenterGaudi::trendingTool->firstTimeThisTag();
    std::cout << "Select for time range: " << startTime << " to " << endTime 
              << " last Tag version " << lastTagVersion << " starts at " << firstTimeThisTag << std::endl;
    
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
          (*itF).histos.end() != itH; ++itH ) {

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

      int theTime = startTime;
      std::vector< std::pair<int,double> > values ;

      //== Handle ratios: The tag name is a pair, separated by a '/'
      std::string myTag   = (*itH).shortName();
      std::string myRatio = "";
      if ( std::find( tags.begin(), tags.end(), myTag ) == tags.end() ) {
        std::cout << "Tag not found : " << myTag << " try to split." << std::endl;
        unsigned int barIndx = myTag.find( "|" );
        if ( barIndx < myTag.size() ) {
          myRatio = myTag.substr( barIndx+1 );
          myTag   = myTag.substr( 0, barIndx );
          std::cout << "  num " << myTag << " den " << myRatio << std::endl;
        }
      }
      
      while ( currVersion <= lastTagVersion ) {
        if ( "" == myRatio ) {
          status = PresenterGaudi::trendingTool->select( theTime , endTime , (*itH).shortName() );
          if ( status ) {
            float theValue ;
            // Read the values in the selected time interval and fill a vector
            while ( PresenterGaudi::trendingTool->nextValue( theTime , theValue ) ) {
              if ( theTime < startTime ) continue;
              if ( theTime > endTime   ) continue;
              values.push_back( std::pair<int,double>( theTime, theValue ) );
            }
          }
        } else {
          unsigned int indxNum = 0;
          unsigned int indxDen = 0;
          for ( unsigned int kk = 0; tags.size() > kk ; ++kk ) {
            if ( tags[kk] == myTag ) indxNum = kk;
            if ( tags[kk] == myRatio ) indxDen = kk;
          }
          std::cout << "IndxNum " << indxNum << " indxDen " << indxDen << std::endl;
          status = PresenterGaudi::trendingTool->select( theTime , endTime );
          if ( status ) {
            std::vector<float> theValues ;
            // Read the values in the selected time interval and fill a vector
            while ( PresenterGaudi::trendingTool->nextEvent( theTime , theValues ) ) {
              if ( theTime < startTime ) continue;
              if ( theTime > endTime   ) continue;
              double theValue = 0.;
              if ( fabs( theValues[indxDen] ) > 0.0001 ) theValue = theValues[indxNum] / theValues[indxDen];
              values.push_back( std::pair<int,double>( theTime, theValue ) );
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
      if ( values.size() > 0 ) {  
        values.push_back( std::pair<int,double>( endTime, values.back().second  ) ) ;
        std::cout << "from " << values[0].first << " to " << values[values.size()-1].first 
                  << " , size " << values.size() << std::endl;
      } else {
        values.push_back( std::pair<int,double>( startTime, 0. ) );
        values.push_back( std::pair<int,double>( endTime, 0. ) );
        std::cout << "For tag " << (*itH).shortName() << " no data found" << std::endl;
      }
      (*itH).createGraph( values, update );
    }
    PresenterGaudi::trendingTool->closeFile( ) ;
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
//=============================================================================
