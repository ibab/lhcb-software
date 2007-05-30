// $Id: DimProxy.cpp,v 1.3 2007-05-30 08:53:49 psomogyi Exp $

// Implementation file for class : DimProxy
// spring cleaning: based on code by J. Helder Lopes & Ulrich Kerzel

#include <iostream>
#include <string>
//#include <stdlib.h>
//#include <malloc.h>

#include <TH1.h>  // ROOT
#include <TH2.h>
#include <TMath.h>

//#include <boost/shared_ptr.hpp>  // BOOST
//#include <boost/tokenizer.hpp>

#include "DimProxy.h"
#include "dic.hxx"

// TODO: Refactor this code to cut down copy-paste look-alikes. Proper polym.
// w subclassing for SVC types... Throw up (exceptions) when DimInfo fails on
// the network.

//using std::string;
//using std::cout;
//using std::endl;
//using boost::shared_ptr;



DimProxy::DimProxy(std::string serviceName = "",
                   int refreshTime = 0,
                   int verbosity   = -1):
  DimInfo(serviceName.c_str(), refreshTime, -1),
  m_serviceType(DimProxy::Unknown),
  m_serviceOK(false),
  m_histoDimension(-1),
  m_serviceSize(0),
  m_serviceName(serviceName),  
  m_serviceUpdated(false),
  m_verbosity(verbosity),
  m_bookedHistogram(false),  
  m_CI4(0),
  m_CF4(0.0),
  m_CD8(0.0),
  m_H1D(NULL),
  m_HPD(NULL),
  m_H2D(NULL)
{
  if (m_verbosity > 0) {    
    std::cout << "* initialsing DIM service " << serviceName << std::endl <<
                 "    refresh time is       " << refreshTime << std::endl;
    }
    
  if (m_serviceName.substr(0,3) == "H1D") {
    m_serviceType = DimProxy::H1D;
    m_histoDimension = 1;
  } else if (m_serviceName.substr(0,3) == "H2D") {
    m_serviceType = DimProxy::H2D;
    m_histoDimension = 2;
  } else if (m_serviceName.substr(0,3) == "HPD") {
    m_serviceType = DimProxy::HPD;
    m_histoDimension = 1;
  } else if (m_serviceName.substr(0,3) == "CI4") {
    m_serviceType = DimProxy::CI4;
    m_histoDimension = 0;
  } else if (m_serviceName.substr(0,3) == "CF4") {
    m_serviceType = DimProxy::CF4;
    m_histoDimension = 0;
  } else if (m_serviceName.substr(0,3) == "CD8") {
    m_serviceType = DimProxy::CD8;
    m_histoDimension = 0;
  } else if (m_serviceName.substr(0,3) == "CC1") {
    m_serviceType = DimProxy::CC1;
    m_histoDimension = 0;
  } else {
    std::cout << "DimProxy::DimProxy cannot identify " <<
                 "DIM SVC requested" << std::endl;
  }
  
  // check if service exists
  DimBrowser dimBrowser;
  int nServices = dimBrowser.getServices(m_serviceName.c_str());
  
  if (m_verbosity > 0) {
    std::cout << "# services for this service name " << nServices <<
                 std::endl;
  }
  
  if (0 != nServices)  {  // Server publishes the counter requested.
    m_serviceOK   = true;        
    m_serviceSize = getSize() / sizeof(float);
  
    // assume integer type by default, now check for other types 
    char  *dimService; 
    char  *dimFormat;
    int   dimType;
    
    while (dimType = dimBrowser.getNextService(dimService, dimFormat)) {      
      if (m_verbosity > 0) {        
        std::cout << "DIM service: " << dimService <<
                     " format "      << dimFormat << std::endl;        
      }
  // Set type via dimFormat instead of SVC name: attn. to conflict w/Histos.      
  //      std::string stringFormat = dimFormat;
  //      if ( (stringFormat.find("I",0) != std::string::npos)
  //           || (stringFormat.find("L",0) != std::string::npos))
  //        m_serviceType = DimProxy::CI4;
  //      
  //      if  (stringFormat.find("F",0) != std::string::npos)
  //        m_serviceType = DimProxy::CF4;
  //
  //      if  (stringFormat.find("D",0) != std::string::npos)
  //        m_serviceType = DimProxy::CD8;
  //
  //      if (stringFormat.find("C",0) != std::string::npos)
  //        m_serviceType = DimProxy::CC1;     
    }
  }
  
  if (m_verbosity > 0) {    
    std::cout << " service OK?        " << m_serviceOK      << std::endl <<
                 "  type              " << m_serviceType    << std::endl;
    if (H1D == m_serviceType ||
        H2D == m_serviceType ||
        HPD == m_serviceType) {           
    std::cout << "  size               " << m_serviceSize    << std::endl <<
                 "histogram dimension: " << m_histoDimension << std::endl;
    }                 
  }    
}

DimProxy::~DimProxy()
{  
  if (m_verbosity > 0) { 
    std::cout << "destructor of DimHinfoHisto called" << std::endl;
  }
  
  if (DimProxy::H1D == m_serviceType && m_H1D != NULL ) {
    if (m_verbosity > 1) std::cout << "deleting H1D histogram" << std::endl;
    m_H1D->Delete();    
  } else if (DimProxy::HPD == m_serviceType && m_HPD != NULL ) {
    if (m_verbosity > 1) std::cout << "deleting HPD histogram" << std::endl;
    m_HPD->Delete();
  } else if (DimProxy::H2D == m_serviceType && m_H2D != NULL) {
    if (m_verbosity > 1) std::cout << "deleting H2D histogram" << std::endl;
    m_H2D->Delete();
  }

  if (m_H1D) delete m_H1D;
  if (m_HPD) delete m_HPD;
  if (m_H2D) delete m_H2D;
  
  if (m_verbosity > 1) {
    std::cout << "destructor of DimProxy ends" << std::endl;
  }
}  

// Put here all H*D and C*D checks...
//bool DimProxy::checkIfServiceSupported() {
//
//  int         dimType;
//  std::string serviceType;
//  typedef boost::tokenizer<boost::char_separator<char> > Tokens;
//
//  // only want DIM services
//  if (dimType != DimSERVICE) { 
//    return false;
//  }
//  
//  // we tokenize the DIM SVC name and fill in the list...
//  // H1D/node_Adder_1/Adder/Algorithmname/Mass
//  Tokens tokenedDIMSVC(m_serviceName, slashSeparator);
//  Tokens::iterator dimSVCNameElements = tokenedDIMSVC.begin();
//  serviceType = *dimSVCNameElements;
//  // The first DIM SVC element is the type, so checking against token:
//  if ("H1D" == serviceType ||
//      "H2D" == serviceType ||
//      "HPD" == serviceType) {
//    return true;
//  } else {
//    return false;
//  }       
//}

TH1* DimProxy::rootH1D()
{
  // only do something if the histogram exists
  if (!m_serviceOK) return 0;
  // only operate on H1D histogram
  if (DimProxy::H1D == m_serviceType)  {
    return m_H1D;
  } else {
    return 0;
  }    
}

TH2* DimProxy::rootH2D()
{
  // only do something if the histogram exists
  if (!m_serviceOK) return 0;
  // only operate on H2D histogram
  if (DimProxy::H2D == m_serviceType)  {
    return m_H2D;
  } else {
    return 0;
  }  
}

TH1* DimProxy::rootHPD()
{
  // only do something if the histogram exists
  if (!m_serviceOK) return 0;
  // only operate on HPD histogram
  if (DimProxy::HPD == m_serviceType)  {
    return m_HPD;
  } else {
    return 0;
  }
}

int DimProxy::intCI4()
{  
  // only do something if the histogram exists
  if (!m_serviceOK) return 0;
  // only operate on CI4 histogram
  if (DimProxy::CI4 == m_serviceType)  {
  return m_CI4;
  } else {
    return 0;
  }  
}

float DimProxy::floatCF4()
{
  // only do something if the histogram exists
  if (!m_serviceOK) return 0;
  // only operate on CF4 histogram
  if (DimProxy::CF4 == m_serviceType){
  return  m_CF4;
  } else {
    return 0;
  }
}

double DimProxy::doubleCD8()
{
  if (!m_serviceOK) return 0;
  // only operate on CD8 histogram
  if (DimProxy::CD8 == m_serviceType){
    return m_CD8;
  } else {
    return 0;
  }
}

void DimProxy::fillHistogramH1D()
{
  const int   nBins   = (int) m_histoData[1];
  const float xMin    = m_histoData[2];
  const float xMax    = m_histoData[3];
  const int   entries = (int) m_histoData[4];

  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram) {
    m_bookedHistogram = true;

    if (m_verbosity > 0) {
      std::cout << "DimProxy booked H1D histogram" << std::endl <<      
                   "H1D histo:  #bins:  " << nBins << std::endl <<
                   "      xMin:         " << xMin  << std::endl <<
                   "      xMax:         " << xMax  << std::endl;
    }

    m_H1D = new TH1F(m_serviceName.c_str(), m_serviceName.c_str(),
                             nBins, xMin, xMax);       
//    boost::shared_ptr<TH1> m_H1D(new TH1F(m_serviceName.c_str(),
//                                          m_serviceName.c_str(),
//                                          nBins, xMin, xMax));                                                            
  }

  // fill histogram
  int offsetData  = 5;
  int offsetError = 5 + nBins + 1;  
  // N.B. bin 0: underflow, bin nBins+1 overflow

  // set underflows and overflows:
  m_H1D->SetBinContent(0      , m_histoData[5]);
  m_H1D->SetBinContent(nBins+1, m_histoData[5+nBins+1]);
  for (int i=1; i <= nBins; ++i) {
    m_H1D->SetBinContent(i, m_histoData[offsetData+i]);
    m_H1D->SetBinError(i,   m_histoData[offsetError+i]);
    if (m_verbosity > 1) {
      std::cout << "index "    << i <<
                   " array "   << m_histoData[offsetData+i] <<
                   " +- "      << m_histoData[offsetError+i] <<
                   " histo "   << m_H1D->GetBinContent(i) <<
                   " +- "      << m_H1D->GetBinError(i) << std::endl;
    }    
  }

  // set #entries
  m_H1D->SetEntries(entries);

  if (m_verbosity > 0) {    
    std::cout << "#entries "    << entries <<
                 " from histo " << m_H1D->GetEntries() << std::endl <<
                 "DimProxy::set1DData  <x> " << m_H1D->GetMean(1) <<
                 std::endl;
  }
}

void DimProxy::fillHistogramH2D()
{
  const int  nBinsX  = (int) m_histoData[1];
  const float  xMin  = m_histoData[2];
  const float  xMax  = m_histoData[3];
  const int  nBinsY  = (int) m_histoData[4];
  const float  yMin  = m_histoData[5];
  const float  yMax  = m_histoData[6];
  const float  entries  = m_histoData[7];  

  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram) {
    if (m_verbosity > 0) {
      std::cout << "DimProxy book H2D histogram " << std::endl <<
              "2D histo: #binsX "     << nBinsX << std::endl <<
              "          xMin   "     << xMin   << std::endl <<
              "          xMax   "     << xMax   << std::endl <<
              "          #binsY "     << nBinsY << std::endl <<
              "          yMin   "     << yMin   << std::endl <<
              "          yMax   "     << yMax   << std::endl;
    }

    // book histo
    m_H2D = new TH2F(m_serviceName.c_str(), m_serviceName.c_str(),
                                 nBinsX, xMin, xMax,
                                 nBinsY, yMin, yMax);        
//    boost::shared_ptr<TH2F> m_H2D(new TH2F(m_serviceName.c_str(),
//                                           m_serviceName.c_str(),
//                                           nBinsX, xMin, xMax,
//                                           nBinsY, yMin, yMax));                
    m_bookedHistogram = true;
    if (m_verbosity > 1) {
     std::cout << "DimProxy booked H2D histogram" << std::endl;
    }
  }

  if (0 == m_serviceSize) m_serviceSize = getSize() / sizeof(float);

  if (m_verbosity > 2){
    std::cout << "service size " << m_serviceSize << std::endl;    
    for (int i=0; i<m_serviceSize; ++i) {
    std::cout << "DimProxy::update2DData bin, data " << i << ", " <<
                 m_histoData[i] << std::endl;
    }
  }

  // fill histogram
  if (m_verbosity > 2) { 
    std::cout << "histogram address " << m_H2D << std::endl;
  }

  int   iData = 8;  //current position in stream
  float data  = 0;

  if (m_verbosity > 1) {
    std::cout << "DimProxy::update2DData fill histogram " << std::endl;
  }

  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      data = (float) m_histoData[iData];      
      if (m_verbosity > 2 ) {
        std::cout << "set bin i, j = " << i << ", " << j << " = " << data <<
                     std::endl;
      }
      m_H2D->SetBinContent(i, j, data);
      iData ++;
    }
  }

  if (m_verbosity > 1) {
    std::cout << "DimProxy::update2DData fill histogram error" <<
                 std::endl;
  }  
  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      data = (float) m_histoData[iData];      
      if (m_verbosity > 2) {
        std::cout << "set bin i, j = " << i << ", " << j << " = " << data <<
                     std::endl;
      }
      m_H2D->SetBinError(i, j, data);
      iData ++;
    }    
  }

  // set number of entries
  m_H2D->SetEntries(entries);

  if (m_verbosity > 0) {
    std::cout << "#entries "         << entries <<
            " from histo "      << m_H2D->GetEntries() <<
            std::endl <<
            "DimProxy <x> " << m_H2D->GetMean(1) <<
            " <y> "             << m_H2D->GetMean(2) << std::endl;
  }

  if (m_verbosity > 2) {    
    for (int i=0; i<= nBinsX+1; ++i) {
      for (int j=0; j <= nBinsY+1; ++j) {
        double binValue = m_H2D->GetBinContent(i, j);
        if (binValue != 0) {
          std::cout << "H2D histo bin " << i << " " << j <<
                       " value "        << binValue <<
                       " error "        << m_H2D->GetBinError(i, j) <<
                       std::endl;
        }
      }
    }
  }
}

void DimProxy::fillHistogramHPD()
{
  // format of data (private communication from Beat)
  //
  // <normal 1-d header>
  // float array containing the number of entries per bin:
  //   (nbin+2 elements)
  // float array containing the sum of the weights per bin:
  //   (nbin+2 elements)
  // float array containing the sum of the squares of the weights per bin:
  //   (nbin+2 elements)
  const int   nBins = (int) m_histoData[1];
  const float xMin  = m_histoData[2];
  const float xMax  = m_histoData[3];
  const int entries = (int) m_histoData[4];
  float *entriesPerBin;
  float *sumWTPerBin;
  float *sumWT2PerBin;

  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram) {
    m_bookedHistogram = true;

    if (m_verbosity > 0) {
      std::cout << "DimProxy booked HPD histogram " << std::endl <<      
                   "HPD histo: #bins  "   << nBins  << std::endl <<
                     "      xMin  "       << xMin   << std::endl <<
                     "      xMax  "       << xMax   << std::endl;
    }
    
    m_HPD = new TH1F(m_serviceName.c_str(), m_serviceName.c_str(),
                                  nBins, xMin, xMax);
//    boost::shared_ptr<TH1> m_HPD(new TH1F(m_serviceName.c_str(),
//                                          m_serviceName.c_str(),
//                                          nBins, xMin, xMax));
  }
  
  // read out data
  const int offsetEntries = 5;
  const int offsetWT      = 5 + nBins + 2;
  const int offsetWT2     = 5 + nBins + 2 + nBins + 2;

  entriesPerBin = &m_histoData[offsetEntries];
  sumWTPerBin   = &m_histoData[offsetWT];
  sumWT2PerBin  = &m_histoData[offsetWT2];

  m_HPD->Reset();
  float yvalue = 0; 
  Double_t yerr   = 0;
  
  // bin 0: underflow, nBins+1 overflow ?
  for (int i=0; i <= nBins+2; ++i) {
    
    yvalue = 0;                                      // mean in Y
    if (entriesPerBin[i] > 0) yvalue = sumWTPerBin[i]/entriesPerBin[i];
    m_HPD->SetBinContent(i, yvalue);    

    yerr = 0;
    if (entriesPerBin[i] > 0) {
      yerr =  // RMS = sqrt(E[x**2]-E[x]**2)
              TMath::Sqrt(sumWT2PerBin[i]/entriesPerBin[i]-yvalue*yvalue);
    }
    m_HPD->SetBinError(i, yerr);
  }
  
  // set #entries
  m_HPD->SetEntries(entries);
  if (m_verbosity > 0) {    
      std::cout << "#entries "    << entries <<
                   " from histo " << m_HPD->GetEntries() << std::endl;
  }
}

bool DimProxy::serviceOK()
{
    return m_serviceOK;
}

bool DimProxy::serviceUpdated()
{  
    return m_serviceUpdated;  
}

void DimProxy::resetServiceUpdated()
{
  m_serviceUpdated = false;

  if (m_verbosity > 0) {
    std::cout << "DimProxy set flag serviceUpdated to " <<
                 m_serviceUpdated << std::endl;
  }
}

void DimProxy::infoHandler()
{
  if (m_verbosity > 0) {
    std::cout << "DimProxy::infoHandler service has been updated" <<
                 std::endl;
  }
  m_serviceUpdated = true;

  if (DimProxy::CI4 == m_serviceType) {
    m_CI4 = getInt();
    if (m_verbosity > 1) {
      std::cout << "Received new int counter: " << m_CI4 << std::endl;
    }
  } else if (DimProxy::CF4 == m_serviceType) {
    m_CF4 = getFloat();
    if (m_verbosity > 1) {
      std::cout << "Received new float counter: " << m_CF4 << std::endl;
    }
  } else if (DimProxy::CD8 == m_serviceType) {
    m_CD8 = getDouble();
    if (m_verbosity > 1) {
      std::cout << "Received new double counter: " << m_CD8 << std::endl;
    }
  } else { // Assume it is an histogram then...
    m_histoData = (float*) getData();
    if (m_verbosity > 0) {
      std::cout << "DimProxy::infoHandler got new histogram with " <<
                   "dimension " << m_histoData[0] << std::endl;
    }  
    if (m_verbosity > 0) {
      std::cout << "DimProxy::infoHandler checking if histogram " <<
                   "dimension matches expectation..." << std::endl;
    }
  
    if (m_histoDimension != m_histoData[0]) {
      std::cout << "DimProxy: dimensions of received histogram do " <<
                   "not match expectation." << std::endl <<
                   "Expected: " << m_histoDimension << 
                   ", got: " << m_histoData[0] << std::endl;
      m_serviceOK = false;    
      return;
    }    
    if (DimProxy::H2D == m_serviceType) DimProxy::fillHistogramH2D();
    if (DimProxy::H1D == m_serviceType) DimProxy::fillHistogramH1D();
    if (DimProxy::HPD == m_serviceType) DimProxy::fillHistogramHPD();
  }
}
