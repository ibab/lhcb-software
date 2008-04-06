// TH1::SetMaximum() and TH1::SetMinimum() 

#include <TPad.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TMath.h>

#include <TFile.h>
#include <TPaveStats.h>
#include <TCanvas.h>

#include <TThread.h>

//#include <TImage.h>
//#include <TGMsgBox.h>

#include <iostream>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>

#include "presenter.h"
#include "DbRootHist.h"

using namespace pres;

DbRootHist::DbRootHist(const std::string & identifier,
                       const std::string & dimServiceName,
                       int refreshTime,
                       int instance,
                       OnlineHistDB* histogramDB,
                       OMAlib* analysisLib,
                       OnlineHistogram* onlineHist)
: HistogramIdentifier(dimServiceName),
  rootHistogram(NULL),
  hostingPad(NULL),
//  histogramImage(NULL),
  m_gauchocommentDimInfo(NULL),
  m_offsetHistogram(NULL),
  m_isAnaHist(false),
  m_anaLoaded(false),
  m_analysisLib(analysisLib),
  m_refreshTime(refreshTime),
  m_histoDimData(NULL),
//  m_toRefresh(false),
  m_cleared(false),
  m_hstype(dimServiceName.substr(0, 3)),
  m_hname(m_histogramName),
  m_instance(instance),
  m_waitTime(20),
  m_msgBoxReturnCode(0),
  m_identifier(identifier),
  m_session(histogramDB),
  m_onlineHistogram(NULL),
  m_isEmptyHisto(false),
  m_refOption("NOREF"),
  m_reference(NULL),
  m_startRun(1),
  m_dataType("default"),  
  m_dimServiceName(dimServiceName)
{
  // Prevent ROOT booking
  TH1::AddDirectory(kFALSE);
  m_dimInfo = new DimInfo(dimServiceName.c_str(), refreshTime, (float)-1.0);

  if (histogramDB) {
    if (onlineHist) { setOnlineHistogram(onlineHist); }
    else { connectToDB(histogramDB, "_NONE_", 1); }
  }

  initHistogram();
  fillHistogram();

  setTH1FromDB();
}
DbRootHist::~DbRootHist()
{
//  m_toRefresh = false;
  if (m_offsetHistogram) { delete m_offsetHistogram; m_offsetHistogram = NULL;}
  if (rootHistogram) { delete rootHistogram; rootHistogram = NULL; }
  if (m_reference) { delete  m_reference; m_reference = NULL; }
//  if (histogramImage) { delete histogramImage; histogramImage = NULL; }
  if (hostingPad) { delete hostingPad; hostingPad = NULL; }
  if (m_dimInfo) { delete m_dimInfo; m_dimInfo = NULL; }
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
  if (onlineHistogram()) {
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
                                              histo->dimServiceName(),
                                              m_refreshTime,
                                              999,
                                              dbSession(),
                                              m_analysisLib,
                                              histo));
        if (NULL == m_anaSources[i]->rootHistogram) { sourcesOK = false; }
      }
      m_anaLoaded = true;
      if (!sourcesOK) { // cannot get sources from DIM
        beEmptyHisto();
        cleanAnaSources();
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
// std::cout << "m_identifier " <<m_identifier << std::endl;
// std::cout << "histogramIdentifier " <<HistogramIdentifier(newDimServiceName).histogramIdentifier()<< std::endl;
  if (HistogramIdentifier(newDimServiceName)
      .histogramIdentifier() == m_identifier) {
    if (m_dimInfo) { delete m_dimInfo; m_dimInfo = NULL; }
    m_dimInfo = new DimInfo(newDimServiceName.c_str(), m_refreshTime,
                            (float)-1.0);
    m_dimServiceName = newDimServiceName;
    initHistogram();
    fillHistogram();
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
      if (s_H1D == m_hstype) {
        m_offsetHistogram = new TH1D(*dynamic_cast<TH1D*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
//    m_offsetHistogram = (TH1D*)rootHistogram->Clone("m_offsetHistogram");
      } else if (s_H2D == m_hstype) {
        m_offsetHistogram = new TH2D(*dynamic_cast<TH2D*>(rootHistogram));
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"
      } else if (s_P1D == m_hstype || s_HPD == m_hstype) {
        m_offsetHistogram = new TH1F(*dynamic_cast<TH1F*>(rootHistogram));        
        m_offsetHistogram->SetBit(kNoContextMenu);
//      m_offsetHistogram->Reset(); //"ICE: Integral Contents, Errors"  
      }
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
//void DbRootHist::infoHandler()
//{
//  TThread::Lock();
////  m_toRefresh = true;
//  TThread::UnLock();
//}
void DbRootHist::initHistogram()
{
  // If not AnaLib hist:
  if (!m_isAnaHist) {
    std::string noGauchocomment = "No gauchocomment";
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
                                   histogramName().c_str(),
                                   m_instance));
    m_histoRootTitle = TString(Form("%s",
                                    m_gauchocommentDimInfo->getString()));
    if (m_gauchocommentDimInfo) {
      delete m_gauchocommentDimInfo;
      m_gauchocommentDimInfo = NULL;
    }

    // wait until data has arrived
    m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    while (m_serviceSize <= 0) {
      gSystem->Sleep(m_waitTime);
      m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    }
    if (-1.0 != m_dimInfo->getFloat()) {
      m_histoDimData = (float*) m_dimInfo->getData();
      if (s_H1D == m_hstype) {
        const int   nBins   = (int) m_histoDimData[1];
        const double xMin   = (double) m_histoDimData[2];
        const double xMax   = (double) m_histoDimData[3];
//      const int   entries = (int) m_histoDimData[4];
        if (!rootHistogram) {
          rootHistogram = new TH1D(m_histoRootName.Data(),
                                   m_histoRootTitle.Data(),
                                   nBins, xMin, xMax);
        }
      } else if (s_H2D == m_hstype) {
        const int   nBinsX   = (int) m_histoDimData[1];
        const double xMin    = (double) m_histoDimData[2];
        const double xMax    = (double) m_histoDimData[3];
        const int   nBinsY   = (int) m_histoDimData[4];
        const double yMin    = (double) m_histoDimData[5];
        const double yMax    = (double) m_histoDimData[6];
//      const float entries  = m_histoDimData[7];
        if (!rootHistogram) {
          rootHistogram = new TH2D(m_histoRootName.Data(),
                                   m_histoRootTitle.Data(),
                                   nBinsX, xMin, xMax,
                                   nBinsY, yMin, yMax);
//        if (!histogramImage) {
//          histogramImage = TImage::Create();
//          histogramImage->SetImage((const Double_t *)((TH2F*)rootHistogram)->GetArray(),
//              ((TH2F*)rootHistogram)->GetNbinsX() + 2,
//              ((TH2F*)rootHistogram)->GetNbinsY() + 2); // , gHistImagePalette
//        }
        }
      } else if (s_P1D == m_hstype || s_HPD == m_hstype) {
        const int   nBins   = (int) m_histoDimData[1];
        const float xMin    = m_histoDimData[2];
        const float xMax    = m_histoDimData[3];
//      const int   entries = (int) m_histoDimData[4];
//      float* entriesPerBin;
//      float* sumWTPerBin;
//      float* sumWT2PerBin;
        if (!rootHistogram) {
          rootHistogram = new TH1F(m_histoRootName.Data(),
                                       m_histoRootTitle.Data(),
                                       nBins, xMin, xMax);
        }
      } else if (s_P2D == m_hstype) {
      } else if (s_CNT == m_hstype) {
      }
    } else {
      // cannot get sources from DIM
      beEmptyHisto();
    }
  } else if (m_isAnaHist && m_anaSources.size() > 0) {
    std::vector<TH1*> sources(m_anaSources.size());
    for (unsigned int i=0; i< m_anaSources.size(); ++i) {
      m_anaSources[i]->initHistogram();
      sources[i]= m_anaSources[i]->rootHistogram;
    }
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
                 (m_analysisLib->getAlg(m_creationAlgorithm));
    if(creator)
      rootHistogram = creator->exec(&sources, &m_parameters,
                                    identifier(),
                                    onlineHistogram()->htitle(),
                                    rootHistogram);
    if (!rootHistogram) { beEmptyHisto(); }
  }
  rootHistogram->SetBit(kNoContextMenu);
}
void DbRootHist::beEmptyHisto()
{
  m_isEmptyHisto = true;
  if (rootHistogram) { delete rootHistogram; rootHistogram = 0; }
  std::string dummyTitle = "ERROR: missing sources for ";
  dummyTitle += identifier();
  if (s_H1D == m_hstype) {
    rootHistogram = new TH1D(m_histoRootName.Data(),
                             dummyTitle.c_str(),
                             1, 0., 1.);
  } else if (s_H2D == m_hstype) {
    rootHistogram = new TH2D(m_histoRootName.Data(),
                             dummyTitle.c_str(),
                             1, 0., 1.,
                             1, 0., 1.);
  } else if (s_P1D == m_hstype || s_HPD == m_hstype) {
    rootHistogram = new TH1F(m_histoRootName.Data(),
                                 dummyTitle.c_str(),
                                 1, 0., 1.);
  } else if (s_P2D == m_hstype) {
  } else if (s_CNT == m_hstype) {
  }
  if(rootHistogram) {
    rootHistogram->SetBit(kNoContextMenu);
    setRootHistogram(rootHistogram);
  }
}

void DbRootHist::fillHistogram()
{
  // TODO: rebin: nBins nBinsX nBinsY initHistogram
//  if (!m_isAnaHist && m_toRefresh) {
  if (rootHistogram) {
    rootHistogram->Reset();
  }
  if (!m_isAnaHist) {
    // wait until data has arrived
    int m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    while (m_serviceSize <= 0) {
      gSystem->Sleep(m_waitTime);
      m_serviceSize = m_dimInfo->getSize()/sizeof(float);
    }
    if (-1.0 != m_dimInfo->getFloat()) {
      m_histoDimData = (float*) m_dimInfo->getData();
      if (s_H1D == m_hstype) {
        const int   nBins   = (int) m_histoDimData[1];
//        const float xMin    = m_histoDimData[2];
//        const float xMax    = m_histoDimData[3];
        const int   entries = (int) m_histoDimData[4];

        // fill histogram
        int offsetData  = 5;
        int offsetError = 5+nBins+1;
        // N.B. bin 0: underflow, bin nBins+1 overflow
        rootHistogram->SetBinContent(0, (double) m_histoDimData[5]);
        rootHistogram->SetBinContent(nBins+1,
                                     (double) m_histoDimData[5 + nBins +
                                                             1]);
        for (int i = 1; i <= nBins; ++i) {
          rootHistogram->SetBinContent(i,
                                       (double) m_histoDimData[offsetData +
                                                               i]);
          rootHistogram->SetBinError(i, (double) m_histoDimData[
                                     offsetError + i]);
        }
        rootHistogram->SetEntries(entries);
      } else if (s_H2D == m_hstype) {
        const int   nBinsX   = (int) m_histoDimData[1];
//        const float xMin     = m_histoDimData[2];
//        const float xMax     = m_histoDimData[3];
        const int   nBinsY   = (int) m_histoDimData[4];
//        const float yMin     = m_histoDimData[5];
//        const float yMax     = m_histoDimData[6];
        const double entries  = (double) m_histoDimData[7];

        int   iData = 8;  //current position in stream
        float data  = 0;
        for (int i=0; i<= nBinsX+1; ++i) {
          for (int j=0; j <= nBinsY+1; ++j) {
            data = (float) m_histoDimData[iData];
            rootHistogram->SetBinContent(i, j, (double) data);
            iData ++;
          }
        }

        for (int i=0; i<= nBinsX+1; ++i) {
          for (int j=0; j <= nBinsY+1; ++j) {
            data = (float) m_histoDimData[iData];
            rootHistogram->SetBinError(i, j, (double) data);
            iData ++;
          }
        }

        rootHistogram->SetEntries(entries);
//      histogramImage->SetImage((const Double_t *)((TH2F*)rootHistogram)->GetArray(),
//          ((TH2F*)rootHistogram)->GetNbinsX() + 2,
//          ((TH2F*)rootHistogram)->GetNbinsY() + 2); // , gHistImagePalette

      } else if (s_P1D == m_hstype || s_HPD == m_hstype) {
//        rootHistogram->Reset();
        const int   nBins   = (int) m_histoDimData[1];
//        const float xMin    = m_histoDimData[2];
//        const float xMax    = m_histoDimData[3];
        const int   entries = (int) m_histoDimData[4];
        float *entriesPerBin;
        float *sumWTPerBin;
        float *sumWT2PerBin;

        const int offsetEntries = 5;
        const int offsetWT      = 5 + nBins+2;
        const int offsetWT2     = 5 + nBins+2 + nBins+2;
      
        entriesPerBin = &m_histoDimData[offsetEntries];
        sumWTPerBin  = &m_histoDimData[offsetWT];
        sumWT2PerBin  = &m_histoDimData[offsetWT2];

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
      } else if (s_P2D == m_hstype) {
      } else if (s_CNT == m_hstype) {
      }

      if (m_cleared && m_offsetHistogram) {
//        rootHistogram->Add(m_offsetHistogram,-1.0); - does not reset errors
        if (s_H1D == m_hstype ||
            s_P1D == m_hstype || s_HPD == m_hstype) {
          for (int i = 1; i <= rootHistogram->GetNbinsX(); ++i) {
            rootHistogram->SetBinContent(i, rootHistogram->GetBinContent(i)
              - m_offsetHistogram->GetBinContent(i));
            rootHistogram->SetBinError(i, sqrt(pow(rootHistogram->
              GetBinError(i), 2) - pow(m_offsetHistogram->
              GetBinError(i), 2)));
          }
        } else if (s_H2D == m_hstype) {
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
      if (hostingPad) { hostingPad->Modified(); }
    }
//    m_toRefresh = false;
  } else if (m_isAnaHist && m_anaSources.size()>0)  {
    std::vector<TH1*> sources(m_anaSources.size());
    for (unsigned int i=0; i< m_anaSources.size(); ++i) {
      m_anaSources[i]->fillHistogram();
      sources[i]= m_anaSources[i]->rootHistogram;
    }
    OMAHcreatorAlg* creator = dynamic_cast<OMAHcreatorAlg*>
      (m_analysisLib->getAlg(m_creationAlgorithm));
    if (creator) {
      rootHistogram = creator->exec(&sources, &m_parameters, identifier(),
                                    onlineHistogram()->htitle(),
                                    rootHistogram);
    }
    if (!rootHistogram) { beEmptyHisto(); }
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
        m_hstype = newOnlineHistogram->hstype();
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
  if (m_onlineHistogram && rootHistogram) {
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
    double bxmin=rootHistogram->GetXaxis()->GetXmin();
    double bxmax=rootHistogram->GetXaxis()->GetXmax();
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
      double bymin = rootHistogram->GetYaxis()->GetXmin();
      double bymax=rootHistogram->GetYaxis()->GetXmax();
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
    rootHistogram->SetStats(true);
    if (m_onlineHistogram->getDisplayOption("STATS", &iopt)) {
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
  }
}
// TODO: should rather use hostingPad
void DbRootHist::setDrawOptionsFromDB(TPad* &pad)
{
  if (m_onlineHistogram && rootHistogram) {
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt("");

    // TPaveStats is obtained after a pad->Draw(), but changing OptStat
    // doesn't resize the Pave.. using gStyle->SetOptStat in Draw() instead
    // if (m_dbHist->getDisplayOption("STATS", &iopt)) {
    //   TPaveStats* stats =  (TPaveStats*)rootHistogram->GetListOfFunctions()->FindObject("stats");
    //   if (stats) stats->SetOptStat(iopt);

    if (m_onlineHistogram->getDisplayOption("DRAWOPTS", &sopt)) {
      rootHistogram->SetDrawOption(sopt.c_str());
    }
    if (m_onlineHistogram->getDisplayOption("LOGX", &iopt)) {
      pad->SetLogx(1);
    }
    if (m_onlineHistogram->getDisplayOption("LOGY", &iopt)) {
      pad->SetLogy(1);
    }
    int gridx = gStyle->GetPadGridX();
    int gridy = gStyle->GetPadGridY();
    if (m_onlineHistogram->getDisplayOption("GRIDX", &iopt)) { gridx=iopt; }
    if (m_onlineHistogram->getDisplayOption("GRIDY", &iopt)) { gridy=iopt; }
    pad->SetGrid(gridx, gridy);
    if (m_onlineHistogram->dimension() > 1) {
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
  }
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
    if ( m_onlineHistogram->isSetDisplayOption(opt) ) {
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
    // note: axis mimina and maxima should not be set in this way, but
    // through the web interface

    TPaveStats* stats =  (TPaveStats*)rootHistogram->GetListOfFunctions()
                                                      ->FindObject("stats");
    // if histogram has not been plotted (or has been plotted without stats),
    // do nothing
    if (stats) {
      iopt = (int) stats->GetOptStat();
      // 111110 seems to be hardcoded in root
      out |= updateDBOption("STATS", &iopt, iopt == 111110 );
    }
    
    sopt = m_refOption;
    out |= updateDBOption("REF", &sopt, sopt.empty());
    
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

    sopt = rootHistogram->GetDrawOption();
    if (sopt.empty()) { sopt = rootHistogram->GetOption(); }
    out |= updateDBOption("DRAWOPTS",  &sopt, sopt.empty());

    fopt = rootHistogram->GetXaxis()->GetTitleSize();
    out |= updateDBOption("TIT_X_SIZE", &fopt,
                          fopt == (float) gStyle->GetTitleSize("X"));
    fopt = rootHistogram->GetXaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_X_OFFS", &fopt,
                          fopt == (float) gStyle->GetTitleOffset("X"));
    fopt = rootHistogram->GetXaxis()->GetLabelSize();
    out |= updateDBOption("LAB_X_SIZE", &fopt,
         fopt == (float) gStyle->GetLabelSize("X"));
    fopt = rootHistogram->GetXaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_X_OFFS", &fopt,
                          fopt == (float) gStyle->GetLabelOffset("X"));
    fopt = rootHistogram->GetYaxis()->GetTitleSize();
    out |= updateDBOption("TIT_Y_SIZE", &fopt,
                          fopt == (float) gStyle->GetTitleSize("Y"));
    fopt = rootHistogram->GetYaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_Y_OFFS", &fopt,
                          fopt == (float) gStyle->GetTitleOffset("Y"));
    fopt = rootHistogram->GetYaxis()->GetLabelSize();
    out |= updateDBOption("LAB_Y_SIZE", &fopt,
                          fopt == (float) gStyle->GetLabelSize("Y"));
    fopt = rootHistogram->GetYaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_Y_OFFS", &fopt,
                          fopt == (float) gStyle->GetLabelOffset("Y"));

    if (m_onlineHistogram->dimension() > 1) {
      sopt = rootHistogram->GetZaxis()->GetTitle();
      out |= updateDBOption("LABEL_Z", &sopt, sopt.empty());
      fopt = rootHistogram->GetZaxis()->GetTitleSize();
      out |= updateDBOption("TIT_Z_SIZE",  &fopt,
                            fopt == (float) gStyle->GetTitleSize("Z"));
      fopt = rootHistogram->GetZaxis()->GetTitleOffset();
      out |= updateDBOption("TIT_Z_OFFS",  &fopt,
                            fopt == (float) gStyle->GetTitleOffset("Z"));
      fopt = rootHistogram->GetZaxis()->GetLabelSize();
      out |= updateDBOption("LAB_Z_SIZE",  &fopt,
                            fopt == (float) gStyle->GetLabelSize("Z"));
      fopt = rootHistogram->GetZaxis()->GetLabelOffset();
      out |= updateDBOption("LAB_Z_OFFS",  &fopt,
                            fopt == (float) gStyle->GetLabelOffset("Z"));
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

void DbRootHist::Draw(TPad* &pad)
{
  if (rootHistogram) {
    int curStat = 0;
    int iopt = 0;
    if (m_onlineHistogram) {
      if (m_onlineHistogram->getDisplayOption("STATS", &iopt)) {
        if (0 != iopt) {
          curStat = gStyle->GetOptStat();
          gStyle->SetOptStat(iopt);
        }
      } else {
        gStyle->SetOptStat("emrou"); // seems to be hardcoded in root
      }
    }
    rootHistogram->Draw();
    setDrawOptionsFromDB(pad);

    if (s_NoReference != m_refOption) {
//      if (getReference()) { drawReference(); }
    }
  }
}

void DbRootHist::normalizeReference()
{
  if (m_reference) {
    //TODO: normFactor goes negative if refhisto "heavier" than cleared histo
    double normFactor = 0.0;
    if (s_Entries == m_refOption) {
      normFactor = rootHistogram->GetSumOfWeights();
      if (m_cleared && m_offsetHistogram) {
        normFactor -= m_offsetHistogram->GetSumOfWeights();
      }
      m_reference->SetNormFactor(normFactor);
    } else if (s_Area == m_refOption) {
      normFactor = rootHistogram->Integral();
      if (m_cleared && m_offsetHistogram) {
        normFactor -= m_offsetHistogram->Integral();
      }
      m_reference->SetNormFactor(normFactor);
    }
  }
}
void DbRootHist::referenceHistogram(ReferenceVisibility visibility)
{
  if (0 == m_reference &&
      s_NoReference != m_refOption) {
    TFile* f = new TFile(m_onlineHistogram->getTask()->reference().c_str(),
                         "READ");
    if (f) {
      if (false == f->IsZombie()) {
        TH1* ref = (TH1*) f->Get(m_onlineHistogram->hname().c_str());
        if (ref) {
          if (m_reference) { delete m_reference; m_reference = 0; }
          ref->SetDirectory(0);
          m_reference = ref;
        }
        f->Close();
      }
    }
    delete f;
    f = NULL;
  } 
  if (0 != m_reference &&
      Show == visibility) {
    // standard plot style
    m_reference->SetLineStyle(2);
    m_reference->SetLineColor(16);
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
