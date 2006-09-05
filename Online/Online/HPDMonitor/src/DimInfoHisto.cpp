// $Id: DimInfoHisto.cpp,v 1.3 2006-09-05 09:27:30 ukerzel Exp $

// Include files 


#ifdef WIN32
#include <Riostream>
namespace win {  
#include <windows.h>
}
#define mysleep win::Sleep
#else
#include <iostream>
#define mysleep usleep
#endif

// ROOT
#include "TH1.h"
#include "TH2.h"


// local
#include "DimInfoHisto.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DimInfoHisto
//
// 2006-08-08 : Ulrich Kerzel
//              based on code by J. Helder Lopes
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DimInfoHisto::DimInfoHisto(std::string serviceName,
                           int         refreshTime,
                           int         verbosity ):
  m_serviceOK(false),
  m_verbosity(verbosity),
  m_histoDimension(-1),
  m_serviceName(serviceName),
  m_bookedHistogram(false),
  m_serviceSize(0),
  DimInfo(serviceName.c_str(), refreshTime, -1) {
  
  if (m_verbosity > 0)
    std::cout << "*** initialsing DIM histogram for service " << m_serviceName
              << std::endl;
  
  
  //
  // determine whether 1D or 2D histogram from serviceName
  //
  if (m_serviceName.substr(0,3) == "H1D")
    m_histoDimension = 1;
  else if (m_serviceName.substr(0,3) == "H2D")
    m_histoDimension = 2;
  else
    std::cout << "DimInfoHisto::DimInfoHisto cannot identify histogram dimension"
              << std::endl;

  //
  // check if service exists
  //
  DimBrowser dimBrowser;
  int nServices = dimBrowser.getServices(m_serviceName.c_str());  
  if (m_verbosity > 0)
    std::cout << "# services for this service name " << nServices
              << std::endl;
  
  if (nServices != 0) {
    
    // Server publishes the counter requested.
    m_serviceOK   = true;        
    m_serviceSize = getSize()/sizeof(float);
  
    // assume integer type by default, now check for other types 
    char *dimService; 
    char *dimFormat;
    int   dimType;
    while(dimType = dimBrowser.getNextService(dimService, dimFormat)) {      
      if (m_verbosity > 0){        
        std::cout << "DIM service : " << dimService << " format " << dimFormat <<  std::endl;        
      } //verbosity
    } //while
  } //if nServices
  
  if (m_verbosity > 0){    
    std::cout << "histogram dimension: "  << m_histoDimension << std::endl;
    std::cout << "service OK?          "  << m_serviceOK      << std::endl;
    std::cout << "        size         "  << m_serviceSize    << std::endl;    
  } // if verbosity
  
  
} //constructor
//=============================================================================
// Destructor
//=============================================================================
DimInfoHisto::~DimInfoHisto() {} 
  
//=============================================================================
TH1* DimInfoHisto::get1DHisto() {

  // only do something if the histogram exists
  if (!m_serviceOK)
    return 0;
  
  // only operate on 1D histogram
  if (m_histoDimension != 1)
    return 0;
  
  return m_histogram1D;
  
} // TH1* get1DHisto

//=============================================================================
void DimInfoHisto::set1DData() {
  
  int   nBins   = (int) m_histoData[1];
  float xMin    = m_histoData[2];
  float xMax    = m_histoData[3];
  int   entries = (int) m_histoData[4];
  

  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram){

    if (m_verbosity > 0)
      std::cout << "DimInfoHisto book 1D histogram " << std::endl;

    m_bookedHistogram = true;
    
    if (m_verbosity > 0) {
      std::cout << "1D histo: #bins " << nBins   << std::endl;
      std::cout << "          xMin  " << xMin    << std::endl;
      std::cout << "          xMax  " << xMax    << std::endl;
    } // if verbosity
    
    m_histogram1D = new TH1F(m_serviceName.c_str(), m_serviceName.c_str(),
                             nBins, xMin, xMax);
  } // if !histo

  //
  // fill histogram
  //
  if (m_verbosity > 0)
    std::cout << "       #entries " << entries << std::endl;    
  m_histogram1D -> SetEntries(entries);
  
  int offsetData  = 5;
  int offsetError = 5+nBins+1;  
  // N.B. bin 0: underflow, bin nBins+1 overflow
  for (int i=0; i<= nBins+1; i++) {
    m_histogram1D->SetBinContent(i, m_histoData[offsetData+i]);
    m_histogram1D->SetBinError(i,m_histoData[offsetError+i]);
    
    if (m_verbosity > 1)
      std::cout << "set bin " << i << " value " << m_histogram1D->GetBinContent(i)
                << " error " << m_histogram1D->GetBinError(i)
                << std::endl;    
  } //for i
  
  if (m_verbosity > 0)
    std::cout << "DimInfoHisto::set1DData  <x> " << m_histogram1D->GetMean(1) << std::endl;
  
  
} //void set1DData


//=============================================================================
TH2* DimInfoHisto::get2DHisto() {

  // only do something if the histogram exists
  if (!m_serviceOK)
    return 0;
  
  // only operate on 2D histogram
  if (m_histoDimension != 2)
    return 0;

  return m_histogram2D;

} // TH2* get2DHisto

//=============================================================================
void DimInfoHisto::set2DData() {
  
  int   nBinsX   = (int) m_histoData[1];
  float xMin     = m_histoData[2];
  float xMax     = m_histoData[3];
  int   nBinsY   = (int) m_histoData[4];
  float yMin     = m_histoData[5];
  float yMax     = m_histoData[6];
  
  
  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram) {

    if (m_verbosity > 0)
      std::cout << "DimInfoHisto book 2D histogram " << std::endl;

    m_bookedHistogram = true;

    if (m_verbosity > 0) {
      std::cout << "2D histo: #binsX " << nBinsX  << std::endl;
      std::cout << "          xMin   " << xMin    << std::endl;
      std::cout << "          xMax   " << xMax    << std::endl;    
      std::cout << "         #binsY  " << nBinsY  << std::endl;
      std::cout << "          yMin   " << yMin    << std::endl;
      std::cout << "          yMax   " << yMax    << std::endl;
    } // if verbosity
    
    // book histo
    m_histogram2D = new TH2F(m_serviceName.c_str(), m_serviceName.c_str(),
                             nBinsX, xMin, xMax,
                             nBinsY, yMin, yMax);    
  } // if ! histo

  if (m_verbosity > 2){
    for (int i=0; i< m_serviceSize; i++) {
        std::cout << "DimInfoHisto::update2DData bin, data " << i << " , " << m_histoData[i] << std::endl;
    } //for
  } // if verbosity
   

  float entries  = m_histoData[7];
  if (m_verbosity > 1) {
    std::cout << "       #entries  " << entries << std::endl;
  } // if verbosity
  
  
  // set histogram data
  m_histogram2D -> SetEntries(entries);
  int iData = 8;  //current position in stream

  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      m_histogram2D -> SetBinContent(i,j,m_histoData[iData++]);
    } //for j    
  } //for i

  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      m_histogram2D -> SetBinError(i,j,m_histoData[iData++]);
    } //for j    
  } //for i

  if (m_verbosity > 0)
    std::cout << "DimInfoHisto <x> " << m_histogram2D->GetMean(1) << " <y> " <<  m_histogram2D->GetMean(2) << std::endl;

  if (m_verbosity > 1){    
    for (int i=0; i<= nBinsX+1; ++i) {
      for (int j=0; j <= nBinsY+1; ++j) {
        double binValue = m_histogram2D -> GetBinContent(i,j);
        if (binValue != 0)
          std::cout << "2D histo bin " << i << " " << j << " value " <<  binValue
                    << " error " << m_histogram2D -> GetBinError(i,j)
                    << std::endl;
      }// for i
    } // for j
  }// if verbosity
  
} // void set2DData

//============================================================================= 
bool DimInfoHisto::serviceOK() {
  return m_serviceOK;
} // bool serviceOK

//=============================================================================
void DimInfoHisto::infoHandler()  {
  
  m_histoData = (float*) getData();

  if (m_verbosity > 0)
    std::cout << "DimInfoHisto::infoHandler got new histogram with dimension " << m_histoData[0] << std::endl;  
  
  // check if received data matches the histogram dimension
  // expected
  if (m_histoDimension != m_histoData[0]) {
    std::cout << "dimensions of received histogram do not match expectation " << std::endl;
    std::cout << "expect  " << m_histoDimension << std::endl;
    std::cout << "got     " << m_histoData[0]   << std::endl;
    m_serviceOK = false;    
  }// if histoDimension

  if (m_histoDimension == 2)
    DimInfoHisto::set2DData();

  if (m_histoDimension == 1)
    DimInfoHisto::set1DData();
  
  
} // bool infoHandler
 
