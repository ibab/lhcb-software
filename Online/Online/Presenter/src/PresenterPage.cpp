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
  m_dimBrowser = NULL;
  m_onlineHistosOnPage.clear();
  m_tasks.clear();
  m_analysis.clear();
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
}

//=========================================================================
//  Prepare the histogram descriptions for access
//=========================================================================
void PresenterPage::prepareAccess( OnlineHistDB* histDB  ) {
  std::cout << "** Preparing access for " << m_onlineHistosOnPage.size() << " online histos on page" << std::endl;
  std::vector<OnlineHistogram*> anaHistos;
  anaHistos.reserve( 100 ); // avoid relocation
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
      anaHistos.push_back( (*itHP )->histo );
    }
  }
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
            if ( (*itH).histo() == histo ) {
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
        (*itH).setShortName( dimName );
        std::cout << "  ++ Search for    " << dimName << std::endl;
        histNames.push_back( dimName );
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
              (*itH).setDisplayOptions();  // Pass the DB flags to the root histogram
              (*itH).prepareForDisplay();
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
        TH1* tmp = analysisLib->findRootHistogram( (*itH).histo(), f );
        TH1* out = (TH1*)tmp->Clone();
        (*itH).setReferenceHistogram( out );
        delete tmp;
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
                  << " pointer " <<  dispH << std::endl;

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
        if ( refHists.size() == (*itA).onlineHistos.size() ) {
          refH = creator->exec( &refHists,
                                &(*itA).params,
                                htitle, //(*itA).displayHisto->histo()->htitle(),
                                htitle,
                                NULL,
                                ref );
        }
        std::cout << "Created analysis histogram " << htitle << std::endl;
      }
      if ( rootH ) {
        if ( update ) {
          (*itA).displayHisto->rootHist()->Reset();
          (*itA).displayHisto->rootHist()->Add( rootH, 1. );
          if ( refH ) {
            (*itA).displayHisto->referenceHist()->Reset();
            (*itA).displayHisto->referenceHist()->Add( refH, 1. );
          }
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
  std::cout << "Load from Archivem time  Point " << timePoint
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
//=============================================================================
