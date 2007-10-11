#include <TPad.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TMath.h>
#include <TThread.h>

#include <iostream>
#include <stdlib.h>
#include <malloc.h>


#include "DbRootHist.h"
#include "OnlineHistDB/OnlineRootHist.h"

DbRootHist::DbRootHist(std::string identifier, 
		       std::string dimServiceName,
           int refreshTime,
		       int localInstance,
           OnlineHistDB* histogramDB)
 : OnlineRootHist(identifier, histogramDB, "_NONE_", localInstance),
   DimInfo(dimServiceName.c_str(), refreshTime, -1, this),
   rootHistogram(NULL),
   hostingPad(NULL),
   m_offsetHistogram(NULL),
   m_refreshTime(refreshTime),
   m_histoDimData(NULL),
   m_histoRootName(NULL),
   m_toRefresh(false),
   m_cleared(false),   
   m_hstype(dimServiceName.substr(0,3)),
   m_hname(identifier),   // GG TODO: remove _$ from title
   m_localInstance(localInstance)   
{
  // Prevent ROOT booking
//  TH1::AddDirectory(kFALSE);

  if (NULL != histogramDB) {
   connectToDB(histogramDB, "_NONE_", localInstance);

  }
  // GG: one could use something else (time ?) to have a unique identifier,
  // and forget about instances
  m_histoRootName = new TString(m_hname + Form("__instance__%i", m_localInstance));

  initFromDim();
  fillFromDim();
}

DbRootHist::~DbRootHist() {
  m_toRefresh = false;
  if (m_offsetHistogram) { delete m_offsetHistogram; }
  if (rootHistogram) { delete rootHistogram; }
  if (m_histoRootName) { delete m_histoRootName; }
}



void DbRootHist::enableClear() {
  // TODO: deal with errors and Reset & Add instead of delete/new
  m_cleared = true;
  if (m_offsetHistogram) { delete m_offsetHistogram; }
  if (OnlineHistDBEnv_constants::HistTypeName[0] == m_hstype) { //"H1D"
    TThread::Lock(); 
    m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));
    TThread::UnLock();
//    m_offsetHistogram = (TH1F*)rootHistogram->Clone("m_offsetHistogram");
  } else if (OnlineHistDBEnv_constants::HistTypeName[1] == m_hstype) { //"H2D"
    TThread::Lock();
    m_offsetHistogram = new TH2F(*dynamic_cast<TH2F*>(rootHistogram));
    TThread::UnLock();
  } else if (OnlineHistDBEnv_constants::HistTypeName[2] == m_hstype) {  // P1D
    TThread::Lock();
    m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));
    TThread::UnLock();
  }
}

void DbRootHist::disableClear() {
  m_cleared = false;  
}

void DbRootHist::infoHandler()  {
  if (m_toRefresh) { fillFromDim(); }
}

void DbRootHist::initFromDim() {

  // wait until data has arrived
  int m_serviceSize = getSize()/sizeof(float);
  while (m_serviceSize <= 0) {
    int m_waitTime = 20; // (double)(5)*1e5;
    gSystem->Sleep(m_waitTime);
//        
    m_serviceSize = getSize()/sizeof(float);
  }
  
  m_histoDimData = (float*) getData();
  
  if (OnlineHistDBEnv_constants::HistTypeName[0] == m_hstype) {  // H1D 
    const int   nBins   = (int) m_histoDimData[1];
    const float xMin    = m_histoDimData[2];
    const float xMax    = m_histoDimData[3];
//    const int   entries = (int) m_histoDimData[4]; 
    if (!rootHistogram) {
      TThread::Lock();
      rootHistogram = new TH1F(m_histoRootName->Data(), m_hname.data(),
				 nBins, xMin, xMax);
      TThread::UnLock();         
//      rootHistogram->SetDirectory(0);
    }
  } else if (OnlineHistDBEnv_constants::HistTypeName[1] == m_hstype) { //"H2D"
    
    const int   nBinsX   = (int) m_histoDimData[1];
    const float xMin     = m_histoDimData[2];
    const float xMax     = m_histoDimData[3];
    const int   nBinsY   = (int) m_histoDimData[4];
    const float yMin     = m_histoDimData[5];
    const float yMax     = m_histoDimData[6];
//    const float entries  = m_histoDimData[7];  
    
    if (!rootHistogram) {
      TThread::Lock();
      rootHistogram = new TH2F(m_histoRootName->Data(), m_hname.data(),
				 nBinsX, xMin, xMax,
				 nBinsY, yMin, yMax);
      TThread::UnLock();
//      rootHistogram->SetDirectory(0);
    }
  } else if (OnlineHistDBEnv_constants::HistTypeName[2] == m_hstype) {  // P1D
    const int   nBins   = (int) m_histoDimData[1];
    const float xMin    = m_histoDimData[2];
    const float xMax    = m_histoDimData[3];
//    const int   entries = (int) m_histoDimData[4];
//    float *entriesPerBin;
//    float *sumWTPerBin;
//    float *sumWT2PerBin;     
    if (!rootHistogram) {
      TThread::Lock();
      rootHistogram = new TH1F(m_histoRootName->Data(), m_hname.data(),
				 nBins, xMin, xMax);
      TThread::UnLock();                                       
//      rootHistogram->SetDirectory(0);	
    }
    
  } else if (OnlineHistDBEnv_constants::HistTypeName[3] == m_hstype ) { //"P2D"  
  } else if (OnlineHistDBEnv_constants::HistTypeName[4] == m_hstype) { //"CNT"    
  }
  if(rootHistogram) {
   setrootHist(rootHistogram);
  }
}
//
void DbRootHist::fillFromDim() {
  // TODO: rebin: nBins nBinsX nBinsY initFromDim
  // thread, fill...
  if (rootHistogram) {
    TThread::Lock();
    rootHistogram->Reset();
    TThread::UnLock();
  }  

  // wait until data has arrived
  int m_serviceSize = getSize()/sizeof(float);
  while (m_serviceSize <= 0) {
    int m_waitTime = 20; // (double)(5)*1e5;
    gSystem->Sleep(m_waitTime);
    m_serviceSize = getSize()/sizeof(float);
  }
  
  m_histoDimData = (float*) getData();
  
   
    if (OnlineHistDBEnv_constants::HistTypeName[0] == m_hstype) {  // H1D 
      const int   nBins   = (int) m_histoDimData[1];
  //    const float xMin    = m_histoDimData[2];
  //    const float xMax    = m_histoDimData[3];
      const int   entries = (int) m_histoDimData[4];
      
      // fill histogram
      int offsetData  = 5;
      int offsetError = 5+nBins+1;  
      // N.B. bin 0: underflow, bin nBins+1 overflow
    
      // set underflows and overflows:
      TThread::Lock();
      rootHistogram->SetBinContent(0       , m_histoDimData[5]);
      rootHistogram->SetBinContent(nBins+1 , m_histoDimData[5+nBins+1]);
      
      for (int i = 1; i <= nBins; ++i) {
        rootHistogram->SetBinContent(i, m_histoDimData[offsetData+i]);
        rootHistogram->SetBinError(i,m_histoDimData[offsetError+i]);
      }
      rootHistogram->SetEntries(entries);
      TThread::UnLock();
   
    } else if (OnlineHistDBEnv_constants::HistTypeName[1] == m_hstype) { //"H2D"
  
      const int   nBinsX   = (int) m_histoDimData[1];
//        const float xMin     = m_histoDimData[2];
//        const float xMax     = m_histoDimData[3];
      const int   nBinsY   = (int) m_histoDimData[4];
//        const float yMin     = m_histoDimData[5];
//        const float yMax     = m_histoDimData[6];
      const float entries  = m_histoDimData[7];  
        
  
      int   iData = 8;  //current position in stream
      float data  = 0;  
      TThread::Lock();
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
      TThread::UnLock();
    
  } else if (OnlineHistDBEnv_constants::HistTypeName[2] == m_hstype) {  // P1D
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
    
    TThread::Lock();
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
    TThread::UnLock();
  } else if (OnlineHistDBEnv_constants::HistTypeName[3] == m_hstype ) { //"P2D"
  
  } else if (OnlineHistDBEnv_constants::HistTypeName[4] == m_hstype) { //"CNT"
    
  }
  
  if (m_cleared && m_offsetHistogram) {    
//    rootHistogram->Add(m_offsetHistogram,-1.0); -- does not reset errors
    if (OnlineHistDBEnv_constants::HistTypeName[0] == m_hstype) {
      TThread::Lock();
      for (int i = 1; i <= rootHistogram->GetNbinsX(); ++i) {
      	rootHistogram->SetBinContent(i,rootHistogram->GetBinContent(i) -
      				     m_offsetHistogram->GetBinContent(i));
      	rootHistogram->SetBinError(i,sqrt( pow(rootHistogram->GetBinError(i),2) -
      					   pow(m_offsetHistogram->GetBinError(i),2) ));
      }
      TThread::UnLock();    
    } else if (OnlineHistDBEnv_constants::HistTypeName[1] == m_hstype) {
      TThread::Lock();
      for (int i=1; i<= rootHistogram->GetNbinsX() ; ++i) {
      	for (int j=1; j <= rootHistogram->GetNbinsY() ; ++j) {
      	  rootHistogram->SetBinContent(i,j,rootHistogram->GetBinContent(i,j) -
      				       m_offsetHistogram->GetBinContent(i,j));
      	  rootHistogram->SetBinError(i,j, sqrt( pow(rootHistogram->GetBinError(i,j),2) -
      						pow(m_offsetHistogram->GetBinError(i,j),2) ) );
      	}
      }
      TThread::UnLock();
    }
  }

  if (hostingPad) {
    hostingPad->Modified();
  }
}
