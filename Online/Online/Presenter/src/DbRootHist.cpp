// $Id $
// TH1::SetMaximum() and TH1::SetMinimum()

#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TMath.h>
#include <TObjArray.h>

#include <TFile.h>
#include <TPaveStats.h>
#include <TCanvas.h>
#include <TKey.h>

#include <TThread.h>

#include <TImage.h>
#include <TText.h>
//#include <TGMsgBox.h>

#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include "presenter.h"
#include "DbRootHist.h"

#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2D.h"

using namespace pres;

DbRootHist::DbRootHist(const std::string & identifier,
                       const std::string & dimServiceName,
                       int refreshTime,
                       int instance,
                       OnlineHistDB* histogramDB,
                       OMAlib* analysisLib,
                       OnlineHistogram* onlineHist,
                       pres::MsgLevel verbosity,
                       DimBrowser* DimBr,
                       boost::recursive_mutex & oraMutex,
                       boost::recursive_mutex & dimMutex,
                       std::vector<std::string*> & tasksNotRunning,
                       boost::recursive_mutex & rootMutex)
: HistogramIdentifier(dimServiceName),
  rootHistogram(NULL),
  hostingPad(NULL),
  m_drawPattern(NULL),
//  histogramImage(NULL),
//  m_usingGauchocommentDimInfo(false);
  m_gauchocommentDimInfo(NULL),
  m_offsetHistogram(NULL),
  m_trendTimeScale(s_pageRefreshRate/1000),
  m_trendBin(1),
  m_isAnaHist(false),
  m_anaLoaded(false),
  m_analysisLib(analysisLib),
  m_refreshTime(refreshTime),
//  m_toRefresh(false),
  m_cleared(false),
  m_fastHitmapPlot(false),
//  m_histogramType(),
//  m_hname(m_histogramName),
  m_instance(instance),
  m_waitTime(20),
  m_msgBoxReturnCode(0),
  m_identifier(identifier),
  m_session(histogramDB),
  m_onlineHistogram(NULL),
  m_isEmptyHisto(false),
  m_retryInit(2),
  m_refOption("AREA"),
  m_histogramImage(NULL),
  m_prettyPalette(NULL),
//  m_textTitle(NULL),
  m_reference(NULL),
  m_startRun(1),
  m_dataType("default"),  
  m_dimServiceName(dimServiceName),
  m_dimInfo(NULL),
  m_dimInfoMonObject(NULL),
  m_verbosity(verbosity),
  m_dimBrowser(DimBr),
  m_partition(dimServiceName),
  m_titpave(NULL),
  m_statpave(NULL),
  m_historyTrendPlotMode(false),
  m_isOverlap(false),
  m_oraMutex(&oraMutex),
  m_dimMutex(&dimMutex),
  m_tasksNotRunning(&tasksNotRunning),
  m_rootMutex(&rootMutex)
{
  // Prevent ROOT booking
  TH1::AddDirectory(kFALSE);
  m_prettyPalette = new TImagePalette(1,0);
//  m_textTitle = new TText(5, 5, "");

  // if dimBrowser is specified, retrieve dim service name from partition name
  if(m_dimBrowser && (!histogramDB)) {
//    m_partition = dimServiceName;
    m_partition = partitionName();
  }
  
  if (histogramDB) {
    if (onlineHist) { setOnlineHistogram(onlineHist); }
    else { connectToDB(histogramDB, "_NONE_", 1); }
  }

//  if( false == m_isAnaHist) {
//    if(m_dimBrowser) {
//      m_dimServiceName = assembleCurrentDimServiceName();
//    }
//  }
  
  initHistogram();
  if (m_verbosity >= Verbose && m_isEmptyHisto) {
    std::cout << "Histogram " << m_identifier << " empty after Init" << std::endl;
  }
  if (false == m_isEmptyHisto) {
    fillHistogram();
    if (m_verbosity >= Verbose && m_isEmptyHisto) {
      std::cout << "Histogram " << m_identifier << " empty after Fill" << std::endl;
    }
  } else {
    beEmptyHisto(); 
  }
      setTH1FromDB();
}
DbRootHist::~DbRootHist()
{
//  m_toRefresh = false;
  if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = NULL;}
  if (rootHistogram) { delete rootHistogram; rootHistogram = NULL; }
  if (m_reference) { delete  m_reference; m_reference = NULL; }
  if (m_histogramImage) { delete m_histogramImage; m_histogramImage = NULL; }
  if (hostingPad && !m_isOverlap) { delete hostingPad; hostingPad = NULL; }
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = NULL; }
  if (m_dimInfoMonObject) { delete m_dimInfoMonObject; m_dimInfoMonObject = NULL; }
  if (m_titpave) {delete m_titpave; m_titpave = NULL; }
  if (m_statpave) {delete m_statpave; m_statpave = NULL; }
  if (m_prettyPalette) {delete m_prettyPalette; m_prettyPalette = NULL; }
//  if (m_textTitle) {delete m_textTitle; m_textTitle = NULL; }  
  
  cleanAnaSources();
}
void DbRootHist::cleanAnaSources()
{
  for (unsigned int i=0; i< m_anaSources.size(); ++i) {
    delete m_anaSources[i];
    m_anaSources[i] = NULL;
  }
  m_anaSources.clear();
}
void DbRootHist::loadAnaSources()
{
  boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  if (oraLock && onlineHistogram()) {
    m_isAnaHist=onlineHistogram()->isAnaHist();
    if (m_isAnaHist && !m_anaLoaded) {
      onlineHistogram()->getCreationDirections(m_creationAlgorithm,
                                               m_sourcenames,
                                               m_parameters);

      cleanAnaSources();
      bool sourcesOK = true;
      for (unsigned int i=0; i< m_sourcenames.size(); ++i) {
        OnlineHistogram* histo = dbSession()->getHistogram(m_sourcenames[i]);
        m_anaSources.push_back(new DbRootHist(m_sourcenames[i],
                                              m_dimServiceName,
                                              m_refreshTime,
                                              999,
                                              dbSession(),
                                              m_analysisLib,
                                              histo,
                                              m_verbosity,
                                              m_dimBrowser,
                                              *m_oraMutex,
                                              *m_dimMutex,
                                              *m_tasksNotRunning,
                                              *m_rootMutex));
        if (NULL == m_anaSources[i]->rootHistogram) { sourcesOK = false; }
      }
      m_anaLoaded = true;
      if (!sourcesOK) { // cannot get sources from DIM
        beEmptyHisto();
      }
    }
  }
  if (m_reference) {
    //    m_reference->SetBit(kCanDelete);
    delete m_reference;
    m_reference = NULL;
  }
}
void DbRootHist::setDimServiceName(std::string newDimServiceName)
{
  HistogramIdentifier histogramIdentifier(newDimServiceName);
  if (0 == histogramIdentifier.histogramIdentifier().compare(m_identifier) &&
      0 == histogramIdentifier.histogramSetName().compare(m_setName) &&
      0 == histogramIdentifier.histogramType().compare(m_histogramType)) {
    setIdentifiersFromDim(newDimServiceName);

    if (s_pfixMonProfile != m_histogramType &&
        s_pfixMonH1D != m_histogramType &&
        s_pfixMonH2D != m_histogramType) {  

      if (m_dimInfo) { delete m_dimInfo; m_dimInfo = NULL; }
      m_dimInfo = new DimInfo(newDimServiceName.c_str(), m_refreshTime,
                            (float)-1.0);
    } else {
      if (m_dimInfoMonObject) {
        delete m_dimInfoMonObject;
        m_dimInfoMonObject = NULL;
      }
      m_dimInfoMonObject = new DimInfoMonObject(newDimServiceName.c_str(),
                                                m_refreshTime, "GauchoDimInfoMonObjectPresenter");
    }

    m_dimServiceName = newDimServiceName;
    if (m_verbosity >= Verbose) {
      std::cout << "DimInfo dimServiceName: " << newDimServiceName << std::endl;
    }
    if (m_dimInfo || m_dimInfoMonObject) {
      initHistogram();
      if (false == m_isEmptyHisto) {
        fillHistogram();
        setTH1FromDB();
      }
    } else {
      beEmptyHisto();
    }
  } else { //TODO: TMsg warning
  }
}
void DbRootHist::enableEdit()
{
  if (hostingPad) {
    hostingPad->SetEditable(true);
    hostingPad->ResetBit(TPad::kCannotMove);
  }
}

void DbRootHist::disableEdit()
{
  if (hostingPad) {
    hostingPad->SetEditable(false);
    hostingPad->SetBit(TPad::kCannotMove);
  }
}
void DbRootHist::enableClear()
{
  // TODO: deal with errors and Reset & Add instead of delete/new
  if (rootHistogram) {
    if (!m_isAnaHist) {
      m_cleared = true;
      if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = 0;}
      if (s_H1D == m_histogramType) {
        m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
//    m_offsetHistogram = (TH1F*)rootHistogram->Clone("m_offsetHistogram");
      } else if (s_pfixMonH1D == m_histogramType) {
        m_offsetHistogram = new TH1D(*dynamic_cast<TH1D*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      } else if (s_H2D == m_histogramType) {
        m_offsetHistogram = new TH2F(*dynamic_cast<TH2F*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      } else if (s_pfixMonH2D == m_histogramType) {
        m_offsetHistogram = new TH2D(*dynamic_cast<TH2D*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      } else if (s_P1D == m_histogramType ||
                 s_HPD == m_histogramType) {
//               s_P2D == m_histogramType                  
        m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));        
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      } else if (s_pfixMonProfile == m_histogramType) {
        m_offsetHistogram = new TProfile(*dynamic_cast<TProfile*>(rootHistogram));        
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      }
      if (m_offsetHistogram) { m_offsetHistogram->AddDirectory(kFALSE); }
    } else { // analysis histogram
      std::vector<DbRootHist*>::iterator hs;
      for (hs = m_anaSources.begin(); hs != m_anaSources.end(); ++hs) {
        (*hs)->enableClear();
      }
    }
  }
}
void DbRootHist::disableClear()
{
  m_cleared = false;
}
void DbRootHist::initHistogram()
{
  
  if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = NULL;}
  if (rootHistogram) { delete rootHistogram; rootHistogram = NULL; }
  if (m_reference) { delete  m_reference; m_reference = NULL; }

  // If not AnaLib hist:
  if (!m_isAnaHist && (m_retryInit > 0) && m_dimBrowser) {
    
    // sed partition   
    if (m_onlineHistogram && m_session && (m_retryInit > 1)) {
      m_dimServiceName = m_onlineHistogram->dimServiceName();
      
      if (m_verbosity >= Verbose) {
        std::cout << "dimServiceName from DB: " << m_dimServiceName << std::endl;
      }      
// TODO: Reset Partition member from main GUI?
// append empty partitions too, w/o _...            
    HistogramIdentifier histogramIdentifier1(m_dimServiceName);
    std::string histType(histogramIdentifier1.histogramType());
      m_dimServiceName.erase(0, m_dimServiceName.find(s_underscore));
      if (s_CNT != histType) {
        m_dimServiceName = histType + s_slash + m_partition + m_dimServiceName;
      } else {
        m_dimServiceName = m_partition + m_dimServiceName;
      }
      
      if (m_verbosity >= Verbose) {
        std::cout << "dimServiceName w partition: " << m_dimServiceName << std::endl;
      }            
    }    

    HistogramIdentifier histogramIdentifier(m_dimServiceName);
    m_histogramType = histogramIdentifier.histogramType();
    if (s_P1D == m_histogramType || s_HPD == m_histogramType ||
        s_H2D == m_histogramType || s_H1D == m_histogramType ||
        s_CNT == m_histogramType ) {
      if( m_dimServiceName.size() > 0 ) {
        if (m_dimInfo) { delete m_dimInfo; m_dimInfo = 0; }
        m_dimInfo = new DimInfo(m_dimServiceName.c_str(), m_refreshTime, (float)-1.0);
      }       
// || s_P2D == m_histogramType          
      if (m_dimInfo) {
        beRegularHisto();
        std::string noGauchocomment = "No gauchocomment";
        if (m_gauchocommentDimInfo) {
          delete m_gauchocommentDimInfo; m_gauchocommentDimInfo = 0;
        }
        
        m_gauchocommentDimInfo = new DimInfo(gauchocommentEric().c_str(),
                                             m_refreshTime,
                                             (char*)noGauchocomment.c_str());
    
        m_serviceSize = m_gauchocommentDimInfo->getSize()/sizeof(char);
        while (m_serviceSize <= 0) {
          gSystem->Sleep(m_waitTime);
          m_serviceSize = m_gauchocommentDimInfo->getSize()/sizeof(char);
        }
        // Does not look like a Gaucho job. Let's see if it is a CCPC...
        if ( 0 == noGauchocomment.compare(m_gauchocommentDimInfo->getString())) {
          if (m_gauchocommentDimInfo) {
            delete m_gauchocommentDimInfo; m_gauchocommentDimInfo = 0;
          }
          m_gauchocommentDimInfo = new DimInfo(gauchocommentBeat().c_str(),
                                               m_refreshTime,
                                               (char*)noGauchocomment.c_str());
          m_serviceSize = m_gauchocommentDimInfo->getSize()/sizeof(char);
          while (m_serviceSize <= 0) {
            gSystem->Sleep(m_waitTime);
            m_serviceSize = m_gauchocommentDimInfo->getSize()/sizeof(char);
          }
        }
        // use ROOT cycle notion ";" for tracking instances
        m_histoRootName = TString(Form("%s;%i",
                                       m_identifier.c_str(),
                                       m_instance));
        m_histoRootTitle = TString(Form("%s",
                                        m_gauchocommentDimInfo->getString()));
//        m_textTitle->SetText(5, 5, m_histoRootTitle.Data());
        if (m_gauchocommentDimInfo) {
          delete m_gauchocommentDimInfo;
          m_gauchocommentDimInfo = NULL;
        }
    
        int m_serviceSize = m_dimInfo->getSize()/sizeof(float);    
        while (m_serviceSize <= 0) {
          gSystem->Sleep(m_waitTime);
          m_serviceSize = m_dimInfo->getSize()/sizeof(float);
        }    
        float* histoDimData;
        if (-1.0 != m_dimInfo->getFloat()) {

          m_retryInit = 0;
          
          if (m_verbosity >= Verbose) {
            std::cout << "Using dimServiceName from DB." << std::endl;
          }
          
          histoDimData = (float*) m_dimInfo->getData();
          if (s_H1D == m_histogramType) {
            const int   nBins   = (int) histoDimData[1];
            const float xMin   = (float) histoDimData[2];
            const float xMax   = (float) histoDimData[3];
    //      const int   entries = (int) histoDimData[4];
            if (!rootHistogram) {
              rootHistogram = new TH1F(m_histoRootName.Data(),
                                       m_histoRootTitle.Data(),
                                       nBins, xMin, xMax);
            }
          } else if (s_H2D == m_histogramType) {
            const int   nBinsX   = (int) histoDimData[1];
            const float xMin    = (float) histoDimData[2];
            const float xMax    = (float) histoDimData[3];
            const int   nBinsY   = (int) histoDimData[4];
            const float yMin    = (float) histoDimData[5];
            const float yMax    = (float) histoDimData[6];
    //      const float entries  = histoDimData[7];
            if (!rootHistogram) {
              rootHistogram = new TH2F(m_histoRootName.Data(),
                                       m_histoRootTitle.Data(),
                                       nBinsX, xMin, xMax,
                                       nBinsY, yMin, yMax);
            }
          } else if (s_P1D == m_histogramType || s_HPD == m_histogramType) {
            const int   nBins   = (int) histoDimData[1];
            const float xMin    = histoDimData[2];
            const float xMax    = histoDimData[3];
    //      const int   entries = (int) histoDimData[4];
    //      float* entriesPerBin;
    //      float* sumWTPerBin;
    //      float* sumWT2PerBin;
            if (!rootHistogram) {
              rootHistogram = new TH1F(m_histoRootName.Data(),
                                       m_histoRootTitle.Data(),
                                       nBins, xMin, xMax);
            }
          } else if (s_CNT == m_histogramType &&
                     isEFF()) {
            int cnt_size = m_dimInfo->getSize()/sizeof(char);
            while (cnt_size <= 0) {
              gSystem->Sleep(m_waitTime);
              cnt_size = m_dimInfo->getSize()/sizeof(char);
            }
            char* cnt_comment  = (char*) m_dimInfo->getData();
            std::stringstream cntCommentStream;    
            for (int ptr = 2*sizeof(double); ptr < cnt_size; ++ptr ){
              cntCommentStream << (char)cnt_comment[ptr];               
            }
            cntCommentStream << std::endl;
            m_histoRootTitle = TString(Form("%s",
                                         cntCommentStream.str().c_str()));                                        
             gStyle->SetTimeOffset(m_offsetTime.Convert());
             if (!rootHistogram) {
              rootHistogram = new TH1D(m_histoRootName.Data(),m_histoRootTitle.Data(),
                                       10, 0, 10 * m_trendTimeScale);
             }
             rootHistogram->GetXaxis()->SetTimeDisplay(1);
          }
        } else {
          // cannot get sources from DIM
          if (m_retryInit == 1) { beEmptyHisto(); }
        }
      }
    } else if (s_pfixMonProfile == m_histogramType ||
               s_pfixMonH1D == m_histogramType ||
               s_pfixMonH2D == m_histogramType) {
      if (m_dimInfoMonObject) {
        delete m_dimInfoMonObject;
        m_dimInfoMonObject = NULL;
      }
                                             
      m_dimInfoMonObject = new DimInfoMonObject(m_dimServiceName.c_str(),
                                                m_refreshTime, "GauchoDimInfoMonObjectPresenter");
        if (m_dimInfoMonObject && m_dimInfoMonObject->createMonObject()) {
          
          m_retryInit = 0;          
          
          if (m_verbosity >= Verbose) {
            std::cout << "GauchoDimInfoMonObjectPresenter: Using dimServiceName from DB." << std::endl;
          } 
      
          TString histoRootName;
          TString histoRootTitle;
    
          if (s_pfixMonH1D == m_histogramType){
            MonH1D* monTH1D = static_cast<MonH1D*>(m_dimInfoMonObject->monObject());
            if (NULL != monTH1D) {
              histoRootName = TString(Form("%s__instance__%i",
                                           monTH1D->histName().c_str(),
                                           m_instance));
              histoRootTitle = TString(Form("%s",
                                            monTH1D->histTitle().c_str()));
              monTH1D->createObject(histoRootName.Data());
              monTH1D->hist()->SetTitle(histoRootTitle);
              if (m_verbosity >= Debug) { monTH1D->print(); }
              rootHistogram = monTH1D->hist();
            }
          } else if (s_pfixMonH2D == m_histogramType){
            MonH2D* monTH2D = static_cast<MonH2D*>(m_dimInfoMonObject->monObject());
            if (NULL != monTH2D) {
              histoRootName = TString(Form("%s__instance__%i",
                                           monTH2D->histName().c_str(),
                                           m_instance));
              histoRootTitle = TString(Form("%s",
                                            monTH2D->histTitle().c_str()));
              monTH2D->createObject(histoRootName.Data());
              monTH2D->hist()->SetTitle(histoRootTitle);
              if (m_verbosity >= Debug) { monTH2D->print(); }
              rootHistogram = monTH2D->hist();
            }
          } else if (s_pfixMonProfile == m_histogramType){
            MonProfile* monProfile = static_cast<MonProfile*>(m_dimInfoMonObject->monObject());
            if (NULL != monProfile) {
              histoRootName = TString(Form("%s__instance__%i",
                                           monProfile->profileName().c_str(),
                                           m_instance));
              histoRootTitle = TString(Form("%s",
                                            monProfile->profileTitle().c_str()));
              monProfile->createObject(histoRootName.Data());
              monProfile->profile()->SetTitle(histoRootTitle);
              if (m_verbosity >= Debug) { monProfile->print(); }
              rootHistogram = monProfile->profile();
            }
          } else {
            std::cout << "MonObject not included in the Presenter: " <<
                         m_histogramType << std::endl;
            rootHistogram =  0;
          }
          if (rootHistogram) {
            m_histoRootName = TString(Form("%s;%i",
                          m_identifier.c_str(),
                          m_instance));
            rootHistogram->AddDirectory(kFALSE); }
            rootHistogram->SetName(m_histoRootName);
        } else {
          if (m_retryInit == 1) {beEmptyHisto(); }
        }
    }
    // cannot get sources from DIM
    if (m_dimBrowser && (m_retryInit > 0)) {
      m_dimServiceName = assembleCurrentDimServiceName();
      if (m_verbosity >= Verbose) {
        std::cout << "DB DIM field invalid, retrying init using "
                  << m_dimServiceName << std::endl;
      }      
     
      --m_retryInit;
      if (m_verbosity >= Verbose) {
        std::cout << "retrying Init." << std::endl;
      }
      initHistogram();
    }
  } else if (m_isAnaHist && m_anaSources.size() > 0) { // analib hist
    std::vector<TH1*> sources(m_anaSources.size());
    bool sourcesOk = true;
    for (unsigned int i=0; i< m_anaSources.size(); ++i) {
      sources[i]= m_anaSources[i]->rootHistogram;
      if (m_anaSources[i]->isEmptyHisto() ) {
        sourcesOk = false;
        if (m_verbosity >= Verbose) {
          std::cout << "source " << i << " is empty" << std::endl;
        }
      }
    }
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
      (m_analysisLib->getAlg(m_creationAlgorithm));
    if(creator && sourcesOk) {
      std::string htitle(onlineHistogram()->htitle());
      rootHistogram = creator->exec(&sources, &m_parameters,
                                    m_identifier,
                                    htitle,
                                    isEmptyHisto() ? NULL : rootHistogram);
      beRegularHisto();
      if (m_verbosity >= Verbose && (!rootHistogram)) { 
        std::cout<< "creator alg. failed!"<<std::endl;
      }
    }
    else {
      if (m_verbosity >= Verbose) {
        std::cout << "creator alg. or sources not found!" << std::endl;
      }
      beEmptyHisto(); 
    }
    if (!rootHistogram) {beEmptyHisto(); }
  }
  if (!rootHistogram) {
    beEmptyHisto();
  } else if (rootHistogram) {
    rootHistogram->SetBit(kNoContextMenu);
  }
}
void DbRootHist::beEmptyHisto()
{
  m_isEmptyHisto = true;
  if (rootHistogram) { delete rootHistogram; rootHistogram = 0; }
  std::string dummyTitle = "ERROR: missing sources for ";
  dummyTitle += identifier();

//  m_onlineHistogram->hstype().compare(s_P1D) ||
//  m_onlineHistogram->hstype().compare(s_HPD)) {
//  m_onlineHistogram->hstype().compare(s_H2D)) {
//  m_onlineHistogram->hstype().compare(s_H1D)) {

//  if (s_H1D  == m_histogramType) {
//    rootHistogram = new TH1F(m_histoRootName.Data(),
//                             dummyTitle.c_str(),
//                             1, 0., 1.);
//  } else if (s_H2D == m_histogramType) {
//    rootHistogram = new TH2F(m_histoRootName.Data(),
//                             dummyTitle.c_str(),
//                             1, 0., 1.,
//                             1, 0., 1.);
//  } else if (s_P1D == m_histogramType ||
//             s_HPD == m_histogramType ||
//             s_pfixMonProfile == m_histogramType) {
    rootHistogram = new TH1F(identifier().c_str(),
                                 dummyTitle.c_str(),
                                 1, 0., 1.);
//  } else if (s_P2D == m_histogramType) {
//  } else if (s_CNT == m_histogramType) {
//  }
  if(rootHistogram) {
    rootHistogram->SetBit(kNoContextMenu);
    rootHistogram->AddDirectory(kFALSE);
    setRootHistogram(rootHistogram);
  }
}
void DbRootHist::fillHistogram()
{
  // TODO: rebin: nBins nBinsX nBinsY initHistogram
//  if (!m_isAnaHist && m_toRefresh) {
  if (rootHistogram && s_CNT != m_histogramType) {
    rootHistogram->Reset();
  }
  if (!m_isAnaHist) {
    if (s_P1D == m_histogramType || s_HPD == m_histogramType || s_H2D == m_histogramType ||
        s_H1D == m_histogramType) {
// || s_P2D == m_histogramType          
      if (m_dimInfo) {
    // wait until data has arrived
    int m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    while (m_serviceSize <= 0) {
      gSystem->Sleep(m_waitTime);
      m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    }
    if (-1.0 != m_dimInfo->getFloat() && rootHistogram && !m_isEmptyHisto) {
      float* histoDimData;
      histoDimData = (float*) m_dimInfo->getData();
      if (s_H1D == m_histogramType) {
        const int   nBins   = (int) histoDimData[1];
//        const float xMin    = histoDimData[2];
//        const float xMax    = histoDimData[3];
        const int   entries = (int) histoDimData[4];

        // fill histogram
        int offsetData  = 5;
        int offsetError = 5+nBins+1;
        // N.B. bin 0: underflow, bin nBins+1 overflow
        rootHistogram->SetBinContent(0, (float) histoDimData[5]);
        rootHistogram->SetBinContent(nBins+1,
                                     (float) histoDimData[5 + nBins +
                                                             1]);
        for (int i = 1; i <= nBins; ++i) {
          rootHistogram->SetBinContent(i,
                                       (float) histoDimData[offsetData +
                                                               i]);
          rootHistogram->SetBinError(i, (float) histoDimData[
                                     offsetError + i]);
        }
        rootHistogram->SetEntries(entries);
      } else if (s_H2D == m_histogramType) {        
        const int   nBinsX   = (int) histoDimData[1];
//        const float xMin     = histoDimData[2];
//        const float xMax     = histoDimData[3];
        const int   nBinsY   = (int) histoDimData[4];
//        const float yMin     = histoDimData[5];
//        const float yMax     = histoDimData[6];
        const float entries  = (float) histoDimData[7];

        int   iData = 8;  //current position in stream
        float data  = 0;
        for (int i=0; i<= nBinsX+1; ++i) {
          for (int j=0; j <= nBinsY+1; ++j) {
            data = (float) histoDimData[iData];
            rootHistogram->SetBinContent(i, j, (float) data);
            iData ++;
          }
        }

        for (int i=0; i<= nBinsX+1; ++i) {
          for (int j=0; j <= nBinsY+1; ++j) {
            data = (float) histoDimData[iData];
            rootHistogram->SetBinError(i, j, (float) data);
            iData ++;
          }
        }

        rootHistogram->SetEntries(entries);

      } else if (s_P1D == m_histogramType || s_HPD == m_histogramType) {
        const int   nBins   = (int) histoDimData[1];
//        const float xMin    = histoDimData[2];
//        const float xMax    = histoDimData[3];
        const int   entries = (int) histoDimData[4];
        float *entriesPerBin;
        float *sumWTPerBin;
        float *sumWT2PerBin;

        const int offsetEntries = 5;
        const int offsetWT      = 5 + nBins+2;
        const int offsetWT2     = 5 + nBins+2 + nBins+2;
      
        entriesPerBin = &histoDimData[offsetEntries];
        sumWTPerBin  = &histoDimData[offsetWT];
        sumWT2PerBin  = &histoDimData[offsetWT2];

        float yvalue = 0; 
        float yerr = 0;
        // bin 0: underflow, nBins+1 overflow ?
        for (int i = 0; i <= nBins+2; i++) {
          yvalue = 0;    
          if (entriesPerBin[i] > 0)
            yvalue = sumWTPerBin[i]/entriesPerBin[i];
          // mean in Y                
          rootHistogram->SetBinContent(i, yvalue);    
      
          yerr = 0;
          if (entriesPerBin[i] > 0)
            yerr = TMath::Sqrt(sumWT2PerBin[i]/entriesPerBin[i]-yvalue*yvalue);
          // RMS = sqrt(E[x**2]-E[x]**2)
          rootHistogram->SetBinError(i, yerr);
          
        }
        rootHistogram->SetEntries(entries);
      }
    }
      }
//    m_toRefresh = false;
   } else if (s_CNT == m_histogramType) {

// This will only work with a callback: reading on the DIM buffer gives stale values
// // HLTA0101_Adder_1/GauchoJob/MonitorSvc/monRate/Counternumber1 (D:2;C)
    
    boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
    double dimContent = 0;
     if (rootLock && m_dimInfo) {
       // wait until data has arrived
       int m_serviceSize = m_dimInfo->getSize()/sizeof(dimContent);
       while (m_serviceSize <= 0) {
         gSystem->Sleep(m_waitTime);
         m_serviceSize = m_dimInfo->getSize()/sizeof(dimContent);
       }
        if (m_isEFF && 
            (-1.0 != *(float*) m_dimInfo->getData()) &&
            rootHistogram && !m_isEmptyHisto) {
          double* histoDimData;
          histoDimData = (double*) m_dimInfo->getData();
          dimContent   = (double) histoDimData[0];
        } else if (-1.0 != m_dimInfo->getDouble() &&
                   rootHistogram && !m_isEmptyHisto) {
          dimContent = m_dimInfo->getDouble();
        }
       rootHistogram->SetBinContent(m_trendBin, dimContent);
       m_trendBin++;
     }
   } else if (s_pfixMonProfile == m_histogramType || s_pfixMonH1D == m_histogramType
                || s_pfixMonH2D == m_histogramType) {
     if (m_dimInfoMonObject && m_dimInfoMonObject->loadMonObject()) {

        if (s_pfixMonH1D == m_histogramType){
          MonH1D* monTH1D = (MonH1D*) m_dimInfoMonObject->monObject();
          if (monTH1D) { monTH1D->loadObject(); }
        }
        if (s_pfixMonH2D == m_histogramType){
          MonH2D* monTH2D = (MonH2D*) m_dimInfoMonObject->monObject();
          if (monTH2D) { monTH2D->loadObject(); }
        }
        if (s_pfixMonProfile == m_histogramType){
          MonProfile* monProfile = (MonProfile*) m_dimInfoMonObject->monObject();
          if (monProfile) { monProfile->loadObject(); }
        }
//        if (hostingPad) hostingPad->Modified();
      }
    }
    if (m_cleared && m_offsetHistogram) {
//        rootHistogram->Add(m_offsetHistogram,-1.0); - does not reset errors
        if (s_H1D == m_histogramType ||
            s_P1D == m_histogramType || s_HPD == m_histogramType ||
            s_pfixMonProfile == m_histogramType ||
            s_pfixMonH1D == m_histogramType) {
          for (int i = 1; i <= rootHistogram->GetNbinsX(); ++i) {
            rootHistogram->SetBinContent(i, rootHistogram->GetBinContent(i)
              - m_offsetHistogram->GetBinContent(i));
            rootHistogram->SetBinError(i, sqrt(pow(rootHistogram->
              GetBinError(i), 2) - pow(m_offsetHistogram->
              GetBinError(i), 2)));
          }
        } else if (s_H2D == m_histogramType ||
                   s_pfixMonH2D == m_histogramType) {
          for (int i=1; i<= rootHistogram->GetNbinsX() ; ++i) {
            for (int j=1; j <= rootHistogram->GetNbinsY() ; ++j) {
              rootHistogram->SetBinContent(i, j,
                rootHistogram->GetBinContent(i, j) -
                m_offsetHistogram->GetBinContent(i, j));
              rootHistogram->SetBinError(i, j, sqrt(pow(rootHistogram->
                GetBinError(i, j), 2) -
                pow(m_offsetHistogram->GetBinError(i, j), 2)));
            }
          }
        }
      }
      if (s_pfixMonH2D == m_histogramType) { //  || s_CNT == m_histogramType
        if ((TH2D::Class() == rootHistogram->IsA()) &&
            m_histogramImage) {
          m_histogramImage->SetImage((const Double_t *)((TH2D*)rootHistogram)->GetArray(), ((TH2D*)rootHistogram)->GetNbinsX() + 2,
                      ((TH2D*)rootHistogram)->GetNbinsY() + 2, m_prettyPalette); //  gWebImagePalette, gHistImagePalette
        }
      }      
      if (hostingPad) { hostingPad->Modified(); }  
  
} else if (m_isAnaHist && m_anaSources.size()>0)  {
    std::vector<TH1*> sources(m_anaSources.size());
    bool sourcesOk = true;
    for (unsigned int i=0; i< m_anaSources.size(); ++i) {
      m_anaSources[i]->fillHistogram();
      sources[i]= m_anaSources[i]->rootHistogram;
      if (m_anaSources[i]->isEmptyHisto() ) 
        sourcesOk = false;
    }
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
      (m_analysisLib->getAlg(m_creationAlgorithm));
    if (creator && sourcesOk) {
      std::string htitle(onlineHistogram()->htitle());
      rootHistogram = creator->exec(&sources, &m_parameters,  m_identifier,
                                    htitle,
                                    isEmptyHisto() ? NULL : rootHistogram);
      beRegularHisto();
    }
    if (hostingPad) { hostingPad->Modified(); }
  }
}
bool DbRootHist::setOnlineHistogram(OnlineHistogram* newOnlineHistogram)
{
  bool out = false;
  bool isInit = (NULL == m_onlineHistogram);
  if (newOnlineHistogram) {
    if (newOnlineHistogram->identifier() == m_identifier &&
        false == newOnlineHistogram->isAbort()) {
      m_onlineHistogram = newOnlineHistogram;
      if ( newOnlineHistogram->isAnaHist() ) {
        m_histogramType = newOnlineHistogram->hstype();
      }
      if (rootHistogram && isInit) { setTH1FromDB(); }
      out = true;
    } else {
      newOnlineHistogram->warningMessage("provided OnlineHistogram object is not compatible");
    }
  }

  loadAnaSources();
  return out;
}
bool DbRootHist::setRootHistogram(TH1* newRootHistogram)
{
  bool out = false;
  bool isInit = (NULL == rootHistogram);
  if (newRootHistogram) {
    rootHistogram = newRootHistogram;
    if (m_onlineHistogram && isInit) { setTH1FromDB(); }
    out = true;
  }
  return out;
}
bool DbRootHist::connectToDB(OnlineHistDB* onlineHistDbSession,
                             std::string page,
                             int instanceOnPage)
{
  bool out;
  if (onlineHistDbSession) { m_session = onlineHistDbSession; }
  if (m_session) {
    out = setOnlineHistogram(m_session->getHistogram(m_identifier, page,
                                                     instanceOnPage));
  } else { out = false; }
  return out;
}
void DbRootHist::setTH1FromDB()
{
  if (m_historyTrendPlotMode) return;
  boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  if (oraLock && m_onlineHistogram && rootHistogram) {
    //m_onlineHistogram->dump(); //DEBUG
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt;
    if (m_onlineHistogram->getDisplayOption("LABEL_X", &sopt)) {
      rootHistogram->SetXTitle (sopt.data());
    }
    if (m_onlineHistogram->getDisplayOption("LABEL_Y", &sopt)) {
      rootHistogram->SetYTitle (sopt.data());
    }
    if (m_onlineHistogram->getDisplayOption("LABEL_Z", &sopt)) {
      rootHistogram->SetZTitle (sopt.data());
    }
    float bxmin=rootHistogram->GetXaxis()->GetXmin();
    float bxmax=rootHistogram->GetXaxis()->GetXmax();
    if (m_onlineHistogram->getDisplayOption("XMIN", &fopt)) { bxmin=fopt; }
    if (m_onlineHistogram->getDisplayOption("XMAX", &fopt)) { bxmax=fopt; }
    rootHistogram->GetXaxis()->SetRangeUser(bxmin,bxmax);

    if (m_onlineHistogram->dimension() <2) { // 1d histograms
      if (m_onlineHistogram->getDisplayOption("YMIN", &fopt)) {
        rootHistogram->SetMinimum(fopt);
      }
      if (m_onlineHistogram->getDisplayOption("YMAX", &fopt)) {
        rootHistogram->SetMaximum(fopt);
      }
    } else {  // 2d histograms
      float bymin = rootHistogram->GetYaxis()->GetXmin();
      float bymax=rootHistogram->GetYaxis()->GetXmax();
      if (m_onlineHistogram->getDisplayOption("YMIN", &fopt)) { bymin=fopt; }
      if (m_onlineHistogram->getDisplayOption("YMAX", &fopt)) { bymax=fopt; }
      rootHistogram->GetYaxis()->SetRangeUser(bymin, bymax);
      if (m_onlineHistogram->getDisplayOption("ZMIN", &fopt)) {
        rootHistogram->SetMinimum(fopt);
      }
      if (m_onlineHistogram->getDisplayOption("ZMAX", &fopt)) {
        rootHistogram->SetMaximum(fopt);
      }
    }
    if (m_onlineHistogram->getDisplayOption("STATS", &iopt)) {
      //gStyle->SetOptStat(iopt);
//std::cout << "optstat for " << rootHistogram->GetName() << " " << ((0 != iopt)? true : false) << std::endl;      
      rootHistogram->SetStats(0 != iopt);
    }
        
    if (m_onlineHistogram->getDisplayOption("REF", &sopt)) {
      m_refOption = sopt;      
    }
    if (m_onlineHistogram->getDisplayOption("FILLSTYLE", &iopt)) {
      rootHistogram->SetFillStyle(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("FILLCOLOR", &iopt)) {
      rootHistogram->SetFillColor(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("LINESTYLE", &iopt)) {
      rootHistogram->SetLineStyle(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("LINECOLOR", &iopt)) {
      rootHistogram->SetLineColor(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("LINEWIDTH", &iopt)) {
      rootHistogram->SetLineWidth(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARKERSIZE", &iopt)) {
      rootHistogram->SetMarkerSize(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARKERSTYLE", &iopt)) {
      rootHistogram->SetMarkerStyle(iopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARKERCOLOR", &iopt)) {
      rootHistogram->SetMarkerColor(iopt);
    }

    if (m_onlineHistogram->getDisplayOption("TIT_X_SIZE", &fopt)) {
      rootHistogram->GetXaxis()->SetTitleSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("TIT_X_OFFS", &fopt)) {
      rootHistogram->GetXaxis()->SetTitleOffset(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_X_SIZE", &fopt)) {
      rootHistogram->GetXaxis()->SetLabelSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_X_OFFS", &fopt)) {
      rootHistogram->GetXaxis()->SetLabelOffset(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("NDIVX", &iopt)) {
      rootHistogram->SetNdivisions(iopt,"X");
    }

    if (m_onlineHistogram->getDisplayOption("TIT_Y_SIZE", &fopt)) {
      rootHistogram->GetYaxis()->SetTitleSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("TIT_Y_OFFS", &fopt)) {
      rootHistogram->GetYaxis()->SetTitleOffset(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_Y_SIZE", &fopt)) {
      rootHistogram->GetYaxis()->SetLabelSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_Y_OFFS", &fopt)) {
      rootHistogram->GetYaxis()->SetLabelOffset(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("NDIVY", &iopt)) {
      rootHistogram->SetNdivisions(iopt,"Y");
    }

    if (m_onlineHistogram->getDisplayOption("TIT_Z_SIZE", &fopt)) {
      rootHistogram->GetZaxis()->SetTitleSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("TIT_Z_OFFS", &fopt)) {
      rootHistogram->GetZaxis()->SetTitleOffset(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_Z_SIZE", &fopt)) {
      rootHistogram->GetZaxis()->SetLabelSize(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("LAB_Z_OFFS", &fopt)) {
      rootHistogram->GetZaxis()->SetLabelOffset(fopt);
    }
    
    if (m_onlineHistogram->getDisplayOption("NORM", &fopt)) {
      if (fopt>0.)
        rootHistogram->SetNormFactor(fopt);
    }

    // custom bin labels
    if (0 != rootHistogram->GetXaxis()) {
      if (m_onlineHistogram->nXbinlabels() > 0) {
        for (unsigned int il = 0; il < m_onlineHistogram->nXbinlabels(); il++) {
          sopt = m_onlineHistogram->binlabel(il,0);
          rootHistogram->GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
    if (0 != rootHistogram->GetYaxis()) {
      if (m_onlineHistogram->nYbinlabels() > 0) {
        for (unsigned int il = 0; il < m_onlineHistogram->nYbinlabels(); il++) {
          sopt = m_onlineHistogram->binlabel(il,1);
          rootHistogram->GetYaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
  }
}

void DbRootHist::setPadMarginsFromDB(TPad* &pad) {
  if (NULL != &pad &&
      NULL != m_onlineHistogram) {
    float fopt = 0.0;
    if (m_onlineHistogram->getDisplayOption("MARGIN_TOP", &fopt)) {
      pad->SetTopMargin(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARGIN_BOTTOM", &fopt)) {
      pad->SetBottomMargin(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARGIN_LEFT", &fopt)) {
      pad->SetLeftMargin(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("MARGIN_RIGHT", &fopt)) {
      pad->SetRightMargin(fopt);
    }
  }
}


// TODO: should rather use hostingPad
void DbRootHist::setDrawOptionsFromDB(TPad* &pad)
{
//  int curStat = gStyle->GetOptStat();
//  gStyle->SetOptStat(0);

  if (m_onlineHistogram && rootHistogram) {
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt("");

    // TPaveStats is obtained after a pad->Draw(), but note that changing OptStat
    // doesn't resize the Pave.. thus it's better to set the global stat options also 
    // before drawing
    
    TPaveStats* stats =  (TPaveStats*)rootHistogram->GetListOfFunctions()->FindObject("stats");

    if (m_historyTrendPlotMode) { // special settings for trend mode
      rootHistogram->SetDrawOption("E1");
      rootHistogram->SetStats(0);
      if (stats) stats->Delete();
      pad->SetLogx(0);
      pad->SetLogy(0);
      rootHistogram->SetXTitle ("Saveset");
      std::string ylab="Average";
      if (m_onlineHistogram->getDisplayOption("LABEL_X", &sopt)) {
        ylab = ylab + " (" + sopt + ")";
      }
      rootHistogram->SetYTitle (ylab.data());
    }
    else { // normal case
      m_onlineHistogram->getDisplayOption("STATS", &iopt);
      if (0 != iopt) {
        if (stats) {
          stats->SetOptStat(iopt);
          double x1=stats->GetX1NDC();
          double x2=stats->GetX2NDC();
          double y1=stats->GetY1NDC();
          double y2=stats->GetY2NDC();
          if (m_onlineHistogram->getDisplayOption("STAT_X_OFFS", &fopt)) {
            x1 = fopt;
          }
          if (m_onlineHistogram->getDisplayOption("STAT_X_SIZE", &fopt)) {
            x2 = x1 + fopt;
          }
          if (m_onlineHistogram->getDisplayOption("STAT_Y_OFFS", &fopt)) {
            y1 = fopt;
          }
          if (m_onlineHistogram->getDisplayOption("STAT_Y_SIZE", &fopt)) {
            y2 = y1 + fopt;
          }
          stats->SetX1NDC(x1);
          stats->SetX2NDC(x2);
          stats->SetY1NDC(y1);
          stats->SetY2NDC(y2);
          // save it to check if was changed at saving time
          if (m_statpave) { delete m_statpave; m_statpave = 0; }
          m_statpave = (TPave*)stats->Clone(); // memleak?
        }
      } else {
        rootHistogram->SetStats(false);
      }
      // title pave
      TPaveText* titpave = (TPaveText*) pad->GetPrimitive("title");
      if (titpave) {
        double x1=titpave->GetX1NDC();
        double x2=titpave->GetX2NDC();
        double y1=titpave->GetY1NDC();
        double y2=titpave->GetY2NDC();
        if (m_onlineHistogram->getDisplayOption("HTIT_X_OFFS", &fopt)) {
          x1 = fopt;
        }
        if (m_onlineHistogram->getDisplayOption("HTIT_X_SIZE", &fopt)) {
          x2 = x1 + fopt;
        }
        if (m_onlineHistogram->getDisplayOption("HTIT_Y_OFFS", &fopt)) {
          y1 = fopt;
        }
        if (m_onlineHistogram->getDisplayOption("HTIT_Y_SIZE", &fopt)) {
          y2 = y1 + fopt;
        }
        titpave->SetX1NDC(x1);
        titpave->SetX2NDC(x2);
        titpave->SetY1NDC(y1);
        titpave->SetY2NDC(y2);
        // save it to check if was changed at saving time
        if (m_titpave) { delete m_titpave; m_titpave = 0; }
        m_titpave = (TPave*)titpave->Clone();
      }
      
      if (m_onlineHistogram->getDisplayOption("DRAWOPTS", &sopt) ) {
        if(m_isOverlap && sopt.find("SAME") == std::string::npos ) sopt += "SAME";
        rootHistogram->SetDrawOption(sopt.c_str());
      }
      if (m_onlineHistogram->getDisplayOption("LOGX", &iopt)) {
        pad->SetLogx(1);
      }
      if (m_onlineHistogram->getDisplayOption("LOGY", &iopt)) {
        if(rootHistogram->GetEntries()>0) pad->SetLogy(1);
      }
      int gridx = gStyle->GetPadGridX();
      int gridy = gStyle->GetPadGridY();
      if (m_onlineHistogram->getDisplayOption("GRIDX", &iopt)) { gridx=iopt; }
      if (m_onlineHistogram->getDisplayOption("GRIDY", &iopt)) { gridy=iopt; }
      pad->SetGrid(gridx, gridy);
      if (rootHistogram->GetDimension() > 1) {
        if (m_onlineHistogram->getDisplayOption("LOGZ", &iopt)) {
          pad->SetLogz(1);
        }
        if (m_onlineHistogram->getDisplayOption("THETA", &fopt)) {
          pad->SetTheta(fopt);
        }
        if (m_onlineHistogram->getDisplayOption("PHI", &fopt)) {
          pad->SetPhi(fopt);
        }
      }
      if (m_onlineHistogram->hasFitFunction()) {
        gStyle->SetOptFit(1111111);
        std::string Name;
        std::vector<float> initValues;
        m_onlineHistogram->getFitFunction(Name,&initValues);
        m_analysisLib->getFitFunction(Name)->fit(rootHistogram, &initValues);
      }
      
      if(m_onlineHistogram->getDisplayOption("TICK_X", &iopt)) {
        pad->SetTickx(iopt);
      }
      if(m_onlineHistogram->getDisplayOption("TICK_Y", &iopt)) {
        pad->SetTicky(iopt);
      }
      if(m_onlineHistogram->getDisplayOption("PADCOLOR", &iopt)) {
        pad->SetFillColor(iopt);
      }
    }
  }
//  gStyle->SetOptStat(curStat);
}
bool DbRootHist::updateDBOption(std::string opt, void* value, bool isDefault)
{
  // update option to current value only  if:
  //   is already set and the value has changed
  //   is not set and value is different from default
  // return true if option is updated in the DB
  bool update = false;
  bool out = false;
  if (m_onlineHistogram && rootHistogram) {
    if (m_onlineHistogram->isSetDisplayOption(opt)) {
      update = m_onlineHistogram->changedDisplayOption(opt, value);
    } else {
      if (!isDefault) { update=true; }
    }
    if (update) { out = m_onlineHistogram->setDisplayOption(opt, value); }
  }
  return out;
}
// TODO: should rather use hostingPad
bool DbRootHist::saveTH1ToDB(TPad* pad)
{
  bool out = false;
  if (m_onlineHistogram && rootHistogram) {
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt("");

    sopt = rootHistogram->GetXaxis()->GetTitle();
    out |= updateDBOption("LABEL_X", &sopt, sopt.empty());

    sopt = rootHistogram->GetYaxis()->GetTitle();
    out |= updateDBOption("LABEL_Y", &sopt, sopt.empty());
    // note: axis minima and maxima should not be set in this way, but
    // through the web interface

    TPaveStats* stats = (TPaveStats*)rootHistogram->GetListOfFunctions()
                                                      ->FindObject("stats");
    // if histogram has not been plotted (or has been plotted without stats),
    // do nothing
    if (stats && m_statpave) {
      iopt = (int) stats->GetOptStat();
      // 111110 seems to be hardcoded in root
      out |= updateDBOption("STATS", &iopt, iopt == 111110 );
      fopt = stats->GetX1NDC();
      out |= updateDBOption("STAT_X_OFFS", &fopt, 
                            TMath::Abs(fopt - m_statpave->GetX1NDC())<0.001);
      fopt = stats->GetX2NDC() - stats->GetX1NDC();
      out |= updateDBOption("STAT_X_SIZE", &fopt, 
                            TMath::Abs(fopt - (m_statpave->GetX2NDC()-m_statpave->GetX1NDC())) <0.001);
      fopt = stats->GetY1NDC();
      out |= updateDBOption("STAT_Y_OFFS", &fopt, 
                            TMath::Abs(fopt - m_statpave->GetY1NDC())<0.001);
      fopt = stats->GetY2NDC() - stats->GetY1NDC();
      out |= updateDBOption("STAT_Y_SIZE", &fopt, 
                            TMath::Abs(fopt - (m_statpave->GetY2NDC()-m_statpave->GetY1NDC())) <0.001);
      delete m_statpave;
      m_statpave = (TPave*)stats->Clone();
    }
    // now title options
    TPaveText* tit = (TPaveText*)pad->GetPrimitive("title");
    if(tit && m_titpave) {
      fopt = tit->GetX1NDC();
      out |= updateDBOption("HTIT_X_OFFS", &fopt, 
                            TMath::Abs(fopt - m_titpave->GetX1NDC())<0.0001);
      fopt = tit->GetX2NDC() - tit->GetX1NDC();
      out |= updateDBOption("HTIT_X_SIZE", &fopt, 
                            TMath::Abs(fopt - (m_titpave->GetX2NDC()-m_titpave->GetX1NDC())) <0.0001);
      fopt = tit->GetY1NDC();
      out |= updateDBOption("HTIT_Y_OFFS", &fopt, 
                            TMath::Abs(fopt - m_titpave->GetY1NDC())<0.001);
      fopt = tit->GetY2NDC() - tit->GetY1NDC();
      out |= updateDBOption("HTIT_Y_SIZE", &fopt, 
                            TMath::Abs(fopt - (m_titpave->GetY2NDC()-m_titpave->GetY1NDC())) <0.0001);
      delete m_titpave;
      m_titpave = (TPave*) tit->Clone();
    }
    

    iopt = (int) rootHistogram->GetFillStyle();
    out |= updateDBOption("FILLSTYLE", &iopt,
                          iopt == (int) gStyle->GetHistFillStyle());
    iopt = (int) rootHistogram->GetFillColor();
    out |= updateDBOption("FILLCOLOR", &iopt,
                          iopt == (int) gStyle->GetHistFillColor());
    iopt = (int) rootHistogram->GetLineStyle();
    out |= updateDBOption("LINESTYLE", &iopt,
                          iopt == (int) gStyle->GetHistLineStyle());
    iopt = (int) rootHistogram->GetLineColor();
    out |= updateDBOption("LINECOLOR", &iopt,
                          iopt == (int) gStyle->GetHistLineColor());
    iopt = (int) rootHistogram->GetLineWidth();
    out |= updateDBOption("LINEWIDTH", &iopt,
                          iopt == (int) gStyle->GetHistLineWidth());
    iopt = (int) rootHistogram->GetMarkerStyle();
    out |= updateDBOption("MARKERSTYLE", &iopt,
                          iopt == (int) gStyle->GetMarkerStyle());
    iopt = (int) rootHistogram->GetMarkerSize();
    out |= updateDBOption("MARKERSIZE", &iopt,
                          iopt == (int) gStyle->GetMarkerSize());
    iopt = (int) rootHistogram->GetMarkerColor();
    out |= updateDBOption("MARKERCOLOR", &iopt,
                          iopt == (int) gStyle->GetMarkerColor());

    sopt = rootHistogram->GetDrawOption();
    if (sopt.empty()) { sopt = rootHistogram->GetOption(); }
    out |= updateDBOption("DRAWOPTS",  &sopt, sopt.empty());

    fopt = rootHistogram->GetXaxis()->GetTitleSize();
    out |= updateDBOption("TIT_X_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleSize("X"))<0.0001);
    fopt = rootHistogram->GetXaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_X_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleOffset("X"))<0.0001);
    fopt = rootHistogram->GetXaxis()->GetLabelSize();
    out |= updateDBOption("LAB_X_SIZE", &fopt,
         TMath::Abs(fopt - (float) gStyle->GetLabelSize("X"))<0.0001);
    fopt = rootHistogram->GetXaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_X_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelOffset("X"))<0.0001);
    iopt = rootHistogram->GetNdivisions("X");
    out |= updateDBOption("NDIVX", &iopt,
                          iopt == (int) gStyle->GetNdivisions("X"));

    fopt = rootHistogram->GetYaxis()->GetTitleSize();
    out |= updateDBOption("TIT_Y_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleSize("Y"))<0.0001);
    fopt = rootHistogram->GetYaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_Y_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Y"))<0.0001);
    fopt = rootHistogram->GetYaxis()->GetLabelSize();
    out |= updateDBOption("LAB_Y_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelSize("Y"))<0.0001);
    fopt = rootHistogram->GetYaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_Y_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelOffset("Y"))<0.0001);
    iopt = rootHistogram->GetNdivisions("Y");
    out |= updateDBOption("NDIVY", &iopt,
                          iopt == (int) gStyle->GetNdivisions("Y"));

    if (m_onlineHistogram->dimension() > 1) {
      sopt = rootHistogram->GetZaxis()->GetTitle();
      out |= updateDBOption("LABEL_Z", &sopt, sopt.empty());
      fopt = rootHistogram->GetZaxis()->GetTitleSize();
      out |= updateDBOption("TIT_Z_SIZE",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetTitleSize("Z"))<0.0001);
      fopt = rootHistogram->GetZaxis()->GetTitleOffset();
      out |= updateDBOption("TIT_Z_OFFS",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Z"))<0.0001);
      fopt = rootHistogram->GetZaxis()->GetLabelSize();
      out |= updateDBOption("LAB_Z_SIZE",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetLabelSize("Z"))<0.0001);
      fopt = rootHistogram->GetZaxis()->GetLabelOffset();
      out |= updateDBOption("LAB_Z_OFFS",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetLabelOffset("Z"))<0.0001);
    }

    // now options from Pad ... be sure we are in the right Pad
   if (pad) {
     iopt = pad->GetLogx();
     out |= updateDBOption("LOGX", &iopt, iopt == gStyle->GetOptLogx());
     iopt = pad->GetLogy();
     out |= updateDBOption("LOGY", &iopt, iopt == gStyle->GetOptLogy());
     iopt = pad->GetGridx() ? 1 : 0;
     out |= updateDBOption("GRIDX", &iopt, (iopt>0) == gStyle->GetPadGridX());
     iopt = pad->GetGridy() ? 1 : 0;
     out |= updateDBOption("GRIDY", &iopt, (iopt>0) == gStyle->GetPadGridY());
     iopt = pad->GetTickx();
     out |= updateDBOption("TICK_X", &iopt, iopt == gStyle->GetPadTickX());
     iopt = pad->GetTicky();
     out |= updateDBOption("TICK_Y", &iopt, iopt == gStyle->GetPadTickY());
     fopt = (float)pad->GetTopMargin();
     out |= updateDBOption("MARGIN_TOP", &fopt, 
                           TMath::Abs(fopt - (float)gStyle->GetPadTopMargin())<0.0001);
     fopt = (float)pad->GetBottomMargin();
     out |= updateDBOption("MARGIN_BOTTOM", &fopt, 
                           TMath::Abs(fopt - (float)gStyle->GetPadBottomMargin())<0.0001);
     fopt = (float)pad->GetLeftMargin();
     out |= updateDBOption("MARGIN_LEFT", &fopt, 
                           TMath::Abs(fopt - (float)gStyle->GetPadLeftMargin())<0.0001);
     fopt = (float)pad->GetRightMargin();
     out |= updateDBOption("MARGIN_RIGHT", &fopt, 
                           TMath::Abs(fopt - (float)gStyle->GetPadRightMargin())<0.0001);
     iopt = pad->GetFillColor();
     out |= updateDBOption("PADCOLOR", &iopt, iopt == 10 ); // 10 corresponds to GetWhitePixel()
                           //(int)gStyle->GetPadColor() );

     if (m_onlineHistogram->dimension() > 1) {
       iopt = pad->GetLogz();
       out |= updateDBOption("LOGZ", &iopt, iopt == gStyle->GetOptLogz());
       fopt = (float)pad->GetTheta();
        // seems to be hardcoded in root:
       out |= updateDBOption("THETA", &fopt, (int)fopt == 30);
       fopt = (float)pad->GetPhi();
       // seems to be hardcoded in root
       out |= updateDBOption("PHI", &fopt, (int)fopt == 30);
     }
   }
   if (out) {
     out = m_onlineHistogram->saveDisplayOptions();
   }
  }
  return out;
}


void DbRootHist::draw(TCanvas* editorCanvas, double xlow, double ylow, double xup, double yup, bool fastHitMapDraw, TPad* overlayOnPad)
{
  m_fastHitmapPlot = fastHitMapDraw;
  TPad* pad=overlayOnPad;
  m_isOverlap = (NULL != overlayOnPad);
  // if (NULL == m_session) {  
  if (!m_isOverlap) {
    pad = new TPad(m_identifier.c_str(),
                   TString(""),
                   TMath::Abs(xlow), TMath::Abs(ylow),
                   TMath::Abs(xup), TMath::Abs(yup));
    setPadMarginsFromDB(pad);
  }

  editorCanvas->cd();

  if (!m_isOverlap) { (pad)->Draw(); }
  pad->cd();

  if (!m_isOverlap) {
    pad->SetBit(kNoContextMenu);
    pad->SetFillColor(10);
  }
  
//  int curStat = gStyle->GetOptStat();
//  gStyle->SetOptStat(0);
  if (0 != rootHistogram) {
    if (TH2D::Class() == rootHistogram->IsA() &&
         m_fastHitmapPlot) {
//      if (m_histogramImage) {
//        delete m_histogramImage; m_histogramImage = NULL;
//      } else
      if (NULL == m_histogramImage) {      
        m_histogramImage = TImage::Create();
        m_histogramImage->SetConstRatio(false);
      }
      m_histogramImage->SetImage((const Double_t *)((TH2D*)rootHistogram)->GetArray(), ((TH2D*)rootHistogram)->GetNbinsX() + 2,
                  ((TH2D*)rootHistogram)->GetNbinsY() + 2, m_prettyPalette); // gWebImagePalette
                  //gHistImagePalette
      m_histogramImage->Draw();
//      m_histogramImage->DrawText(m_textTitle, 0, 0);

      
//      pad->SetTitle(m_histoRootTitle);
    } else {
      std::string opt =  m_isOverlap ? "SAME" : "";
      rootHistogram->Draw(opt.c_str());      
    }
  }
//  gStyle->SetOptStat(curStat);
  setDrawOptionsFromDB(pad);
  if (!m_isOverlap) pad->SetName(m_histoRootName);
  hostingPad = pad;


 if (NULL != m_session) {
  std::string sopt("");
  boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
  if (rootLock && 0 != m_onlineHistogram  &&
      m_onlineHistogram->getDisplayOption("DRAWPATTERN", &sopt) &&
      false == m_isOverlap) {

    std::string drawPatternFile = m_analysisLib->refRoot() + "/" + 
                                  m_onlineHistogram->task() + "/" + sopt;
// std::cout << "drawPatternFile: "  << drawPatternFile.c_str() << std::endl;
    TFile rootFile(drawPatternFile.c_str()); 

//  secondPad->SetFillStyle(4000); //will be transparent
// or TImage::Merge(const TImage* , const char* = "alphablend", Int_t = 0, Int_t = 0) 4 pixmap

    if (rootFile.IsZombie()) {
        std::cout << "Error opening Root file" << std::endl;
    } else {
       TIter next1(rootFile.GetListOfKeys());
       TKey* key;    
       while ((key = (TKey*)next1())) {
         if (key->ReadObj()->InheritsFrom(TCanvas::Class())) {
       m_drawPattern = (TCanvas*)key->ReadObj();
         }
       }
       m_drawPattern->SetPad(TMath::Abs(xlow), TMath::Abs(ylow),
                      TMath::Abs(xup), TMath::Abs(yup));
       m_drawPattern->SetName(m_identifier.c_str());

   TPad *padsav = (TPad*)gPad;
//   gPad =   editorCanvas->cd();
   TObject *obj; //, *clone;

   dynamic_cast<TAttLine*>(m_drawPattern)->Copy((TAttLine&)*pad);
   dynamic_cast<TAttFill*>(m_drawPattern)->Copy((TAttFill&)*pad);
   dynamic_cast<TAttPad*>(m_drawPattern)->Copy((TAttPad&)*pad);

   TIter next(m_drawPattern->GetListOfPrimitives());
   while ((obj=next())) {
      pad->cd();
      if (TBox::Class() == obj->IsA() ||
          TLine::Class() == obj->IsA() ||
          TText::Class() == obj->IsA()) {
          obj->Draw();
//      clone = obj->Clone();
//      pad->GetListOfPrimitives()->Add(clone,next.GetOption());
      }
   }
   if (padsav) padsav->cd();
  }
  rootFile.Close();
 }


    }
}

void DbRootHist::normalizeReference()
{
  if (m_reference) {
    float normFactor = rootHistogram->GetNormFactor();
    // if GetNormFactor() >0, histogram is drawn normalized, just use the same normalization
    if (normFactor<0.1) {
      if (s_Entries == m_refOption) {
        normFactor = rootHistogram->GetSumOfWeights();
        // next lines needed only if m_offsetHistogram has not been subtracted yet
        //if (m_cleared && m_offsetHistogram) {
        //  normFactor -= m_offsetHistogram->GetSumOfWeights();
        //}
        m_reference->SetNormFactor(normFactor);
      } else if (s_Area == m_refOption) {
        normFactor = rootHistogram->Integral();
        //if (m_cleared && m_offsetHistogram) {
        //  normFactor -= m_offsetHistogram->Integral();
        //}
      }
    }
    m_reference->SetNormFactor(normFactor);
  }
}
void DbRootHist::referenceHistogram(ReferenceVisibility visibility)
{
  boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
  if (rootLock && rootHistogram &&
      s_H2D != m_histogramType &&
      s_pfixMonH2D != m_histogramType) {
    if (0 == m_reference &&
        s_NoReference != m_refOption &&
        rootHistogram->GetDimension() == 1 &&
        Show == visibility) {
      TH1* ref = (TH1*)m_analysisLib->getReference(m_onlineHistogram);
      if (ref) {
        if (m_reference) { delete m_reference; m_reference = 0; }
        m_reference = ref;
      }
    } 
    if (0 != m_reference &&
        Show == visibility) {
      // standard plot style
      m_reference->SetLineStyle(2);
      m_reference->SetLineColor(2); // red
      // normalization
      normalizeReference();
      TVirtualPad *padsav = gPad;
      hostingPad->cd();    
      m_reference->Draw("SAME");
      m_reference->SetStats(0);
      //    m_reference->Draw();
      hostingPad->Modified();
      padsav->cd();
    } else if (Hide == visibility) {
      if (0 != m_reference) { delete m_reference; m_reference = NULL; }
    }
  }
}

std::string DbRootHist::findDimServiceName(const std::string & dimServiceType) {
  char *dimService; 
  char *dimFormat;
  int dimType;
  std::string dimServiceName("");
  std::string dimServiceNameQueryBegining("");

  if (!m_partition.empty()) {
    dimServiceNameQueryBegining = dimServiceType + s_slash +
                                  m_partition +
                                  s_underscore + "*";
  } else if (!dimServiceType.empty()) {
    dimServiceNameQueryBegining = "*" + s_eff_monRate + "*";    
  } else {
    dimServiceNameQueryBegining = "*";    
  }
  
  
  boost::recursive_mutex::scoped_lock dimLock(*m_dimMutex);
  boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  if (dimLock && oraLock && m_onlineHistogram &&  m_session) {
    bool botherDimBrowser(true);
    std::string taskName(m_onlineHistogram->task());
//TODO: GauchoJob/Adder confusion    
    if (taskName == "GauchoJob") {taskName = "Adder";} 
    std::vector<std::string*>::iterator lookForTasksNotRunningIt;
    if (0 < m_tasksNotRunning->size()) {
      for (lookForTasksNotRunningIt = m_tasksNotRunning->begin();
           lookForTasksNotRunningIt != m_tasksNotRunning->end();
           ++lookForTasksNotRunningIt) {
        if (((**lookForTasksNotRunningIt) == taskName)) {
          botherDimBrowser = false;
        if (m_verbosity >= Verbose) {
          std::cout << "Skipping dead task: " << taskName << std::endl;
          }
          break;
        }
        }
    }
      if (botherDimBrowser) {
        int numberOfServices = m_dimBrowser->getServices(dimServiceNameQueryBegining.c_str());
        while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {
          std::string dimServiceCandidate(dimService);
          TString dimServiceTS(dimServiceCandidate);
           
          if (false == dimServiceTS.EndsWith(s_gauchocomment.c_str())) {
            HistogramIdentifier histogramCandidate = HistogramIdentifier(dimServiceCandidate);
//TODO: GauchoJob/Adder confusion            
            std::string identifier =  ("GauchoJob" == m_onlineHistogram->task()? "Adder" : m_onlineHistogram->task() ) + s_slash + m_onlineHistogram->algorithm() +
                    s_slash + m_onlineHistogram->hname();    
            if (0 == (histogramCandidate.histogramIdentifier().compare(identifier))) {          
              dimServiceName = dimServiceCandidate;
              break;
            } else {
          std::string* deadTaskName = new std::string(taskName); // memleak?
          m_tasksNotRunning->push_back(deadTaskName);
          if (m_verbosity >= Verbose) {
            std::cout << std::endl << "NOT found: " << dimService << std::endl;
            }
          }
        }
      }
      if (0 == numberOfServices) {
          std::string* deadTaskName = new std::string(taskName);
          m_tasksNotRunning->push_back(deadTaskName);
          if (m_verbosity >= Verbose) {
            std::cout << std::endl << "No partition for: " << dimService << std::endl;
          }
      }      
    }
  }
  return dimServiceName;
}

std::string DbRootHist::assembleCurrentDimServiceName() {
  
  std::string dimServiceName("");
  std::string dimServiceNameQueryBegining;

if (0 != m_onlineHistogram) {
  if (0 == m_onlineHistogram->hstype().compare(s_P1D) ||
      0 == m_onlineHistogram->hstype().compare(s_HPD)) {
    dimServiceName = findDimServiceName(s_pfixMonProfile);
    if (dimServiceName.empty()) {
      dimServiceName = findDimServiceName(s_HPD);
    }
    if (dimServiceName.empty()) {
      dimServiceName = findDimServiceName(s_P1D);
    }        
  } else if (0 == m_onlineHistogram->hstype().compare(s_H2D)) {
    dimServiceName = findDimServiceName(s_pfixMonH2D);
    if (dimServiceName.empty()) {
      dimServiceName = findDimServiceName(s_H2D);          
    }              
  } else if (0 == m_onlineHistogram->hstype().compare(s_H1D)) {
    dimServiceName = findDimServiceName(s_pfixMonH1D);
    if (dimServiceName.empty()) {
      dimServiceName = findDimServiceName(s_H1D);          
    }
  } else if (0 == m_onlineHistogram->hstype().compare(s_CNT)) {
    dimServiceName = findDimServiceName(std::string(""));
//    if (dimServiceName.empty()) {
//      dimServiceName = findDimServiceName(s_H1D);          
//    }
  }

  if (!dimServiceName.empty()){
    if (m_verbosity >= Verbose) {
      std::cout << std::endl << "assembled current Dim service name: "
                << dimServiceName << std::endl << std::endl;
    }  
   if (0 == m_onlineHistogram->hstype().compare(s_CNT)) {
    dimServiceName = m_dimServiceName.erase(0, s_CNT.length()+1);
   }
    setIdentifiersFromDim(dimServiceName);


    boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);  
    if (oraLock && m_onlineHistogram &&  m_session) {
      m_onlineHistogram->setDimServiceName(dimServiceName);
      m_onlineHistogram->checkServiceName();
      m_session->commit();
    }
  }
}
  return dimServiceName;
}
