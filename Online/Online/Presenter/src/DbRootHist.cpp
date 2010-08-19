// $Id: DbRootHist.cpp,v 1.181 2010-08-19 20:55:20 robbep Exp $
#include "DbRootHist.h"

// STL 
#include <iostream>
#include <cstdlib>
#include <cmath>

// ROOT
#ifdef WIN32
#  pragma warning( push )
#  pragma warning( disable : 4800 )
#endif
#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TEllipse.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TPaveStats.h>
#include <TCanvas.h>
#include <TKey.h>
#include <TImage.h>
#ifdef WIN32
#  pragma warning( pop )
#endif

// boost
#include <boost/thread/recursive_mutex.hpp>

// Online
#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2D.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OnlineHistDB/OnlineHistogram.h"
#include "OMAlib/OMAlib.h"

// local
#include "MonRateRace.h"
#include "presenter.h"
#include "PresenterInformation.h"

//============================================================================
// Constructor
//============================================================================
DbRootHist::DbRootHist(const std::string & identifier,
                       const std::string & dimServiceName,
                       int refreshTime, int instance,
                       OnlineHistDB* histogramDB,
                       OMAlib* analysisLib,
                       OnlineHistogram* onlineHist,
                       pres::MsgLevel verbosity,
                       DimBrowser* DimBr,
                       boost::recursive_mutex * oraMutex,
                       boost::recursive_mutex * dimMutex,
                       std::vector<std::string*> & tasksNotRunning,
                       boost::recursive_mutex * rootMutex)
  : HistogramIdentifier(dimServiceName),
    m_rootHistogram(NULL),
    m_identifier(identifier),
    m_hostingPad(NULL),
    m_presenterInfo( NULL ),
    m_drawPattern(NULL),
    m_gauchocommentDimInfo(NULL),
    m_offsetHistogram(NULL),
    m_trendTimeScale(pres::s_pageRefreshRate/1000),
    m_trendBin(1),
    m_isAnaHist(false),
    m_anaLoaded(false),
    m_analysisLib(analysisLib),
    m_creationAlgorithm(""),
    m_sourcenames(0),
    m_parameters(0),
    m_refreshTime(refreshTime),
    m_cleared(false),
    m_fastHitmapPlot(false),
    m_instance(instance),
    m_waitTime(20),
    m_session(histogramDB),
    m_onlineHistogram(NULL),
    m_isEmptyHisto(false),
    m_retryInit(2),
    m_refOption("AREA"),
    m_histogramImage(NULL),
    m_prettyPalette(NULL),
    m_reference(NULL),
    m_dimServiceName(dimServiceName),
    m_dimInfo(NULL),
    m_dimInfoMonObject(NULL),
    m_monRateRace(NULL),
    m_verbosity(verbosity),
    m_dimBrowser(DimBr),
    m_partition(dimServiceName),
    m_fitfunction(NULL),
    m_titpave(NULL),
    m_statpave(NULL),
    m_historyTrendPlotMode(false),
    m_isOverlap(false),
    m_oraMutex( oraMutex ),
    m_dimMutex( dimMutex ),
    m_tasksNotRunning(&tasksNotRunning),
    m_rootMutex( rootMutex ),
    m_effServiceType(pres::MonRate) {
  // Prevent ROOT booking
  TH1::AddDirectory(kFALSE);
  m_prettyPalette = new TImagePalette(1,0);

  TString dimMon(identifier);
  if (dimMon.BeginsWith(pres::s_adder.c_str()) &&
      dimMon.EndsWith(pres::s_eff_TCK.c_str())) {
    m_effServiceType = pres::TCKinfo;
    char *dimService; 
    char *dimFormat;
    int dimType;
    std::string query = dimServiceName + pres::s_underscore + pres::s_adder + pres::s_DimWildcard;

    boost::recursive_mutex::scoped_lock dimLock(*m_dimMutex);

    m_dimBrowser->getServices(query.c_str());
    while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {
      std::string dimServiceCandidate(dimService);
      TString dimServiceTS(dimServiceCandidate);
       
      if (true) { //eFFRegexp
        HistogramIdentifier histogramCandidate = HistogramIdentifier(dimServiceCandidate); 
        if (0 == (histogramCandidate.histogramIdentifier().compare(identifier))) {          
          m_dimServiceName = dimServiceCandidate;
          break;  
        }
      }
    }
  }
  
  // if dimBrowser is specified, retrieve dim service name from partition name
  if (m_dimBrowser && (!histogramDB) && (pres::TCKinfo != m_effServiceType)) {
    //    m_partition = dimServiceName;
    m_partition = partitionName();
  }
  
  if (histogramDB && (pres::TCKinfo != m_effServiceType)) {
    if (onlineHist) { setOnlineHistogram(onlineHist); }
    else { connectToDB(histogramDB, "_NONE_", m_instance); }
  }
}

//===============================================================================
// Destructor
//===============================================================================
DbRootHist::~DbRootHist() {
  if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = NULL;}
  if (m_rootHistogram) { delete m_rootHistogram; m_rootHistogram = NULL; }
  if (m_reference) { delete  m_reference; m_reference = NULL; }
  if (m_histogramImage) { delete m_histogramImage; m_histogramImage = NULL; }
  if (m_hostingPad && !m_isOverlap) { delete m_hostingPad; m_hostingPad = NULL; }
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = NULL; }
  if (m_dimInfoMonObject) { delete m_dimInfoMonObject; m_dimInfoMonObject = NULL; }
  if (m_monRateRace) { delete m_monRateRace; m_monRateRace = NULL; }
  if (m_fitfunction) { delete m_fitfunction; m_fitfunction = NULL; }
  if (m_titpave) {delete m_titpave; m_titpave = NULL; }
  if (m_statpave) {delete m_statpave; m_statpave = NULL; }
  if (m_prettyPalette) {delete m_prettyPalette; m_prettyPalette = NULL; }  
  m_oraMutex = 0 ;
  m_dimMutex = 0 ;
  m_rootMutex = 0 ; 
  cleanAnaSources();
}

//===============================================================================
// Clean analysis sources
//===============================================================================
void DbRootHist::cleanAnaSources() {
  for (unsigned int i=0; i< m_anaSources.size(); ++i) {
    delete m_anaSources[i];
    m_anaSources[i] = NULL;
  }
  m_anaSources.clear();
}

//===============================================================================
// Load analysis sources
//===============================================================================
void DbRootHist::loadAnaSources() {
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
        DbRootHist* anaHisto = new DbRootHist(m_sourcenames[i],
                                              m_dimServiceName,
                                              m_refreshTime,
                                              999,
                                              dbSession(),
                                              m_analysisLib,
                                              histo,
                                              m_verbosity,
                                              m_dimBrowser,
                                              m_oraMutex,
                                              m_dimMutex,
                                              *m_tasksNotRunning,
                                              m_rootMutex);
        m_anaSources.push_back(anaHisto);
        if (NULL == m_anaSources[i]->m_rootHistogram) { sourcesOK = false; }
      }
      m_anaLoaded = true;
      if ( !sourcesOK ) beEmptyHisto();
    }
  }
  if (m_reference) {
    delete m_reference;
    m_reference = NULL;
  }
}

//=============================================================================
// Enable histogram editing
//=============================================================================
void DbRootHist::enableEdit() {
  if (0 == m_hostingPad) return ;
  m_hostingPad->SetEditable(true);
  m_hostingPad->ResetBit(TPad::kCannotMove);
}

//=============================================================================
// Disable histogram editing
//=============================================================================
void DbRootHist::disableEdit() {
  if ( 0 == m_hostingPad ) return ;
  m_hostingPad->SetEditable(false);
  m_hostingPad->SetBit(TPad::kCannotMove);
}

//=============================================================================
// Enable histogram clearing
//=============================================================================
void DbRootHist::enableClear() {
  if ( 0 == m_rootHistogram) return ;

  if (!m_isAnaHist) {
    m_cleared = true;
    if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = 0;}
    if (pres::s_H1D == m_histogramType) {
      m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(m_rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (s_pfixMonH1D == m_histogramType) {
      m_offsetHistogram = new TH1D(*dynamic_cast<TH1D*>(m_rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (pres::s_H2D == m_histogramType) {
      m_offsetHistogram = new TH2F(*dynamic_cast<TH2F*>(m_rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (s_pfixMonH2D == m_histogramType) {
      m_offsetHistogram = new TH2D(*dynamic_cast<TH2D*>(m_rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (pres::s_P1D == m_histogramType ||
	       pres::s_HPD == m_histogramType) {
      m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(m_rootHistogram));        
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (s_pfixMonProfile == m_histogramType) {
      m_offsetHistogram = new TProfile(*dynamic_cast<TProfile*>(m_rootHistogram));        
      m_offsetHistogram->SetBit(kNoContextMenu);
    }
    if (m_offsetHistogram) { m_offsetHistogram->AddDirectory(kFALSE); }
  } else { // analysis histogram
    std::vector<DbRootHist*>::iterator hs;
    for (hs = m_anaSources.begin(); hs != m_anaSources.end(); ++hs) {
      (*hs)->enableClear();
    }
  }
}

//=============================================================================
// Initialize histogram 
//=============================================================================
void DbRootHist::initHistogram() {  
  if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = NULL;}
  if (m_rootHistogram) { delete m_rootHistogram; m_rootHistogram = NULL; }
  if (m_reference) { delete  m_reference; m_reference = NULL; }

  // If not AnaLib hist:
  if (!m_isAnaHist && (m_retryInit > 0) && m_dimBrowser) {
    // sed partition   
    if (m_onlineHistogram && m_session && (m_retryInit > 1)) {
      if (pres::TCKinfo != m_effServiceType) {
        m_dimServiceName = m_onlineHistogram->dimServiceName();
      }

      if (m_verbosity >= pres::Verbose) {
        std::cout << "dimServiceName from DB: " << m_dimServiceName << std::endl;
      }      

      HistogramIdentifier histogramIdentifier1(m_dimServiceName);
      std::string histType(histogramIdentifier1.histogramType());
      m_dimServiceName.erase(0, m_dimServiceName.find(pres::s_underscore));
      if (pres::s_CNT != histType) {
        m_dimServiceName = histType + pres::s_slash + m_partition + m_dimServiceName;
      } else {
        m_dimServiceName = m_partition + m_dimServiceName;
      }
      
      if (m_verbosity >= pres::Verbose) {
        std::cout << "dimServiceName w partition: " << m_dimServiceName << std::endl;
      }            
    }

    HistogramIdentifier histogramIdentifier(m_dimServiceName);
    m_histogramType = histogramIdentifier.histogramType();

    // _addFromDim?    
    if ( (m_histogramType.empty()) && (pres::TCKinfo == effServiceType())) {
      m_histogramType = pres::s_CNT;
    }

    if (pres::s_P1D == m_histogramType || pres::s_HPD == m_histogramType ||
        pres::s_H2D == m_histogramType || pres::s_H1D == m_histogramType ||
        pres::s_CNT == m_histogramType ) {
      if( m_dimServiceName.size() > 0 ) {
        if (m_dimInfo) { delete m_dimInfo; m_dimInfo = 0; }
        if ( (pres::s_CNT == m_histogramType) && (pres::TCKinfo != effServiceType())) {              
	  m_dimServiceName = findDimServiceName(pres::s_CNT);
        }
	m_dimInfo = new DimInfo(m_dimServiceName.c_str(), m_refreshTime, (float)-1.0);
      }       

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
        if ( -1.0 != m_dimInfo -> getFloat() ) {
          m_retryInit = 0;
          
          if ( m_verbosity >= pres::Verbose ) 
            std::cout << "Using dimServiceName from DB." << std::endl;
          
          histoDimData = (float*) m_dimInfo->getData();
          if (pres::s_H1D == m_histogramType) {
            const int   nBins   = (int) histoDimData[1];
            const float xMin   = (float) histoDimData[2];
            const float xMax   = (float) histoDimData[3];
            if ( 0 == m_rootHistogram ) {
              m_rootHistogram = new TH1F(m_histoRootName.Data(),
					 m_histoRootTitle.Data(),
					 nBins, xMin, xMax);
            }
          } else if (pres::s_H2D == m_histogramType) {
            const int   nBinsX   = (int) histoDimData[1];
            const float xMin    = (float) histoDimData[2];
            const float xMax    = (float) histoDimData[3];
            const int   nBinsY   = (int) histoDimData[4];
            const float yMin    = (float) histoDimData[5];
            const float yMax    = (float) histoDimData[6];
            if ( 0 == m_rootHistogram ) 
              m_rootHistogram = new TH2F(m_histoRootName.Data(),
					 m_histoRootTitle.Data(),
					 nBinsX, xMin, xMax,
					 nBinsY, yMin, yMax);
          } else if (pres::s_P1D == m_histogramType || pres::s_HPD == m_histogramType) {
            const int   nBins   = (int) histoDimData[1];
            const float xMin    = histoDimData[2];
            const float xMax    = histoDimData[3];
            if ( 0 == m_rootHistogram ) {
	      m_histoRootTitle = lastName();
              m_rootHistogram = new TH1F(m_histoRootName.Data(),
					 m_histoRootTitle.Data(),
					 nBins, xMin, xMax);
            }
          } else if ( pres::s_CNT == m_histogramType && ( 0 != m_presenterInfo ) ) {
            TString dimServiceTS(m_dimServiceName);
            
            // could be in MonRateRace?
            if (true == dimServiceTS.EndsWith(pres::s_eff_TCK.c_str())) {
              m_effServiceType = pres::TCKinfo;               
              m_monRateRace = new MonRateRace(m_dimServiceName, pres::TCKinfo);
            } else {
              m_effServiceType = pres::MonRate;
              m_monRateRace = new MonRateRace(m_dimServiceName, pres::MonRate);      
            }
            
            if ( ( pres::Online == m_presenterInfo->presenterMode() ) ||
		 ( pres::EditorOnline == m_presenterInfo->presenterMode() ) ) {
              m_trendTimeScale = 100 * m_trendTimeScale;
              if ( 0 == m_rootHistogram )
                m_rootHistogram = new TH1D(m_histoRootName.Data(),pres::s_eff_init.c_str(),
					   m_trendTimeScale, 0, m_trendTimeScale);
            } else if ( pres::Batch == m_presenterInfo->presenterMode() ) {
              gStyle->SetTimeOffset(m_offsetTime.Convert());
              if ( 0 == m_rootHistogram) 
                m_rootHistogram = new TH1D(m_histoRootName.Data(),m_histoRootTitle.Data(),
					   10, 0, 10 * m_trendTimeScale);
              m_rootHistogram -> GetXaxis() -> SetTimeDisplay(1);
	    }
	  }
	} else {
          // cannot get sources from DIM
          if (m_retryInit == 1) { 
            std::cout << "Failed to load " << m_dimServiceName << std::endl;
            beEmptyHisto(); 
          }
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
						m_refreshTime, 
						"GauchoDimInfoMonObjectPresenter");
      
      if ( ( 0 != m_dimInfoMonObject ) && ( m_dimInfoMonObject->createMonObject() ) ) {
          
	m_retryInit = 0;          
          
	if (m_verbosity >= pres::Verbose) 
	  std::cout << "GauchoDimInfoMonObjectPresenter: Using dimServiceName from DB." << std::endl;
      
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
	    if (m_verbosity >= pres::Debug) { monTH1D->print(); }
	    m_rootHistogram = monTH1D->hist();
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
	    if (m_verbosity >= pres::Debug) { monTH2D->print(); }
	    m_rootHistogram = monTH2D->hist();
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
	    if (m_verbosity >= pres::Debug) { monProfile->print(); }
	    m_rootHistogram = monProfile->profile();
	  }
	} else {
	  std::cout << "MonObject not included in the Presenter: " <<
	    m_histogramType << std::endl;
	  m_rootHistogram =  0;
	}
	if ( 0 != m_rootHistogram ) {
	  m_histoRootName = TString(Form("%s;%i",
					 m_identifier.c_str(),
					 m_instance));
	  m_rootHistogram->AddDirectory(kFALSE); }
	m_rootHistogram->SetName(m_histoRootName);
      } else {
	if (m_retryInit == 1) {
	  std::cout << "Failed to load " << m_dimServiceName.c_str() << std::endl;
	  beEmptyHisto(); 
	}
      }
    }

    // cannot get sources from DIM
    if (m_dimBrowser && (m_retryInit > 0)) {
      std::string newName =assembleCurrentDimServiceName();

      if (m_verbosity >= pres::Verbose) {
        std::cout << "DB DIM field invalid, retrying init using "
                  << newName << std::endl;
      }      
      if ( !newName.empty() ) {
        m_dimServiceName = newName;
        --m_retryInit;
        if (m_verbosity >= pres::Verbose) {
          std::cout << "retrying Init with " << m_dimServiceName << std::endl;
        }
        initHistogram();
      } else {
        std::cout << "failed to find DIM service '" << m_dimServiceName << "'" << std::endl;
      }
    }
  } else if (m_isAnaHist && m_anaSources.size() > 0) { // analib hist
    if (m_verbosity >= pres::Debug) 
      std::cout << "initializing virtual histogram " << m_identifier << std::endl;
    
    boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
    boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
    if (oraLock && rootLock) {
      std::vector<TH1*> sources(m_anaSources.size());
      bool sourcesOk = true;
      
      
      for (unsigned int i=0; i< m_anaSources.size(); ++i) {
        if (m_verbosity >= pres::Debug) {
          std::cout << "   ->  virtual histogram source " << m_anaSources[i]->identifier() << std::endl;
        }
        m_anaSources[i]->initHistogram();
        sources[i]= m_anaSources[i]->m_rootHistogram;
        if (m_anaSources[i]->isEmptyHisto() ) {
          sourcesOk = false;
          if (m_verbosity >= pres::Verbose) {
            std::cout << "source " << m_anaSources[i]->identifier() << " is empty" << std::endl;
          }
        }
      }
      if (sourcesOk) {
        makeVirtualHistogram(sources);
      }
      else {
        if (m_verbosity >= pres::Verbose) {
          std::cout << "virtual histogram sources not found!" << std::endl;
        }
        beEmptyHisto(); 
      }
    }
  }

  if ( 0 == m_rootHistogram) beEmptyHisto();
  else m_rootHistogram->SetBit(kNoContextMenu);
  
  // Apply default drawing options:
  applyDefaultDrawOptions( ) ;

  if (m_verbosity >= pres::Verbose && m_isEmptyHisto) 
    std::cout << "Histogram " << m_identifier << " empty after Init" << std::endl;
 
  if (false == m_isEmptyHisto) {
    fillHistogram();
    if (m_verbosity >= pres::Verbose && m_isEmptyHisto) 
      std::cout << "Histogram " << m_identifier << " empty after Fill" 
		<< std::endl;
  } else {
    if ( m_onlineHistogram && 0 == m_retryInit ) {
      std::cout << "DIM histo '" << m_onlineHistogram->dimServiceName() << "' = '" 
                << m_dimServiceName << "' not found" << std::endl;
    }
    beEmptyHisto(); 
  }  
}

//=============================================================================
// Set empty histo
//=============================================================================
void DbRootHist::beEmptyHisto() {
  m_isEmptyHisto = true;
  if ( 0 != m_rootHistogram ) { delete m_rootHistogram; m_rootHistogram = 0; }
  std::string dummyTitle = "ERROR: missing sources for ";
  dummyTitle += identifier();

  m_rootHistogram = new TH1F(identifier().c_str(), dummyTitle.c_str(),
			     1, 0., 1.);

  if( 0 != m_rootHistogram ) {
    m_rootHistogram->SetBit(kNoContextMenu);
    m_rootHistogram->AddDirectory(kFALSE);
    setRootHistogram( m_rootHistogram ) ;
  }

  applyDefaultDrawOptions() ;
}

//=============================================================================
// Fill histogram
//=============================================================================
void DbRootHist::fillHistogram() {
  if ( ( 0 != m_rootHistogram ) && ( pres::s_CNT != m_histogramType ) ) 
    m_rootHistogram->Reset();
  
  if ( ! m_isAnaHist ) {       
    if (pres::s_P1D == m_histogramType || pres::s_HPD == m_histogramType || 
	pres::s_H2D == m_histogramType || pres::s_H1D == m_histogramType) {

      if (m_dimInfo) {
	// wait until data has arrived
	int m_serviceSize = m_dimInfo->getSize()/sizeof(float);
	while (m_serviceSize <= 0) {
	  gSystem->Sleep(m_waitTime);
	  m_serviceSize = m_dimInfo->getSize()/sizeof(float);
	}
	if ( ( -1.0 != m_dimInfo->getFloat() ) && ( 0 != m_rootHistogram ) && 
	     ( !m_isEmptyHisto ) ) {
	  float* histoDimData;
	  histoDimData = (float*) m_dimInfo->getData();
	  if (pres::s_H1D == m_histogramType) {
	    const int   nBins   = (int) histoDimData[1];
	    const int   entries = (int) histoDimData[4];

	    // fill histogram
	    int offsetData  = 5;
	    int offsetError = 5+nBins+1;
	    // N.B. bin 0: underflow, bin nBins+1 overflow
	    m_rootHistogram->SetBinContent(0, (float) histoDimData[5]);
	    m_rootHistogram->SetBinContent(nBins+1,
					   (float) histoDimData[5 + nBins + 1]);
	    for (int i = 1; i <= nBins; ++i) {
	      m_rootHistogram->SetBinContent(i,
					     (float) histoDimData[offsetData + i]);
	      m_rootHistogram->SetBinError(i, (float) histoDimData[offsetError + i]);
	    }
	    m_rootHistogram->SetEntries(entries);
	  } else if (pres::s_H2D == m_histogramType) {        
	    const int   nBinsX   = (int) histoDimData[1];
	    const int   nBinsY   = (int) histoDimData[4];
	    const float entries  = (float) histoDimData[7];

	    int   iData = 8;  //current position in stream
	    float data  = 0;
	    for (int i=0; i<= nBinsX+1; ++i) {
	      for (int j=0; j <= nBinsY+1; ++j) {
		data = (float) histoDimData[iData];
		m_rootHistogram->SetBinContent(i, j, (float) data);
		iData ++;
	      }
	    }

	    for (int i=0; i<= nBinsX+1; ++i) {
	      for (int j=0; j <= nBinsY+1; ++j) {
		data = (float) histoDimData[iData];
		m_rootHistogram->SetBinError(i, j, (float) data);
		iData ++;
	      }
	    }

	    m_rootHistogram->SetEntries(entries);
	  } else if ( pres::s_P1D == m_histogramType || pres::s_HPD == m_histogramType) {
	    const int   nBins   = (int) histoDimData[1];
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

	    double yvalue = 0. ; 
	    double yerr = 0.;
	    // bin 0: underflow, nBins+1 overflow ?
	    for (int i = 0; i <= nBins+2; i++) {
	      yvalue = 0. ;    
	      if (entriesPerBin[i] > 0)
		yvalue = sumWTPerBin[i]/entriesPerBin[i];
	      // mean in Y                
	      m_rootHistogram->SetBinContent(i, yvalue);    
      
	      yerr = 0;
	      if (entriesPerBin[i] > 0)
		yerr = sqrt(sumWT2PerBin[i]/entriesPerBin[i]-yvalue*yvalue);
	      // RMS = sqrt(E[x**2]-E[x]**2)
	      m_rootHistogram->SetBinError(i, yerr);
        
	    }
	    m_rootHistogram->SetEntries(entries);
	  }
	}
      }
    } else if ( ( pres::s_CNT == m_histogramType ) &&
		( 0 != m_rootHistogram ) && ( !m_isEmptyHisto ) ) {
      if (pres::MonRate == m_monRateRace->effServiceType()) {
        double dimContent = 0;
        if (m_monRateRace &&  
            (true == m_monRateRace->isRateValid()) ) {
          dimContent = m_monRateRace->currentValue();
          m_histoRootTitle = TString(Form("%s",
					  (m_monRateRace->title()).c_str()));
          m_rootHistogram->SetTitle(m_histoRootTitle.Data());
          if ( ( 0 != m_presenterInfo ) && 
	       ( ( pres::Online == m_presenterInfo->presenterMode() ) ||
		 ( pres::EditorOnline == m_presenterInfo->presenterMode() ) ) ) {
	    int i = 0;
	    int  nbins = m_rootHistogram->GetNbinsX() ;
	    double stats[5]={0,0,0,0,0};
	    m_rootHistogram->PutStats(stats); // reset mean value, etc   
	    for ( i=1 ; i<=nbins-1 ; ++i ) 
	      m_rootHistogram->SetBinContent(i,m_rootHistogram->GetBinContent(i+1));
	    m_rootHistogram->SetBinContent(i,dimContent);   
          } else if ( 0 != m_presenterInfo && 
		      pres::Batch == m_presenterInfo->presenterMode() ) {
	    m_rootHistogram->SetBinContent(m_trendBin, dimContent);
	    m_trendBin++;              
          }
        } else if (m_monRateRace && false == m_monRateRace->isRateValid())  { 
	  // Invalid data, just shift histo
          if ( ( 0 != m_presenterInfo ) && 
	       ( ( pres::Online == m_presenterInfo->presenterMode()) ||
		 ( pres::EditorOnline == m_presenterInfo->presenterMode()) ) ) {
	    int i = 0;
	    int  nbins = m_rootHistogram->GetNbinsX();
	    double stats[5]={0,0,0,0,0};
	    m_rootHistogram->PutStats(stats); // reset mean value, etc   
	    for ( i=1 ; i<=nbins-1 ; ++i )
	      m_rootHistogram->SetBinContent(i,m_rootHistogram->GetBinContent(i+1));
	    m_rootHistogram->SetBinContent(nbins, 0.0);   
          } else if ( ( 0 != m_presenterInfo ) && 
		      ( pres::Batch == m_presenterInfo->presenterMode() ) ) {
	    m_rootHistogram->SetBinContent(m_trendBin, 0.0);
	    m_trendBin++;              
          }
        }
      } else if ( pres::TCKinfo == m_monRateRace->effServiceType() &&
		  m_monRateRace && (true == m_monRateRace->isRateValid())) {
        m_histoRootTitle = TString( Form( "%x", m_monRateRace->currentTCK()));
        m_rootHistogram->SetTitle(m_histoRootTitle.Data());
      } 
    } else if ( ( s_pfixMonProfile == m_histogramType ) || ( s_pfixMonH1D == m_histogramType ) ||
		( s_pfixMonH2D == m_histogramType ) ) {
      if (m_dimInfoMonObject) {
        if (m_dimInfoMonObject->loadMonObject()) {
          
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
        }
      }
    }
    if (m_cleared && m_offsetHistogram) {     
      if (pres::s_H1D == m_histogramType ||
	  pres::s_P1D == m_histogramType || pres::s_HPD == m_histogramType ||
	  s_pfixMonProfile == m_histogramType ||
	  s_pfixMonH1D == m_histogramType) {
	for (int i = 1; i <= m_rootHistogram->GetNbinsX(); ++i) {
	  m_rootHistogram->SetBinContent(i, m_rootHistogram->GetBinContent(i)
					 - m_offsetHistogram->GetBinContent(i));
	  m_rootHistogram->SetBinError(i, sqrt(pow(m_rootHistogram->
						   GetBinError(i), 2) - pow(m_offsetHistogram->
									    GetBinError(i), 2)));
	}
      } else if (pres::s_H2D == m_histogramType ||
		 s_pfixMonH2D == m_histogramType) {
	for (int i=1; i<= m_rootHistogram->GetNbinsX() ; ++i) {
	  for (int j=1; j <= m_rootHistogram->GetNbinsY() ; ++j) {
	    m_rootHistogram->SetBinContent(i, j,
					   m_rootHistogram->GetBinContent(i, j) -
					   m_offsetHistogram->GetBinContent(i, j));
	    m_rootHistogram->SetBinError(i, j, sqrt(pow(m_rootHistogram->
							GetBinError(i, j), 2) -
						    pow(m_offsetHistogram->GetBinError(i, j), 2)));
	  }
	}
      }
    }
    if (s_pfixMonH2D == m_histogramType) { 
      if ((TH2D::Class() == m_rootHistogram->IsA()) &&
	  m_histogramImage) {
	m_histogramImage->SetImage( (const Double_t *)((TH2D*)m_rootHistogram)->GetArray(), 
				    ((TH2D*)m_rootHistogram)->GetNbinsX() + 2,
				    ((TH2D*)m_rootHistogram)->GetNbinsY() + 2, m_prettyPalette); 
      }
    }      
  } else if (m_isAnaHist && m_anaSources.size()>0)  { // virtual histogram
    boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
    boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
    if (oraLock && rootLock) {
      std::vector<TH1*> sources(m_anaSources.size());
      bool sourcesOk = true;
      for (unsigned int i=0; i< m_anaSources.size(); ++i) {
        m_anaSources[i]->fillHistogram();
        sources[i]= m_anaSources[i]->m_rootHistogram;
        if (m_anaSources[i]->isEmptyHisto() ) 
          sourcesOk = false;
      }
      if(sourcesOk) {
        makeVirtualHistogram(sources);
      }
    }
  }
  if( m_onlineHistogram) {
    if (m_onlineHistogram->hasFitFunction() && NULL != m_fitfunction && 
	false == m_isEmptyHisto) {
      // trick to update fit w/o redrawing
      TF1* newfit = new TF1(*(m_fitfunction->fittedfun())); // clone fit TF1
      newfit->SetRange( m_rootHistogram->GetXaxis()->GetXmin(),
      			m_rootHistogram->GetXaxis()->GetXmax() );
      m_rootHistogram->GetListOfFunctions()->Add(newfit); // attach newfit to histo, which now owns it
      m_rootHistogram->Fit(newfit,"QN"); // fit w/o drawing/storing
      for (int ip=0; ip <newfit->GetNpar(); ip++) {
        // update parameters of stored fit function
        m_fitfunction->fittedfun()->SetParameter(ip, newfit->GetParameter(ip));
      }
    }
  }
  if (m_onlineHistogram ) { setTH1FromDB(); }
  if (0 != m_hostingPad) m_hostingPad->Modified();
  
}




//=============================================================================
// Set online histogram 
//=============================================================================
bool DbRootHist::setOnlineHistogram(OnlineHistogram* newOnlineHistogram) {
  bool out = false;
  bool isInit = (NULL == m_onlineHistogram);
  if (newOnlineHistogram) {
    if (newOnlineHistogram->identifier() == m_identifier &&
        false == newOnlineHistogram->isAbort()) {
      m_onlineHistogram = newOnlineHistogram;
      if ( newOnlineHistogram->isAnaHist() ) {
        m_histogramType = newOnlineHistogram->hstype();
      }
      if ( ( 0 != m_rootHistogram ) && isInit) { 
        setTH1FromDB(); }
      out = true;
    } else {
      newOnlineHistogram->warningMessage("provided OnlineHistogram object is not compatible");
    }
  }

  loadAnaSources();
  return out;
}

//=============================================================================
// Set root histogram
//=============================================================================
bool DbRootHist::setRootHistogram( TH1* newRootHistogram ) {
  bool out = false;
  if ( newRootHistogram ) {
    m_rootHistogram = newRootHistogram ;
    applyDefaultDrawOptions( ) ;
    if ( m_onlineHistogram ) { 
      setTH1FromDB() ; 
      // Set correct names and titles
      m_rootHistogram -> SetName( m_onlineHistogram -> hname().c_str() ) ;
    }
    out = true;
  }
  return out;
}

//=============================================================================
// Connect to database
//=============================================================================
bool DbRootHist::connectToDB(OnlineHistDB* onlineHistDbSession,
                             std::string page,
                             int instanceOnPage) {
  bool out;
  if (onlineHistDbSession) { m_session = onlineHistDbSession; }
  if (m_session) {
    out = setOnlineHistogram(m_session->getNewHistogram(m_identifier, page,
							instanceOnPage));
  } else { out = false; }
  return out;
}

//=============================================================================
// Set histogram from database
//=============================================================================
void DbRootHist::setTH1FromDB() {
  if ( m_historyTrendPlotMode ) return;
  if ( ( 0 == m_onlineHistogram ) || ( 0 == m_rootHistogram ) )
    return ;

  boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
        
  if ( oraLock && rootLock ) {  
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt;
    if (m_onlineHistogram->getDisplayOption("LABEL_X", &sopt)) 
      m_rootHistogram->SetXTitle (sopt.data());
    if (m_onlineHistogram->getDisplayOption("LABEL_Y", &sopt))
      m_rootHistogram->SetYTitle (sopt.data());
    if (m_onlineHistogram->getDisplayOption("LABEL_Z", &sopt)) 
      m_rootHistogram->SetZTitle (sopt.data());
    double bxmin=m_rootHistogram->GetXaxis()->GetXmin();
    double bxmax=m_rootHistogram->GetXaxis()->GetXmax();

    if (m_onlineHistogram->getDisplayOption("XMIN", &fopt)) { bxmin=fopt; }
    if (m_onlineHistogram->getDisplayOption("XMAX", &fopt)) { bxmax=fopt; }
    m_rootHistogram->GetXaxis()->SetRangeUser(bxmin,bxmax);
    
    if (m_onlineHistogram->dimension() <2) { // 1d histograms
      if (m_onlineHistogram->getDisplayOption("YMIN", &fopt))
        m_rootHistogram->SetMinimum(fopt);
      if (m_onlineHistogram->getDisplayOption("YMAX", &fopt)) 
        m_rootHistogram->SetMaximum(fopt);
    } else {  // 2d histograms
      double bymin = m_rootHistogram->GetYaxis()->GetXmin();
      double bymax = m_rootHistogram->GetYaxis()->GetXmax();
      if (m_onlineHistogram->getDisplayOption("YMIN", &fopt)) { bymin=fopt; }
      if (m_onlineHistogram->getDisplayOption("YMAX", &fopt)) { bymax=fopt; }
      m_rootHistogram->GetYaxis()->SetRangeUser(bymin, bymax);
      if (m_onlineHistogram->getDisplayOption("ZMIN", &fopt)) 
        m_rootHistogram->SetMinimum(fopt);
      if (m_onlineHistogram->getDisplayOption("ZMAX", &fopt)) 
        m_rootHistogram->SetMaximum(fopt);
    }
    if (m_onlineHistogram->getDisplayOption("STATS", &iopt))
      m_rootHistogram->SetStats(0 != iopt);
    
    if (m_onlineHistogram->getDisplayOption("SHOWTITLE", &sopt)) 
      m_rootHistogram->SetTitle(sopt.c_str());
            
    if (m_onlineHistogram->getDisplayOption("REF", &sopt)) 
      m_refOption = sopt;      

    if (m_onlineHistogram->getDisplayOption("FILLSTYLE", &iopt))
      m_rootHistogram->SetFillStyle(iopt);
    if (m_onlineHistogram->getDisplayOption("FILLCOLOR", &iopt))
      m_rootHistogram->SetFillColor(iopt);
    if (m_onlineHistogram->getDisplayOption("LINESTYLE", &iopt)) 
      m_rootHistogram->SetLineStyle(iopt);
    if (m_onlineHistogram->getDisplayOption("LINECOLOR", &iopt)) 
      m_rootHistogram->SetLineColor(iopt);
    if (m_onlineHistogram->getDisplayOption("LINEWIDTH", &iopt)) 
      m_rootHistogram->SetLineWidth(iopt);
    if (m_onlineHistogram->getDisplayOption("MARKERSIZE", &fopt)) 
      m_rootHistogram->SetMarkerSize((Size_t)fopt);
    if (m_onlineHistogram->getDisplayOption("MARKERSTYLE", &iopt))
      m_rootHistogram->SetMarkerStyle(iopt);
    if (m_onlineHistogram->getDisplayOption("MARKERCOLOR", &iopt))
      m_rootHistogram->SetMarkerColor(iopt);

    if (m_onlineHistogram->getDisplayOption("TIT_X_SIZE", &fopt)) 
      m_rootHistogram->GetXaxis()->SetTitleSize(fopt);
    if (m_onlineHistogram->getDisplayOption("TIT_X_OFFS", &fopt))
      m_rootHistogram->GetXaxis()->SetTitleOffset(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_X_SIZE", &fopt)) 
      m_rootHistogram->GetXaxis()->SetLabelSize(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_X_OFFS", &fopt))
      m_rootHistogram->GetXaxis()->SetLabelOffset(fopt);
    if (m_onlineHistogram->getDisplayOption("NDIVX", &iopt))
      m_rootHistogram->SetNdivisions(iopt,"X");

    if (m_onlineHistogram->getDisplayOption("TIT_Y_SIZE", &fopt)) 
      m_rootHistogram->GetYaxis()->SetTitleSize(fopt);
    if (m_onlineHistogram->getDisplayOption("TIT_Y_OFFS", &fopt)) 
      m_rootHistogram->GetYaxis()->SetTitleOffset(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_Y_SIZE", &fopt)) 
      m_rootHistogram->GetYaxis()->SetLabelSize(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_Y_OFFS", &fopt)) 
      m_rootHistogram->GetYaxis()->SetLabelOffset(fopt);
    if (m_onlineHistogram->getDisplayOption("NDIVY", &iopt))
      m_rootHistogram->SetNdivisions(iopt,"Y");

    if (m_onlineHistogram->getDisplayOption("TIT_Z_SIZE", &fopt)) 
      m_rootHistogram->GetZaxis()->SetTitleSize(fopt);
    if (m_onlineHistogram->getDisplayOption("TIT_Z_OFFS", &fopt)) 
      m_rootHistogram->GetZaxis()->SetTitleOffset(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_Z_SIZE", &fopt))
      m_rootHistogram->GetZaxis()->SetLabelSize(fopt);
    if (m_onlineHistogram->getDisplayOption("LAB_Z_OFFS", &fopt))
      m_rootHistogram->GetZaxis()->SetLabelOffset(fopt);
    
    if (m_onlineHistogram->getDisplayOption("NORM", &fopt))
      if (fopt>0.) m_rootHistogram->SetNormFactor(fopt);

    // custom bin labels
    if ( 0 != m_rootHistogram->GetXaxis()) {
      if (m_onlineHistogram->nXbinlabels() > 0) {
        for (unsigned int il = 0; il < m_onlineHistogram->nXbinlabels(); il++) {
          sopt = m_onlineHistogram->binlabel(il,0);
          m_rootHistogram->GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
    if (0 != m_rootHistogram->GetYaxis()) {
      if (m_onlineHistogram->nYbinlabels() > 0) {
        for (unsigned int il = 0; il < m_onlineHistogram->nYbinlabels(); il++) {
          sopt = m_onlineHistogram->binlabel(il,1);
          m_rootHistogram->GetYaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
  }
}

//===============================================================================
// Set Pad margins from DB
//===============================================================================
void DbRootHist::setPadMarginsFromDB(TPad * pad) {
  if ( ( 0 == pad ) || ( 0 == m_onlineHistogram ) ) return ;

  float fopt = 0.0;    
  if ( m_onlineHistogram -> getDisplayOption( "MARGIN_TOP"    , &fopt ) )
    pad -> SetTopMargin( fopt ) ;
  if ( m_onlineHistogram -> getDisplayOption( "MARGIN_BOTTOM" , &fopt ) ) 
    pad -> SetBottomMargin( fopt ) ;
  if ( m_onlineHistogram -> getDisplayOption( "MARGIN_LEFT"   , &fopt ) ) 
    pad -> SetLeftMargin( fopt ) ;
  if ( m_onlineHistogram -> getDisplayOption( "MARGIN_RIGHT"  , &fopt ) ) 
    pad -> SetRightMargin( fopt ) ;
}

//===============================================================================
// Fit histogram
//===============================================================================
void DbRootHist::fit() {
  if ( 0 == m_onlineHistogram ) return ;
  if ( ( m_onlineHistogram->hasFitFunction() ) && ( 0 != m_rootHistogram ) ) {
    std::string Name;
    std::vector<float> initValues;
    gStyle -> SetOptFit( 1111111 ) ;
    m_onlineHistogram -> getFitFunction(Name,&initValues);

    if (m_verbosity >= pres::Verbose) 
      std::cout << "fitting histogram " << m_onlineHistogram->identifier() <<
	" with function "<< Name << std::endl;
    
    if ( 0 == m_fitfunction) {
      OMAFitFunction* requestedFit =  m_analysisLib->getFitFunction(Name);
      
      if ( 0 == requestedFit ) {
	std::cerr << "Unknown fit function: " << Name 
		  << " to fit histogram " 
		  << m_onlineHistogram -> identifier() 
		  << std::endl ;
	return ;
      }
      
      // clone to own it and be thread-safe
      m_fitfunction = new OMAFitFunction(*requestedFit);
    }
    m_fitfunction->fit(m_rootHistogram, &initValues);
  }
}

//=============================================================================
// set options from database
//=============================================================================
void DbRootHist::setDrawOptionsFromDB(TPad* pad) {
  if ( ( 0 == m_onlineHistogram ) || ( 0 == m_rootHistogram ) ) return ;
  int iopt = 0;
  float fopt = 0.0;
  std::string sopt("");
  
  // TPaveStats is obtained after a pad->Draw(), but note that changing OptStat
  // doesn't resize the Pave.. thus it's better to set the global stat options also 
  // before drawing
  
  if (m_historyTrendPlotMode) { // special settings for trend mode
    m_rootHistogram->SetDrawOption("E1");
    m_rootHistogram->SetStats(0);
    TPaveStats* stats =  
      (TPaveStats*) m_rootHistogram->GetListOfFunctions()->FindObject("stats");
    
    if (stats) stats->Delete();
    pad->SetLogx(0);
    pad->SetLogy(0);
    fopt=.16;
    pad->SetBottomMargin( (Float_t) fopt);
    bool histByRun=false;
    if( 0 != m_presenterInfo ) 
      histByRun = m_presenterInfo -> globalHistoryByRun() ;

    if ( histByRun ) {
      m_rootHistogram->GetXaxis()->SetTitle("run");
      fopt=1.2;
      m_rootHistogram->GetXaxis()->SetTitleOffset( (Float_t) fopt);
    }
    else {
      m_rootHistogram->GetXaxis()->SetTitle("time");
      fopt=1.6;
      m_rootHistogram->GetXaxis()->SetTitleOffset( (Float_t) fopt);
    }
    std::string ylab="Average";
    if (m_onlineHistogram->getDisplayOption("LABEL_X", &sopt)) 
      ylab = ylab + " (" + sopt + ")";
   
    m_rootHistogram->SetYTitle (ylab.data());
  }
  else { // normal case
    int statOpt;
    if(false == m_onlineHistogram->getDisplayOption("STATS", &statOpt))
      statOpt = pres::s_defStatOptions;
    if (0 != statOpt) {
      int statStyle=0;
      if (m_onlineHistogram->getDisplayOption("STATTRANSP", &iopt))
        if (iopt>0) statStyle=1001;
     
      gStyle->SetStatStyle(statStyle); // apparently, this must be called before SetOptStat
      
      gStyle->SetOptStat( statOpt );
      TPaveStats* stats =  
        (TPaveStats*)m_rootHistogram->GetListOfFunctions()->FindObject("stats"); 
      
      if (stats) {
        double x1=stats->GetX1NDC();
        double x2=stats->GetX2NDC();
        double y1=stats->GetY1NDC();
        double y2=stats->GetY2NDC();
        if (m_onlineHistogram->getDisplayOption("STAT_X_OFFS", &fopt)) x1 = fopt;
        if (m_onlineHistogram->getDisplayOption("STAT_X_SIZE", &fopt)) x2 = x1 + fopt;
        if (m_onlineHistogram->getDisplayOption("STAT_Y_OFFS", &fopt)) y1 = fopt;
        if (m_onlineHistogram->getDisplayOption("STAT_Y_SIZE", &fopt)) y2 = y1 + fopt;
        
        stats->SetX1NDC(x1);
        stats->SetX2NDC(x2);
        stats->SetY1NDC(y1);
        stats->SetY2NDC(y2);
        // save it to check if was changed at saving time
        if (m_statpave) { delete m_statpave; m_statpave = 0; }
        m_statpave = (TPave*)stats->Clone(); // memleak?
      }
    } 
    // title pave
    TPaveText* titpave = (TPaveText*) pad->GetPrimitive("title");
    if (titpave) {
      int optTit=1;
      if(m_onlineHistogram->getDisplayOption("NOTITLE", &iopt)) {
        if (iopt>0) { //user requires no title window
          optTit=0;
        }
      }
      if( 0 == optTit) {
        // put window title out of sight (better than using TStyle::SetOptTitle which is too global..)          
        titpave->SetX1NDC(-2);
        titpave->SetX2NDC(-1);
      }
      else {
        double x1=titpave->GetX1NDC();
        double x2=titpave->GetX2NDC();
        double y1=titpave->GetY1NDC();
        double y2=titpave->GetY2NDC();
        if (m_onlineHistogram->getDisplayOption("HTIT_X_OFFS", &fopt)) x1 = fopt;
        if (m_onlineHistogram->getDisplayOption("HTIT_X_SIZE", &fopt)) x2 = x1 + fopt;
        if (m_onlineHistogram->getDisplayOption("HTIT_Y_OFFS", &fopt)) y1 = fopt;
        if (m_onlineHistogram->getDisplayOption("HTIT_Y_SIZE", &fopt)) y2 = y1 + fopt;

        titpave->SetX1NDC(x1);
        titpave->SetX2NDC(x2);
        titpave->SetY1NDC(y1);
        titpave->SetY2NDC(y2);
        // save it to check if was changed at saving time
        if (m_titpave) { delete m_titpave; m_titpave = 0; }
        m_titpave = (TPave*)titpave->Clone();
      }
    }
    
    if (m_onlineHistogram->getDisplayOption("DRAWOPTS", &sopt) ) {
      if(m_isOverlap && sopt.find("SAME") == std::string::npos ) sopt += "SAME";
      m_rootHistogram->SetDrawOption(sopt.c_str());
    }
    if (m_onlineHistogram->getDisplayOption("LOGX", &iopt)) pad->SetLogx(1);
    if (m_onlineHistogram->getDisplayOption("LOGY", &iopt)) {
      if (m_rootHistogram->GetEntries()>0) pad->SetLogy(1) ;
      // Set log scale also if the minimum and maximum scale is not 0
      else if ( ( m_rootHistogram -> GetMinimum() > 0. ) &&
		( m_rootHistogram -> GetMaximum() > 0. ) ) pad -> SetLogy( 1 ) ;
    }

    int gridx = gStyle->GetPadGridX();
    int gridy = gStyle->GetPadGridY();
    if (m_onlineHistogram->getDisplayOption("GRIDX", &iopt)) gridx=iopt; 
    if (m_onlineHistogram->getDisplayOption("GRIDY", &iopt)) gridy=iopt; 
    pad->SetGrid(gridx, gridy);
    if ( m_rootHistogram->GetDimension() > 1) {
      if (m_onlineHistogram->getDisplayOption("LOGZ", &iopt)) pad->SetLogz(1);
      if (m_onlineHistogram->getDisplayOption("THETA", &fopt)) pad->SetTheta(fopt);
      if (m_onlineHistogram->getDisplayOption("PHI", &fopt)) pad->SetPhi(fopt);
    }
    
    if(m_onlineHistogram->getDisplayOption("TICK_X", &iopt)) pad->SetTickx(iopt);
    if(m_onlineHistogram->getDisplayOption("TICK_Y", &iopt)) pad->SetTicky(iopt);
    if(m_onlineHistogram->getDisplayOption("PADCOLOR", &iopt)) pad->SetFillColor(iopt);
  }
}

//=============================================================================
// update option to current value only  if:
//   is already set and the value has changed
//   is not set and value is different from default
// return true if option is updated in the DB
//=============================================================================
bool DbRootHist::updateDBOption(std::string opt, void* value, bool isDefault) {
  if ( ( 0 == m_onlineHistogram ) || ( 0 == m_rootHistogram ) ) 
    return false ;
  
  bool update = false;
  bool out = false;
  if ( m_onlineHistogram->isSetDisplayOption(opt) ) 
    update = m_onlineHistogram->changedDisplayOption(opt, value);
  else 
    if (!isDefault) { update=true; }
  if (update) out = m_onlineHistogram->setDisplayOption(opt, value);
  return out;
}

//=============================================================================
// Save Histo to DB
//=============================================================================
bool DbRootHist::saveTH1ToDB(TPad* pad) {
  if ( ( 0 == m_onlineHistogram ) || ( 0 == m_rootHistogram ) ) return false ;

  bool out = false;

  int iopt = 0;
  float fopt = 0.0;
  std::string sopt("");
  
  sopt = m_rootHistogram->GetXaxis()->GetTitle();
  out |= updateDBOption("LABEL_X", &sopt, sopt.empty());
  
  sopt = m_rootHistogram->GetYaxis()->GetTitle();
  out |= updateDBOption("LABEL_Y", &sopt, sopt.empty());
  // note: axis minima and maxima should not be set in this way, but
  // through the web interface
  
  TPaveStats* stats = (TPaveStats*)m_rootHistogram->GetListOfFunctions()
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
  
  iopt = (int) m_rootHistogram->GetFillStyle();
  out |= updateDBOption("FILLSTYLE", &iopt,
                        iopt == (int) gStyle->GetHistFillStyle());
  iopt = (int) m_rootHistogram->GetFillColor();
  out |= updateDBOption("FILLCOLOR", &iopt,
			iopt == (int) gStyle->GetHistFillColor());
  iopt = (int) m_rootHistogram->GetLineStyle();
  out |= updateDBOption("LINESTYLE", &iopt,
			iopt == (int) gStyle->GetHistLineStyle());
  iopt = (int) m_rootHistogram->GetLineColor();
  out |= updateDBOption("LINECOLOR", &iopt,
			iopt == (int) gStyle->GetHistLineColor());
  iopt = (int) m_rootHistogram->GetLineWidth();
  out |= updateDBOption("LINEWIDTH", &iopt,
			iopt == (int) gStyle->GetHistLineWidth());
  iopt = (int) m_rootHistogram->GetMarkerStyle();
  out |= updateDBOption("MARKERSTYLE", &iopt,
			iopt == (int) gStyle->GetMarkerStyle());
  fopt = (float) m_rootHistogram->GetMarkerSize();
  out |= updateDBOption("MARKERSIZE", &fopt,
      TMath::Abs(fopt - (float) gStyle->GetMarkerSize())<0.00001 );
  iopt = (int) m_rootHistogram->GetMarkerColor();
  out |= updateDBOption("MARKERCOLOR", &iopt,
			iopt == (int) gStyle->GetMarkerColor());
  
  sopt = m_rootHistogram->GetDrawOption();
  if (sopt.empty()) { sopt = m_rootHistogram->GetOption(); }
  out |= updateDBOption("DRAWOPTS",  &sopt, sopt.empty());
  
  fopt = m_rootHistogram->GetXaxis()->GetTitleSize();
  out |= updateDBOption("TIT_X_SIZE", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetTitleSize("X"))<0.0001);
  fopt = m_rootHistogram->GetXaxis()->GetTitleOffset();
  out |= updateDBOption("TIT_X_OFFS", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetTitleOffset("X"))<0.0001);
  fopt = m_rootHistogram->GetXaxis()->GetLabelSize();
  out |= updateDBOption("LAB_X_SIZE", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetLabelSize("X"))<0.0001);
  fopt = m_rootHistogram->GetXaxis()->GetLabelOffset();
  out |= updateDBOption("LAB_X_OFFS", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetLabelOffset("X"))<0.0001);
  iopt = m_rootHistogram->GetNdivisions("X");
  out |= updateDBOption("NDIVX", &iopt,
			iopt == (int) gStyle->GetNdivisions("X"));
  
  fopt = m_rootHistogram->GetYaxis()->GetTitleSize();
  out |= updateDBOption("TIT_Y_SIZE", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetTitleSize("Y"))<0.0001);
  fopt = m_rootHistogram->GetYaxis()->GetTitleOffset();
  out |= updateDBOption("TIT_Y_OFFS", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Y"))<0.0001);
  fopt = m_rootHistogram->GetYaxis()->GetLabelSize();
  out |= updateDBOption("LAB_Y_SIZE", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetLabelSize("Y"))<0.0001);
  fopt = m_rootHistogram->GetYaxis()->GetLabelOffset();
  out |= updateDBOption("LAB_Y_OFFS", &fopt,
			TMath::Abs(fopt - (float) gStyle->GetLabelOffset("Y"))<0.0001);
  iopt = m_rootHistogram->GetNdivisions("Y");
  out |= updateDBOption("NDIVY", &iopt,
			iopt == (int) gStyle->GetNdivisions("Y"));
  
  if (m_onlineHistogram->dimension() > 1) {
    sopt = m_rootHistogram->GetZaxis()->GetTitle();
    out |= updateDBOption("LABEL_Z", &sopt, sopt.empty());
    fopt = m_rootHistogram->GetZaxis()->GetTitleSize();
    out |= updateDBOption("TIT_Z_SIZE",  &fopt,
			  TMath::Abs(fopt - (float) gStyle->GetTitleSize("Z"))<0.0001);
    fopt = m_rootHistogram->GetZaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_Z_OFFS",  &fopt,
			  TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Z"))<0.0001);
    fopt = m_rootHistogram->GetZaxis()->GetLabelSize();
    out |= updateDBOption("LAB_Z_SIZE",  &fopt,
			  TMath::Abs(fopt - (float) gStyle->GetLabelSize("Z"))<0.0001);
    fopt = m_rootHistogram->GetZaxis()->GetLabelOffset();
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
  return out;
}


//===========================================================================================
// Draw the histogram on the page
//===========================================================================================
void DbRootHist::draw(TCanvas* editorCanvas, double xlow, double ylow, double xup, double yup, 
                      bool fastHitMapDraw, TPad* overlayOnPad) {
  m_fastHitmapPlot = fastHitMapDraw;
  TPad* pad=overlayOnPad;
  m_isOverlap = (NULL != overlayOnPad);

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
  
  if (0 != m_rootHistogram) {
    if (TH2D::Class() == m_rootHistogram->IsA() &&
        m_fastHitmapPlot) {
      if (NULL == m_histogramImage) {      
        m_histogramImage = TImage::Create();
        m_histogramImage->SetConstRatio(false);
      }

      m_histogramImage->SetImage((const Double_t *)((TH2D*)m_rootHistogram)->GetArray(), 
				 ((TH2D*)m_rootHistogram)->GetNbinsX() + 2,
				 ((TH2D*)m_rootHistogram)->GetNbinsY() + 2, 
				 m_prettyPalette); // gWebImagePalette
      m_histogramImage->Draw();
    } else {
      std::string opt =  m_isOverlap ? "SAME" : "";
      m_rootHistogram->Draw(opt.c_str());
    }
  }
  setDrawOptionsFromDB(pad);

  // fit if requested
  fit(); 

  if (!m_isOverlap) pad->SetName(m_histoRootName);

  m_hostingPad = pad;

  if (NULL != m_session) {
    std::string sopt("");
    boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
    if (rootLock && 0 != m_onlineHistogram ) { 
      if (m_onlineHistogram->getDisplayOption("DRAWPATTERN", &sopt) &&
          false == m_isOverlap) {
        
        std::string drawPatternFile = m_analysisLib->refRoot() + "/" + 
          m_onlineHistogram->task() + "/" + sopt;
        
        TFile rootFile(drawPatternFile.c_str()); 
        
        if (rootFile.IsZombie()) {
          std::cout << "Error opening Root file" << std::endl;
        } else {
          TIter next1( rootFile.GetListOfKeys() ) ;
          TKey* key;    
          while ( ( key = (TKey*) next1() ) ) {
            if ( key -> ReadObj( ) -> InheritsFrom( TCanvas::Class() ) )
              m_drawPattern = (TCanvas*) key -> ReadObj() ;
          }
          m_drawPattern -> SetPad( TMath::Abs( xlow ) , TMath::Abs( ylow ) ,
                                   TMath::Abs( xup  ) , TMath::Abs( yup  ) ) ;
          m_drawPattern -> SetName( m_identifier.c_str() ) ;
          
          TPad *padsav = (TPad*)gPad;
          TObject *obj; 
          
          dynamic_cast< TAttLine* >( m_drawPattern ) -> Copy( (TAttLine&) *pad ) ;
          dynamic_cast< TAttFill* >( m_drawPattern ) -> Copy( (TAttFill&) *pad ) ;
          dynamic_cast< TAttPad*  >( m_drawPattern ) -> Copy( (TAttPad& ) *pad ) ;
          
          TIter next( m_drawPattern -> GetListOfPrimitives() ) ;
          while ( ( obj=next() ) ) {
            pad->cd();
            if ( TBox::Class()     == obj->IsA() ||
                 TLine::Class()    == obj->IsA() ||
                 TText::Class()    == obj->IsA() || 
                 TEllipse::Class() == obj->IsA() ) 
              obj->Draw();
          }
          if ( padsav ) padsav->cd();
        }
        rootFile.Close();
      }
    }
  }
}

//==========================================================================================
// Normalize histogram
//==========================================================================================
void DbRootHist::normalizeReference() {
  if (m_reference && (m_histogramType != pres::s_CNT)) {
    double normFactor = m_rootHistogram->GetNormFactor();

    // if GetNormFactor() >0, histogram is drawn normalized, just use the same normalization
    if (normFactor<0.1) {
      if (pres::s_Entries == m_refOption) {
        normFactor = m_rootHistogram->GetEntries();
        m_reference->SetNormFactor(normFactor);
      } else if (pres::s_Area == m_refOption) {
        normFactor = m_rootHistogram->Integral();
      }
    }
    m_reference->SetNormFactor(normFactor);
  }
}

void DbRootHist::referenceHistogram(ReferenceVisibility visibility) {
  std::string sopt;
  if ( 0 == m_rootHistogram ) return ;
  boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
  if ( rootLock && (pres::s_H2D != m_histogramType) && 
       (s_pfixMonH2D != m_histogramType) ) {

    if ( (0 == m_reference) &&
         ( pres::s_NoReference != m_refOption) &&
         ( m_rootHistogram->GetDimension() == 1) &&
         ( Show == visibility)) {

      TH1* ref = NULL;
      std::string tck( pres::s_default_tck ) ;
      if ( 0 != m_presenterInfo ) {
        if ( ( pres::s_eff_init != m_presenterInfo ->currentTCK() ) &&
	     ( !( m_presenterInfo -> currentTCK() ).empty() ) )
          tck = m_presenterInfo->currentTCK();
      }
      if (tck != pres::s_default_tck)
        ref = (TH1*)m_analysisLib->getReference(m_onlineHistogram, 1, tck);
      else 
        ref = (TH1*)m_analysisLib->getReference(m_onlineHistogram);
      
      if (ref) {
        if (m_reference) { delete m_reference; m_reference = 0; }
        m_reference = ref;
      }
    } 
    if ( ( 0 != m_reference ) && ( Show == visibility ) ) {
      // standard plot style
      m_reference->SetLineStyle(2);
      m_reference->SetLineColor(2); // red
      // normalization
      normalizeReference();
      TVirtualPad *padsav = gPad;
      m_hostingPad->cd();
      std::string refdopt="SAME";
      if (m_onlineHistogram->getDisplayOption("REFDRAWOPTS", &sopt) ) {
        refdopt += sopt;
      }
      m_reference->Draw(refdopt.c_str());
      m_reference->SetStats(0);

      TProfile* p= dynamic_cast<TProfile*>(m_rootHistogram);
      TProfile* pref=dynamic_cast<TProfile*>(m_reference);
      if (p && pref) {
        // spread display option is not propagated to online TProfile (while works in history mode):
        // use option for reference also for the online histogram
        p->SetErrorOption(pref->GetErrorOption());
      }

      m_hostingPad->Modified();
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
  bool dsnFound=false;
  std::string dimServiceName("");
  std::string dimServiceNameQueryBegining("");

  if (pres::s_CNT != dimServiceType &&
      !m_partition.empty()) {
    dimServiceNameQueryBegining = dimServiceType + pres::s_slash +
      m_partition +
      pres::s_underscore + pres::s_DimWildcard + m_onlineHistogram->rootName();
  } else if (pres::s_CNT == dimServiceType &&
             !m_partition.empty() ) {
    dimServiceNameQueryBegining = m_partition + pres::s_underscore + m_taskName + pres::s_DimWildcard;    
  } else {
    dimServiceNameQueryBegining = pres::s_DimWildcard;    
  }
  

  boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  if (oraLock && m_onlineHistogram &&  m_session) {
    bool botherDimBrowser(true);
    std::string taskName(m_onlineHistogram->task());
    std::vector<std::string*>::iterator lookForTasksNotRunningIt;
    if (0 < m_tasksNotRunning->size()) {
      for (lookForTasksNotRunningIt = m_tasksNotRunning->begin();
           lookForTasksNotRunningIt != m_tasksNotRunning->end();
           ++lookForTasksNotRunningIt) {
        if (((**lookForTasksNotRunningIt) == taskName)) {
          botherDimBrowser = false;
	  if (m_verbosity >= pres::Verbose) {
	    std::cout << "Skipping dead task: " << taskName << std::endl;
          }
          break;
        }
      }
    }
    if (botherDimBrowser) {
      boost::recursive_mutex::scoped_lock dimLock(*m_dimMutex);
      int numberOfServices = m_dimBrowser->getServices(dimServiceNameQueryBegining.c_str());
      if (m_verbosity >= pres::Verbose) {
	std::cout << "findDimServiceName quering DimBrowser for " << dimServiceNameQueryBegining << std::endl;
	std::cout << "    -> found "<<numberOfServices<<" services"<<std::endl;
      }
      while((dimType = m_dimBrowser->getNextService(dimService, dimFormat))) {
	std::string dimServiceCandidate(dimService);
	TString dimServiceTS(dimServiceCandidate);
	if (false == dimServiceTS.EndsWith(pres::s_gauchocomment.c_str())) {
	  HistogramIdentifier histogramCandidate = HistogramIdentifier(dimServiceCandidate);
	  if (0 == (histogramCandidate.histogramIdentifier().compare(m_onlineHistogram->identifier()))) {          
	    dimServiceName = dimServiceCandidate;              
	    if (m_verbosity >= pres::Verbose) {
	      std::cout << "findDimServiceName found compatible dim service " << dimServiceCandidate  << std::endl;
	    }
	    dsnFound=true;
	    break;
	  } 
	}
      }
      if (!dsnFound) {
	if (m_verbosity >= pres::Verbose)
	  std::cout << std::endl 
		    << "No compatible dim service found for: " 
		    << m_onlineHistogram->identifier() << std::endl;
      }      
    }
  }
  return dimServiceName;
}

void DbRootHist::setOverlapMode(pres::ServicePlotMode overlapMode) {
  if (pres::separate == overlapMode) {
    m_isOverlap = false;
  } else if (pres::overlap ==  overlapMode) {
    m_isOverlap = true;
  }
}

std::string DbRootHist::assembleCurrentDimServiceName() {
  std::string dimServiceName("");
  std::string dimServiceNameQueryBegining;
  
  if (0 != m_onlineHistogram) {
    if (0 == m_onlineHistogram->hstype().compare(pres::s_P1D) ||
        0 == m_onlineHistogram->hstype().compare(pres::s_HPD)) {
      dimServiceName = findDimServiceName(s_pfixMonProfile);
      if (dimServiceName.empty()) {
        dimServiceName = findDimServiceName(pres::s_HPD);
      }
      if (dimServiceName.empty()) {
        dimServiceName = findDimServiceName(pres::s_P1D);
      }        
    } else if (0 == m_onlineHistogram->hstype().compare(pres::s_H2D)) {
      dimServiceName = findDimServiceName(s_pfixMonH2D);
      if (dimServiceName.empty()) {
        dimServiceName = findDimServiceName(pres::s_H2D);          
      }              
    } else if (0 == m_onlineHistogram->hstype().compare(pres::s_H1D)) {
      dimServiceName = findDimServiceName(s_pfixMonH1D);
      if (dimServiceName.empty()) {
        dimServiceName = findDimServiceName(pres::s_H1D);          
      }
    } else if (0 == m_onlineHistogram->hstype().compare(pres::s_CNT)) {
      dimServiceName = findDimServiceName(pres::s_CNT);
    }
    
    if (!dimServiceName.empty()){
      if (m_verbosity >= pres::Verbose) {
        std::cout << std::endl << "assembled current Dim service name: "
                  << dimServiceName << std::endl << std::endl;
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


// produce virtual histogram on the fly
TH1* DbRootHist::makeVirtualHistogram(std::vector<TH1*> &sources) {
  if (!m_analysisLib) return NULL;
  OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
    (m_analysisLib->getAlg(m_creationAlgorithm));
  if(creator) {
    std::string htitle(onlineHistogram()->htitle());
    TH1* ref=NULL;
    if(creator->needRef() && m_anaSources.size()>0) {
      OnlineHistogram* firsth=m_anaSources[0]->onlineHistogram();
      std::string tck(pres::s_default_tck);
      if ( 0 != m_presenterInfo ) { 
        if ( ( pres::s_eff_init != m_presenterInfo -> currentTCK() ) &&
	     ( !(m_presenterInfo->currentTCK()).empty()) )  
          tck = m_presenterInfo->currentTCK();
      }
      if (tck != pres::s_default_tck)
        ref = (TH1*)m_analysisLib->getReference(firsth, 1, tck);
      else 
        ref = (TH1*)m_analysisLib->getReference(firsth);
    }
    m_rootHistogram = creator->exec(&sources, &m_parameters,
                                    m_identifier,
                                    htitle,
                                    isEmptyHisto() ? NULL : m_rootHistogram,
                                    ref);
    if (ref) delete ref;
    if(m_rootHistogram) beRegularHisto();
    else 
      if (m_verbosity >= pres::Verbose) std::cout<< "creator algorithm failed!"<<std::endl;
  }
  else 
    if (m_verbosity >= pres::Verbose) 
      std::cout << "creator algorithm for virtual histogram not found!" << std::endl;
  return m_rootHistogram;
}

//===========================================================================================
// Apply default Drawing options 
//===========================================================================================
void DbRootHist::applyDefaultDrawOptions( ) {
  if ( 0 == m_rootHistogram ) return ;
  m_rootHistogram -> SetStats( false ) ;
}
