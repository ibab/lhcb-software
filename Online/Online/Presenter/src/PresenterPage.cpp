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
}
//=============================================================================
// Destructor
//=============================================================================
PresenterPage::~PresenterPage() {}


//=========================================================================
//  Reset and clear. remove all objects created and owned by the page.
//=========================================================================
void PresenterPage::clear ( ) {
  m_onlineHistosOnPage.clear();
  m_tasks.clear();
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    delete (*itA).displayHisto;
  }
  m_analysis.clear();
}

//=========================================================================
//  Prepare the histogram descriptions for access
//=========================================================================
void PresenterPage::prepareAccess( OnlineHistDB* histDB  ) {
  std::cout << "** Preparing access for " << m_onlineHistosOnPage.size() << " online histos on page" << std::endl;
  for ( std::vector<OnlineHistoOnPage*>::iterator itHP =  m_onlineHistosOnPage.begin();
        m_onlineHistosOnPage.end() != itHP; ++itHP ) {

    if ( !(*itHP)->histo->isAnaHist() ) {
      bool existed = false;
      for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
        if ( (*itT).name == (*itHP)->histo->task() ) {
          (*itT).histos.push_back( DisplayHistogram( (*itHP)->histo ) );
          existed = true;
          std::cout << (*itHP)->histo->task() << " ++ Histo   " << (*itHP)->histo->dimServiceName() << std::endl;
          break;
        }
      }
      if ( !existed ) {
        TaskHistos newTask;
        newTask.name = (*itHP)->histo->task();
        newTask.dead = false;
        newTask.histos.push_back( DisplayHistogram( (*itHP)->histo ) );
        m_tasks.push_back( newTask );
        std::cout << (*itHP)->histo->task() << " ** Histo   " << (*itHP)->histo->dimServiceName() << std::endl;
      }
    } else {    //== Analysis histograms: Get the source histograns
      AnalysisHisto myAna;
      myAna.displayHisto = new DisplayHistogram( (*itHP)->histo );
      (*itHP)->histo->getCreationDirections( myAna.algorithm, myAna.histoNames, myAna.params );
      for (unsigned int i=0; i< myAna.histoNames.size(); ++i) {
        OnlineHistogram* histo = histDB->getHistogram( myAna.histoNames[i] );
        myAna.onlineHistos.push_back( histo );
        bool existed = false;
        for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
          if ( (*itT).name == histo->task() ) {
            for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
                  (*itT).histos.end() != itH; ++itH ) {
              if ( (*itH).histo() == histo ) {
                std::cout << "              already existing " << histo->identifier() << std::endl;
                existed = true;
                break;
              }
            }
            if ( !existed ) {
              (*itT).histos.push_back( DisplayHistogram( histo ) );
              existed = true;
              std::cout << histo->task() << " ++++ Histo " << histo->identifier() << std::endl;
            }
            break;
          }
        }
        if ( !existed ) {
          TaskHistos newTask;
          newTask.name = histo->task();
          newTask.dead = false;
          newTask.histos.push_back( DisplayHistogram( histo ) );
          m_tasks.push_back( newTask );
          std::cout << histo->task() << " **** Histo " << histo->identifier() << std::endl;
        }
      }
      m_analysis.push_back( myAna );
    }
  }
  std::cout << ".. Now we have " <<  m_tasks.size() << " tasks on page" << std::endl;
}
//=========================================================================
//  Load from the monitoring service
//=========================================================================
void PresenterPage::loadFromDIM( std::string& partition, bool update ) {
  std::vector<std::string> knownTasks;
  HistTask::TaskList( "", knownTasks );

  std::cout << "Found " << knownTasks.size() << " tasks with 2011 publication." << std::endl;
  for ( std::vector<std::string>::iterator itS = knownTasks.begin();
        knownTasks.end() != itS; ++itS ) {
    std::cout << "   " << (*itS ) << std::endl;
  }
  
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    bool foundTheTask = false;
    for ( std::vector<std::string>::iterator itS = knownTasks.begin();
          knownTasks.end() != itS; ++itS ) {
      if ( (*itS).find( partition ) != 0 ) continue;
      if ( (*itS).find( (*itT).name ) == std::string::npos ) continue;
      (*itT).location = *itS;
      foundTheTask = true;
      break;
    }
    if ( foundTheTask ) {
      HistTask myHists( (*itT).location );
      std::vector<std::string> allHists;
      myHists.Directory( allHists );

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
        }
        
        std::cout << "-- Search for DIM name " << dimName << std::endl;
        if ( std::find( allHists.begin(), allHists.end(), dimName ) != allHists.end() ) {
          histNames.push_back( dimName );
          (*itH).setShortName( dimName );
          std::cout << "  ++ Search for    " << dimName << std::endl;
        } else {
          std::cout << "  -- Not Available " << dimName << std::endl;
          (*itH).setShortName( "" );
        }
      }
      std::vector<TObject*> results;
      std::cout << "before calling Histos, size " << histNames.size() << std::endl;
      int status = myHists.Histos( histNames, results );
      std::cout << "Load result  = " << status << " result size " << results.size() << std::endl;
      while ( results.size() < histNames.size() ) results.push_back( 0 );
      unsigned int indx = 0;
      for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
            (*itT).histos.end() != itH; ++itH ) {
        if ( (*itH).shortName() == histNames[indx] ) {
          if ( 0 == results[indx] ) {
            std::cout << "-- not found " << histNames[indx] << std::endl;
            (*itH).createDummyHisto();
          } else {
            std::cout << "++ set histo " << histNames[indx] << std::endl;
            if ( update && 
                 ( (*itH).rootHist()->GetNbinsX() == ((TH1*)results[indx])->GetNbinsX() ) ) {
              (*itH).rootHist()->Reset();
              (*itH).rootHist()->Add( (TH1*)results[indx], 1. );
              delete results[indx];
              (*itH).prepareForDisplay();
              (*itH).setDisplayOptions();  // Pass the DB flags to the root histogram
              (*itH).setDrawingOptions( (*itH).hostingPad() );
              (*itH).hostingPad()->Modified();
            } else {
              if ( NULL != (*itH).rootHist() ) delete (*itH).rootHist();
              (*itH).setRootHist( (TH1*) results[indx] );
              (*itH).setDisplayOptions();  // Pass the DB flags to the root histogram
              (*itH).prepareForDisplay();
            }
          }
          indx++;
        } else {
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
  std::cout << "   ... display prepared" << std::endl;
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
        TH1* out = analysisLib->findRootHistogram( (*itH).histo(), f );
        (*itH).setReferenceHistogram( out );
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
void PresenterPage::buildAnalysisHistos (OMAlib* analysisLib ) {
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    std::cout << "Get creator for algorithm '" << (*itA).algorithm << "'" << std::endl;
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>( analysisLib->getAlg( (*itA).algorithm ) );
    if ( creator ) {
      std::string htitle( (*itA).displayHisto->histo()->htitle());
      std::cout << "  Title " << htitle << std::endl;
      TH1* ref=NULL;
      if ( creator->needRef() && (*itA).histoNames.size() > 0 ) {
        std::cout << "   .. get ref for " << (*itA).histoNames[0] << " pointer " << (*itA).onlineHistos[0] << std::endl;
        DisplayHistogram* myDisplay = displayHisto( (*itA).onlineHistos[0] );
        if ( NULL == myDisplay ) {
          std::cout << "   corresponding DisplayHisto not found" << std::endl;
        } else {
          ref = myDisplay->referenceHist();
        }
      }
      std::vector<TH1*> rootHists;
      for ( std::vector<OnlineHistogram*>::iterator itOH = (*itA).onlineHistos.begin();
            (*itA).onlineHistos.end() != itOH; ++itOH ) {
        std::cout << "  get OnlineHistogram " << itOH-(*itA).onlineHistos.begin()
                  << " hname " << (*itOH)->hname()
                  << " pointer " <<  displayHisto( (*itOH) ) << std::endl;

        if ( NULL !=  displayHisto( (*itOH) ) ) rootHists.push_back( displayHisto( (*itOH) )->rootHist() );
      }
      std::cout << "   before executing the analysis operation " << std::endl;
      TH1* rootH = NULL;
      if ( rootHists.size() == (*itA).onlineHistos.size() ) {
        rootH = creator->exec( &rootHists,
                               &(*itA).params,
                               htitle, //(*itA).displayHisto->histo()->htitle(),
                               htitle,
                               NULL,
                               ref );
        std::cout << "Created analysis histogram " << htitle << std::endl;
      }
      if ( rootH ) {
        (*itA).displayHisto->setRootHist( rootH );
        (*itA).displayHisto->setDisplayOptions( );
      } else {
        std::cout<< "creator algorithm failed!"<<std::endl;
        (*itA).displayHisto->createDummyHisto();
      }
    } else {
      std::cout << "creator algorithm for virtual histogram not found!" << std::endl;
      (*itA).displayHisto->createDummyHisto();
    }
  }
}

//=========================================================================
//  Load the histograms from Archives, i.e. from history files.
//=========================================================================
void PresenterPage::loadFromArchive( Archive* archive,
                                     const std::string & timePoint,
                                     const std::string & pastDuration ) {
  std::cout << "Load from Archivem time  Point " << timePoint
            << " pastDuration " << pastDuration << std::endl;
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    archive->setFiles( (*itT).name, timePoint, pastDuration );
    for ( std::vector<DisplayHistogram>::iterator itH = (*itT).histos.begin();
          (*itT).histos.end() != itH; ++itH ) {
      archive->fillHistogramFromFiles( &(*itH) );
    }
  }
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
  if ( onlHist->isAnaHist() ) {
    for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
      if ( (*itA).displayHisto->histo() == onlHist ) return (*itA).displayHisto;
    }
    return NULL;
  }
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      if ( (*itDH).histo() == onlHist ) return &(*itDH);
    }
  }
  return NULL;
}

//=========================================================================
//== Give the (first) DisplayHistogram for the specified OnlineHistogram
//=========================================================================
DisplayHistogram* PresenterPage::displayHisto( TH1* hist ) {
  for ( std::vector<AnalysisHisto>::iterator itA = m_analysis.begin(); m_analysis.end() != itA; ++itA ) {
    if ( (*itA).displayHisto->rootHist() == hist ) return (*itA).displayHisto;
  }
  for ( std::vector<TaskHistos>::iterator itT = m_tasks.begin(); m_tasks.end() != itT; ++itT ) {
    for ( std::vector<DisplayHistogram>::iterator itDH = (*itT).histos.begin(); (*itT).histos.end() != itDH; ++itDH ) {
      if ( (*itDH).rootHist() == hist ) return &(*itDH);
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
//=============================================================================
