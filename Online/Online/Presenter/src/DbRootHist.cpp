#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TMath.h>
#include <TThread.h>
//#include <TImage.h>
//#include <TGMsgBox.h>

#include <iostream>
#include <stdlib.h>
#include <malloc.h>

#include "presenter.h"
#include "DbRootHist.h"
#include "OnlineHistDB/OnlineRootHist.h"

using namespace pres;

DbRootHist::DbRootHist(std::string identifier, 
    std::string dimServiceName,
    int refreshTime,
    int localInstance,
    OnlineHistDB* histogramDB,
    OMAlib* AnaLib)
: OnlineRootHist(identifier, histogramDB, "_NONE_", localInstance),
  DimInfo(dimServiceName.c_str(), refreshTime, (float)-1.0, this),
  rootHistogram(NULL),
  hostingPad(NULL),
//  histogramImage(NULL),
  m_offsetHistogram(NULL),
  m_isAnaHist(false),
  m_anaLoaded(false),
  m_analib(AnaLib),
  m_refreshTime(refreshTime),
  m_histoDimData(NULL),
  m_toRefresh(false),
  m_cleared(false),   
  m_hstype(dimServiceName.substr(0,3)),
  m_hname(identifier),   // GG TODO: remove _$ from title
  m_localInstance(localInstance),
  m_waitTime(20),
  m_msgBoxReturnCode(0)
{
  // Prevent ROOT booking
  TH1::AddDirectory(kFALSE);

  if (NULL != histogramDB) {
    connectToDB(histogramDB, "_NONE_", localInstance);
    loadAnaSources();
  }
  
  // GG: one could use something else (time ?) to have a unique identifier,
  // and forget about instances
  m_histoRootName = TString(identifier + Form("__instance__%i", m_localInstance));

  initHistogram();
  fillHistogram();
//        new TGMsgBox(gClient->GetRoot(), this, "DIM Error",
//        Form("Histogram is not available online:\n\n%s\n",               
//          m_hname.c_str()), kMBIconExclamation,
//        kMBOk, &m_msgBoxReturnCode);
}

DbRootHist::~DbRootHist()
{
  m_toRefresh = false;
  if (m_offsetHistogram) { delete m_offsetHistogram; }
  if (rootHistogram) { delete rootHistogram; }
//  if (m_histoRootName) { delete m_histoRootName; }
//  if (histogramImage) { delete histogramImage; }
  if (hostingPad) { delete hostingPad; }  
  cleanAnaSources(); 
}

void DbRootHist::cleanAnaSources()
{
  for (unsigned int i=0; i< m_AnaSources.size(); ++i) { 
    delete m_AnaSources[i];
  }
  m_AnaSources.clear();
}

//   TImage *img = TImage::Create();
//
//   TF2 *f2 = new TF2("f2","(xygaus + xygaus(5) + xylandau(10))",-4,4,-4,4);
//   Double_t params[] = {130,-1.4,1.8,1.5,1, 150,2,0.5,-2,0.5, 3600,-2,0.7,-3,0.3};
//   f2->SetParameters(params);
//   TH2D *h2 = new TH2D("h2","xygaus + xygaus(5) + xylandau(10)",100,-4,4,100,-4,4);
//   h2->FillRandom("f2",40000);
//   img->SetImage((const Double_t *)h2->GetArray(), h2->GetNbinsX() + 2,
//                  h2->GetNbinsY() + 2, gHistImagePalette);
//   img->Draw();
//   img->StartPaletteEditor();

void DbRootHist::loadAnaSources()
{
  if (dbHist()) {
    m_isAnaHist=dbHist()->isAnaHist();
    if(m_isAnaHist && !m_anaLoaded) {
      dbHist()->getCreationDirections(m_CreationAlgorithm,m_sourcenames,m_parameters);
      
      cleanAnaSources();
      bool sourcesOK = true;
      for (unsigned int i=0; i< m_sourcenames.size(); ++i) {
        OnlineHistogram* histo = dbSession()->getHistogram(m_sourcenames[i]);
          m_AnaSources.push_back( new  DbRootHist( m_sourcenames[i],
  				histo->dimServiceName(),
  				m_refreshTime,
  				999,
  				dbSession(),
  				m_analib));
        if (NULL == m_AnaSources[i]->rootHistogram) { sourcesOK = false; }
      }
      m_anaLoaded = true;
      if (!sourcesOK) { // cannot get sources from DIM
    	if (rootHistogram) { delete rootHistogram; }
    	rootHistogram = new TH1F(m_histoRootName.Data(),
                      				 "ERROR: missing sources", 
                      				 1, 0., 1.);
    	cleanAnaSources();
      }
    }
  }
}

bool DbRootHist::setdbHist(OnlineHistogram*  oh) {
  bool out = OnlineRootHist::setdbHist(oh);
  loadAnaSources();
  return out;
}

void DbRootHist::enableEdit() {
  hostingPad->SetEditable(true);
  hostingPad->ResetBit(TPad::kCannotMove);
}

void DbRootHist::disableEdit() {
  hostingPad->SetEditable(false);
  hostingPad->SetBit(TPad::kCannotMove);
}

void DbRootHist::enableClear() {
  // TODO: deal with errors and Reset & Add instead of delete/new
  if (rootHistogram) {
    m_cleared = true;
    if (m_offsetHistogram) { delete m_offsetHistogram; }
    if (H1D == m_hstype) { //"H1D"
      m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
      //    m_offsetHistogram = (TH1F*)rootHistogram->Clone("m_offsetHistogram");
    } else if (H2D == m_hstype) { //"H2D"
      m_offsetHistogram = new TH2F(*dynamic_cast<TH2F*>(rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    } else if (P1D == m_hstype ||
               "HPD" == m_hstype) {  // P1D
      m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));
      m_offsetHistogram->SetBit(kNoContextMenu);
    }
  }
}

void DbRootHist::disableClear() {
  m_cleared = false;  
}

void DbRootHist::infoHandler()  {
  TThread::Lock();
  m_toRefresh = true;
  TThread::UnLock();
}

void DbRootHist::initHistogram() {

  // If not AnaLib hist: 
  if (false == m_isAnaHist) {   
    // wait until data has arrived
    int m_serviceSize = getSize()/sizeof(float);
    while (m_serviceSize <= 0) {
      gSystem->Sleep(m_waitTime);
      m_serviceSize = getSize()/sizeof(float);
    }
    if (-1.0 != getFloat()) {
      m_histoDimData = (float*) getData();
    
      if (H1D == m_hstype) {  // H1D 
        const int   nBins   = (int) m_histoDimData[1];
        const float xMin    = m_histoDimData[2];
        const float xMax    = m_histoDimData[3];
        //    const int   entries = (int) m_histoDimData[4]; 
        if (!rootHistogram) {
          rootHistogram = new TH1F(m_histoRootName.Data(), m_hname.data(),
              nBins, xMin, xMax);
          rootHistogram->SetBit(kNoContextMenu);
        }
      } else if (H2D == m_hstype) { //"H2D"  
        const int   nBinsX   = (int) m_histoDimData[1];
        const float xMin     = m_histoDimData[2];
        const float xMax     = m_histoDimData[3];
        const int   nBinsY   = (int) m_histoDimData[4];
        const float yMin     = m_histoDimData[5];
        const float yMax     = m_histoDimData[6];
        //    const float entries  = m_histoDimData[7];  
    
        if (!rootHistogram) {
          rootHistogram = new TH2F(m_histoRootName.Data(), m_hname.data(),
              nBinsX, xMin, xMax,
              nBinsY, yMin, yMax);
          rootHistogram->SetBit(kNoContextMenu);    
  //        if (!histogramImage) {
  //          histogramImage = TImage::Create();
  //          histogramImage->SetImage((const Double_t *)((TH2F*)rootHistogram)->GetArray(),
  //              ((TH2F*)rootHistogram)->GetNbinsX() + 2,
  //              ((TH2F*)rootHistogram)->GetNbinsY() + 2); // , gHistImagePalette
  //        }
        }
      } else if (P1D == m_hstype ||
                 "HPD" == m_hstype) {  // P1D
        const int   nBins   = (int) m_histoDimData[1];
        const float xMin    = m_histoDimData[2];
        const float xMax    = m_histoDimData[3];
        //    const int   entries = (int) m_histoDimData[4];
        //    float *entriesPerBin;
        //    float *sumWTPerBin;
        //    float *sumWT2PerBin;     
        if (!rootHistogram) {
          rootHistogram = new TH1F(m_histoRootName.Data(), m_hname.data(),
              nBins, xMin, xMax);
          rootHistogram->SetBit(kNoContextMenu);                                          	
        }
    
      } else if (OnlineHistDBEnv_constants::HistTypeName[3] == m_hstype ) { //"P2D"  
      } else if (OnlineHistDBEnv_constants::HistTypeName[4] == m_hstype) { //"CNT"    
      }
      if(rootHistogram) {
        setrootHist(rootHistogram);
      }
    }
  } else {
    for (unsigned int i=0; i< m_AnaSources.size(); ++i) { 
      m_AnaSources[i]->initHistogram();
    }
  }
}

void DbRootHist::fillHistogram() {
  
  if (false == m_isAnaHist) {

    // TODO: rebin: nBins nBinsX nBinsY initHistogram
    // thread, fill...
    if (m_toRefresh) {    
      if (rootHistogram) {
        rootHistogram->Reset();
      }  
    
      // wait until data has arrived
      int m_serviceSize = getSize()/sizeof(float);
      while (m_serviceSize <= 0) {
        gSystem->Sleep(m_waitTime);
        m_serviceSize = getSize()/sizeof(float);
      }
    
      if (-1.0 != getFloat()) {
        m_histoDimData = (float*) getData();
      
        if (H1D == m_hstype) {  // H1D 
          const int   nBins   = (int) m_histoDimData[1];
          //    const float xMin    = m_histoDimData[2];
          //    const float xMax    = m_histoDimData[3];
          const int   entries = (int) m_histoDimData[4];
      
          // fill histogram
          int offsetData  = 5;
          int offsetError = 5+nBins+1;  
          // N.B. bin 0: underflow, bin nBins+1 overflow
      
          // set underflows and overflows:
          rootHistogram->SetBinContent(0, m_histoDimData[5]);
          rootHistogram->SetBinContent(nBins+1, m_histoDimData[5+nBins+1]);
      
          for (int i = 1; i <= nBins; ++i) {
            rootHistogram->SetBinContent(i, m_histoDimData[offsetData+i]);
            rootHistogram->SetBinError(i, m_histoDimData[offsetError+i]);
          }
          rootHistogram->SetEntries(entries);
      
        } else if (H2D == m_hstype) { //"H2D"
      
          const int   nBinsX   = (int) m_histoDimData[1];
          //        const float xMin     = m_histoDimData[2];
          //        const float xMax     = m_histoDimData[3];
          const int   nBinsY   = (int) m_histoDimData[4];
          //        const float yMin     = m_histoDimData[5];
          //        const float yMax     = m_histoDimData[6];
          const float entries  = m_histoDimData[7];  
      
          int   iData = 8;  //current position in stream
          float data  = 0;  
          for (int i=0; i<= nBinsX+1; ++i) {
            for (int j=0; j <= nBinsY+1; ++j) {
              data = (float) m_histoDimData[iData];      
              rootHistogram->SetBinContent(i,j,data);
              iData ++;
            } //for j    
          } //for i
      
          for (int i=0; i<= nBinsX+1; ++i) {
            for (int j=0; j <= nBinsY+1; ++j) {
              data = (float) m_histoDimData[iData];      
              rootHistogram->SetBinError(i,j,data);
              iData ++;
            } //for j    
          } //for i
      
          rootHistogram->SetEntries(entries);
    //      histogramImage->SetImage((const Double_t *)((TH2F*)rootHistogram)->GetArray(),
    //          ((TH2F*)rootHistogram)->GetNbinsX() + 2,
    //          ((TH2F*)rootHistogram)->GetNbinsY() + 2); // , gHistImagePalette
      
        } else if (P1D == m_hstype ||
                 "HPD" == m_hstype) {  // P1D
          const int   nBins   = (int) m_histoDimData[1];
          //    const float xMin    = m_histoDimData[2];
          //    const float xMax    = m_histoDimData[3];
          const int   entries = (int) m_histoDimData[4];
          float *entriesPerBin;
          float *sumWTPerBin;
          float *sumWT2PerBin;
      
          int offsetData  = 5;
          int offsetError = 5+nBins+1;  
          // N.B. bin 0: underflow, bin nBins+1 overflow
      
          const int offsetEntries = 5;
          const int offsetWT      = 5 + nBins + 2;
          const int offsetWT2     = 5 + nBins+2 + nBins + 2;
      
          entriesPerBin = &m_histoDimData[offsetEntries];
          sumWTPerBin  = &m_histoDimData[offsetWT];
          sumWT2PerBin  = &m_histoDimData[offsetWT2];
      
          float yvalue = 0; 
          float yerr = 0;
      
          // bin 0: underflow, nBins+1 overflow ?
          for (int i = 0; i <= nBins+2; i++) {
            yvalue = 0;    
            if (entriesPerBin[i] > 0)
              yvalue = sumWTPerBin[i]/entriesPerBin[i];  // mean in Y    
            rootHistogram->SetBinContent(i, yvalue);    
      
            yerr = 0;
            if (entriesPerBin[i] > 0)
              yerr = TMath::Sqrt(sumWT2PerBin[i]/entriesPerBin[i]-yvalue*yvalue);  // RMS = sqrt(E[x**2]-E[x]**2)
            rootHistogram->SetBinError(i, yerr);
      
          }
          // set underflows and overflows:
          rootHistogram->SetBinContent(0, m_histoDimData[5]);
          rootHistogram->SetBinContent(nBins+1 , m_histoDimData[5+nBins+1]);
          for (int i = 1; i <= nBins; ++i) {
            rootHistogram->SetBinContent(i, m_histoDimData[offsetData+i]);
            rootHistogram->SetBinError(i, m_histoDimData[offsetError+i]);
          }
          rootHistogram->SetEntries(entries);
        } else if (OnlineHistDBEnv_constants::HistTypeName[3] == m_hstype ) { //"P2D"
      
        } else if (OnlineHistDBEnv_constants::HistTypeName[4] == m_hstype) { //"CNT"
      
        }
      
        if (m_cleared && m_offsetHistogram) {    
          //    rootHistogram->Add(m_offsetHistogram,-1.0); -- does not reset errors
          if (H1D == m_hstype ||
              P1D == m_hstype || HPD == m_hstype) {
            for (int i = 1; i <= rootHistogram->GetNbinsX(); ++i) {
              rootHistogram->SetBinContent(i,rootHistogram->GetBinContent(i) -
                  m_offsetHistogram->GetBinContent(i));
              rootHistogram->SetBinError(i,sqrt( pow(rootHistogram->GetBinError(i),2) -
                    pow(m_offsetHistogram->GetBinError(i),2) ));
            }
          } else if (H2D == m_hstype) {
            for (int i=1; i<= rootHistogram->GetNbinsX() ; ++i) {
              for (int j=1; j <= rootHistogram->GetNbinsY() ; ++j) {
                rootHistogram->SetBinContent(i,j,rootHistogram->GetBinContent(i,j) -
                    m_offsetHistogram->GetBinContent(i,j));
                rootHistogram->SetBinError(i,j, sqrt( pow(rootHistogram->GetBinError(i,j),2) -
                      pow(m_offsetHistogram->GetBinError(i,j),2) ) );
              }
            }
          }
        }      
        if (hostingPad) {
          hostingPad->Modified();
        }
      }
      m_toRefresh = false;
    }
  } else if (m_AnaSources.size()>0)  {
    std::vector<TH1*> sources(m_AnaSources.size());
    for (unsigned int i=0; i< m_AnaSources.size(); ++i) {
      m_AnaSources[i]->fillHistogram();
      sources[i]= m_AnaSources[i]->rootHistogram;
    }
    bool init = (NULL == rootHistogram);
    rootHistogram = (dynamic_cast<OMAHcreatorAlg*>(m_analib->getAlg( m_CreationAlgorithm ) ) )
            ->exec(&sources, 
             &m_parameters,
             identifier(), 
             dbHist()->htitle(),
             rootHistogram);
    if (init) { 
      setrootHist(rootHistogram);
    }
    if (hostingPad) {
      hostingPad->Modified();
    }
  } 
}
