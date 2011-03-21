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
      std::string file = myHist->hname();
      if ( file.find( ".trend" ) < file.size() ) file = file.substr( 0, file.find( ".trend" ) );
      unsigned indx = file.size();
      while ( 0 < indx && file[indx-1] != '/' ) --indx;
      file = file.substr(0,indx) + partition + "_" + file.substr( indx );
      std::string variable = myHist->algorithm();
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
          (*itT).histos.push_back( DisplayHistogram( myHist ) );
          existed = true;
          std::cout << myHist->task() << " ++ Histo   " << myHist->dimServiceName() << std::endl;
          break;
        }
      }
      if ( !existed ) {
        TaskHistos newTask;
        newTask.name = myHist->task();
        newTask.dead = false;
        newTask.histos.push_back( DisplayHistogram( myHist ) );
        m_tasks.push_back( newTask );
        std::cout << myHist->task() << " ** Histo   " << myHist->dimServiceName() << std::endl;
      }
    } else {    //== Analysis histograms: Get the source histograns
      anaHistos.push_back( myHist );
    }
  }

  //== Post process the analysis histograms as they can be nested

  for ( std::vector<OnlineHistogram*>::iterator itOH = anaHistos.begin(); anaHistos.end() != itOH; ++itOH ) {
    AnalysisHisto myAna;
    myAna.displayHisto = new DisplayHistogram( *itOH );
    (*itOH)->getCreationDirections( myAna.algorithm, myAna.histoNames, myAna.params );
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
            if ( (*itH).histo()->identifier() == histo->identifier() ) {
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
void PresenterPage::loadFromDIM( std::string& partition, bool update ) {
  if ( m_tasks.size() == 0 ) return;
  
  std::vector<std::string> knownTasks;
  HistTask::TaskList( "", knownTasks );

  std::cout << "Found " << knownTasks.size() << " tasks with 2011 publication." << std::endl;
  for ( std::vector<std::string>::iterator itS = knownTasks.begin();
        knownTasks.end() != itS; ++itS ) {
    std::cout << "   " << (*itS ) << std::endl;
  }
  
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    bool foundTheTask = false;
    std::string taskName = (*itT).name;
    if ( taskName == "GauchoJob" ) taskName = "Adder";

    for ( std::vector<std::string>::iterator itS = knownTasks.begin();
          knownTasks.end() != itS; ++itS ) {
      if ( (*itS).find( partition ) != 0 ) continue;
      if ( (*itS).find( taskName ) == std::string::npos ) continue;
      (*itT).location = *itS;
      //==== TEMPORARY PATCH === force partition number !!!!!
      if ( taskName == "Adder" ) (*itT).location = "PART01_Adder_1";  
      foundTheTask = true;
      break;
    }
    if ( foundTheTask ) {
      std::cout << "Search for services of task " << (*itT).location << std::endl;
      HistTask myHists( (*itT).location );

      std::vector<std::string> knownNames;
      //== Get the list of services...
      int kk = myHists.Directory( knownNames );
      std::cout << "Directory returned status " << kk << " with " << knownNames.size() << " histograms" << std::endl;
      //for ( std::vector<std::string>::iterator itS = knownNames.begin(); knownNames.end() != itS  ; ++itS ) {
      //  std::cout << "      -" << *itS << "-" << std::endl;
      //}

      std::vector<std::string> histNames;
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        std::string dimName = (*itH).histo()->dimServiceName();
        unsigned int pos = dimName.find( "/" );
        if ( pos < dimName.size() ) {
          dimName.erase( 0, pos+1 );
          pos = dimName.find( "/" );
          if ( pos < dimName.size() ) {
            dimName.erase( 0, pos+1 );
          }
          if ( dimName.find( "GauchoJob/" ) == 0 ) {
            dimName.erase( 0, 10 );
            dimName = dimName.substr(0, dimName.find("/"))+"/"+dimName;
          }
        }
        (*itH).setShortName( dimName );
        if ( std::find( knownNames.begin(), knownNames.end(), dimName ) != knownNames.end() ) {
          std::cout << "  ++ Search for '" << dimName << "'" << std::endl;
          histNames.push_back( dimName );
        } else {
          std::cout << "  -- Not found '" << dimName << "'" << std::endl;
        }
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
      }
      
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        bool found = false;
        for ( unsigned int indx = 0 ; histNames.size() > indx ; ++indx ) {
          if ( (*itH).shortName() == histNames[indx] ) {            
            if ( 0 != results[indx] ) {
              found = true;
              if ( update && 
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
      if ( !foundTask ) std::cout << "  --- no services for task " << (*itT).name << std::endl;

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
          delete tmp;
        }
      }
      std::cout << "*** References are set ***" << std::endl;
      f->Close();
      delete f;
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
      for ( std::vector<OnlineHistogram*>::iterator itOH = (*itA).onlineHistos.begin();
            (*itA).onlineHistos.end() != itOH; ++itOH ) {
        DisplayHistogram* dispH = displayHisto( (*itOH) );
        std::cout << "  get OnlineHistogram " << itOH-(*itA).onlineHistos.begin()
                  << " hname " << (*itOH)->hname()
                  << " pointer " << dispH << std::endl;

        if ( NULL != dispH ) {
          rootHists.push_back( dispH->rootHist() );
          if ( NULL != dispH->referenceHist() ) {
            refHists.push_back(  dispH->referenceHist() );
          }
        }
      }
      TH1* rootH = NULL;
      TH1* refH  = NULL;
      if ( rootHists.size() == (*itA).onlineHistos.size() ) {
        rootH = creator->exec( &rootHists,
                               &(*itA).params,
                               htitle, //(*itA).displayHisto->histo()->htitle(),
                               htitle,
                               NULL,
                               ref );
        if ( !update && refHists.size() == (*itA).onlineHistos.size() ) {
          refH = creator->exec( &refHists,
                                &(*itA).params,
                                htitle, //(*itA).displayHisto->histo()->htitle(),
                                htitle,
                                NULL,
                                ref );
        }
        std::cout << "Created analysis histogram " << htitle << " pointer " << rootH;
        if ( rootH ) std::cout << " integral " << rootH->Integral();
        std::cout << std::endl;
      }
      if ( rootH ) {
        if ( update ) {
          (*itA).displayHisto->copyFrom( rootH );
          delete rootH;
         } else {
          (*itA).displayHisto->setRootHist( rootH );
          (*itA).displayHisto->setDisplayOptions( );
          if ( refH ) (*itA).displayHisto->setReferenceHistogram( refH );
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
    archive->setFiles( (*itT).name, timePoint, pastDuration );
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      archive->fillHistogramFromFiles( &(*itH) );
      (*itH).setDisplayOptions();
      (*itH).prepareForDisplay();
    }
  }
  std::cout << "Archive loaded" << std::endl;
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
      if ( (*itDH).histo()->identifier() == onlHist->identifier() ) return &(*itDH);
    }
  }
  return NULL;
}

//=========================================================================
//== Give the (first) DisplayHistogram for the specified OnlineHistogram
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
//  Draw the full page
//=========================================================================
void PresenterPage::drawPage ( TCanvas* editorCanvas, OMAlib* analysisLib, bool fastHitMapDraw ) {

  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {

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

        std::cout << "== overlap: Mother " << mother << std::endl;
        std::cout << "   histo " << mother->histo << std::endl;

        for ( std::vector<OnlineHistoOnPage*>::iterator itPrev =  m_onlineHistosOnPage.begin();
              m_onlineHistosOnPage.end() != itPrev; ++itPrev ) {
          if ( (*itPrev)->histo == mother->histo) {
            std::cout << "  display histo " <<  displayHisto( (*itPrev) ) << std::endl;
            overlayOnPad = displayHisto( (*itPrev) )->hostingPad();
            break;
          }
        }
      }
    }

    DisplayHistogram* dispH =  displayHisto( (*itHP)  );
    if (  dispH ) {
      std::cout << "  display histo " << dispH->histo()->identifier()
                << " on pad X "<< xlow << " to "<< xup
                << "  Y " << ylow << " to " << yup
                << " overlay " << overlayOnPad << std::endl;
      dispH->draw( editorCanvas, xlow, ylow, xup, yup, analysisLib, fastHitMapDraw, overlayOnPad );
    }
  }
  std::cout << "Update the canvas" << std::endl;

  // workaround attempt for ROOT painter objects:
  // reliable random crashes when below invoked...
  // via TH painter (timing dependent?)
  editorCanvas->Update();
  updateDrawingOptions();
  editorCanvas->Update();
  std::cout << "Second Update of the canvas" << std::endl;
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
//  Fill the tren plots. Start and end time in integer. If endTime is zero, startTime is an offset
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
    std::cout << "Select for time range: " << startTime << " to " << endTime << std::endl;
    
    for ( std::vector<DisplayHistogram>::iterator itH = (*itF).histos.begin();
          (*itF).histos.end() != itH; ++itH ) {
      status = PresenterGaudi::trendingTool->select( startTime , endTime , (*itH).shortName() );
      if ( ! status ) {
        std::cerr << "  unknown tag name " << (*itH).shortName() << std::endl ;
        (*itH).createDummyHisto();
        continue;
      }

      int theTime ;
      float theValue ;
      std::vector< std::pair<int,double> > values ;
      values.push_back( std::pair<int,double>( startTime, 0. ) ) ;
      // Read the values in the selected time interval and fill a vector
      while ( PresenterGaudi::trendingTool->nextValue( theTime , theValue ) ) {
        if ( theTime < startTime ) continue;
        if ( theTime > endTime   ) continue;
        values.push_back( std::pair<int,double>( theTime, theValue ) );
      }
      values.push_back( std::pair<int,double>( endTime, values.back().second  ) ) ;
      values[0].second = values[1].second;
      std::cout << "from " << values[0].first << " to " << values[values.size()-1].first 
                << " , size " << values.size() << std::endl;
      (*itH).createGraph( values, update );
    }
    PresenterGaudi::trendingTool->closeFile( ) ;
  }
}
//=============================================================================
