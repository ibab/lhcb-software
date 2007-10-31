// $Id: DimInfoHisto.cpp,v 1.14 2007-10-31 10:45:37 ukerzel Exp $

// Include files 


#include <iostream>
#include <stdlib.h>
#include <malloc.h>

#ifdef WIN32
namespace win {  
#include <windows.h>
}
#define mysleep win::Sleep
#else
#define mysleep usleep
#endif

// ROOT
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

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
  DimInfo(serviceName.c_str(), refreshTime, -1),
  m_serviceOK(false),
  m_serviceUpdated(false),
  m_bookedHistogram(false),
  m_verbosity(verbosity),
  m_histoDimension(-1),
  m_serviceSize(0),
  m_serviceName(serviceName),
  m_serviceType(DimInfoHisto::unknown),
  m_nWait(0),
  m_waitTime(0){
  
  if (m_verbosity > 0)
    std::cout << "*** initialising DIM histogram for service " << m_serviceName
              << std::endl;
  
  
  //
  // determine whether 1D or 2D histogram from serviceName
  //
  if ((m_serviceName.substr(0,3) == "H1D") || (m_serviceName.substr(0,3) == "HPD"))
    m_histoDimension = 1;
  else if (m_serviceName.substr(0,3) == "H2D")
    m_histoDimension = 2;
  else
    std::cout << "DimInfoHisto::DimInfoHisto cannot identify histogram dimension"
              << std::endl;

  if (m_serviceName.substr(0,3) == "H1D")
    m_serviceType = DimInfoHisto::h1D;
  if (m_serviceName.substr(0,3) == "H2D")
    m_serviceType = DimInfoHisto::h2D;
  if (m_serviceName.substr(0,3) == "HPD")
    m_serviceType = DimInfoHisto::hProfile;
  

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
    m_serviceSize = getSize()/sizeof(float);

    // wait until data has arrived
    m_waitTime =(int)((double)refreshTime*1e5); // 10% of refresh time
    if (m_verbosity > 0)
      std::cout << "wait time " << m_waitTime << std::endl;

    while (m_serviceSize <= 0) {
      m_nWait++;
      std::cout << m_nWait << "th wait for service " << m_serviceName << std::endl;
      mysleep(m_waitTime);
      m_serviceSize = getSize()/sizeof(float);
    }
    m_serviceOK   = true;        
    m_nWait       = 0; //reset

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
    std::cout << "        type         "  << m_serviceType    << std::endl;    
  } // if verbosity
  
  
} //constructor
//=============================================================================
// Destructor
//=============================================================================
DimInfoHisto::~DimInfoHisto() {

  if (m_verbosity > 0)
    std::cout << "destructor of DimHinfoHisto called" << std::endl;
  
  if (m_verbosity > 1)
    std::cout << "delete 1D histo " << std::endl;
  if (m_serviceType == DimInfoHisto::h1D && m_histogram1D != NULL )
    m_histogram1D->Delete();  

  if (m_verbosity > 1)
    std::cout << "delete profile histo " << std::endl;
  if (m_serviceType == DimInfoHisto::hProfile && m_histogramProfile != NULL )
    m_histogramProfile->Delete();  
  
  if (m_verbosity > 1)
    std::cout << "delete 2D histo " << std::endl;  
  if (m_serviceType == DimInfoHisto::h2D && m_histogram2D != NULL)
    m_histogram2D->Delete();
  
  if (m_verbosity > 1)
    std::cout << "destructor of DimInfoHisto ends" << std::endl;
  
}// destructor  
//=============================================================================

TH1* DimInfoHisto::getProfileHisto() {

  // only do something if the histogram exists
  if (!m_serviceOK)
    return 0;
  
  // only operate on 1D histogram
 if (m_serviceType == DimInfoHisto::hProfile)
  {
    return m_histogramProfile;
  }
  else
  {
    return 0;
  }
  return 0;
  
} // TH1* getProfileHisto

TH1* DimInfoHisto::get1DHisto() {

  // only do something if the histogram exists
  if (!m_serviceOK)
    return 0;
  
  // only operate on 1D histogram
  if (m_serviceType == DimInfoHisto::h1D)
  {
    return m_histogram1D;
  }
  else
  {
    return 0;
  }

  return 0;
  
} // TH1* get1DHisto

//=============================================================================
void DimInfoHisto::set1DData() {
  
  const int   nBins   = (int) m_histoData[1];
  const float xMin    = m_histoData[2];
  const float xMax    = m_histoData[3];
  const int   entries = (int) m_histoData[4];
  

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
  int offsetData  = 5;
  int offsetError = 5+nBins+1;  
  // N.B. bin 0: underflow, bin nBins+1 overflow

  // set underflows and overflows:
  m_histogram1D->SetBinContent(0       , m_histoData[5]);
  m_histogram1D->SetBinContent(nBins+1 , m_histoData[5+nBins+1]);
  for (int i=1; i<= nBins; i++) {
    m_histogram1D->SetBinContent(i, m_histoData[offsetData+i]);
    m_histogram1D->SetBinError(i,m_histoData[offsetError+i]);
    
    if (m_verbosity > 1)
      std::cout << "index " << i 
                << " array " << m_histoData[offsetData+i]        << " +- " << m_histoData[offsetError+i]
                << " histo " << m_histogram1D->GetBinContent(i)  << " +- " << m_histogram1D->GetBinError(i)
                << std::endl;    
  } //for i

  //
  // set #entries
  //
  m_histogram1D -> SetEntries(entries);

  if (m_verbosity > 0) {    
    std::cout << " #entries "    << entries
              << "  from histo " << m_histogram1D -> GetEntries() << std::endl;
    std::cout << "DimInfoHisto::set1DData  <x> " << m_histogram1D->GetMean(1) << std::endl;
  } // if verbose
  
  
  
} //void set1DData


//=============================================================================
TH2* DimInfoHisto::get2DHisto() {

  // only do something if the histogram exists
  if (!m_serviceOK)
    return 0;
  
  // only operate on 2D histogram
  if (m_serviceType != DimInfoHisto::h2D)
    return 0;

  return m_histogram2D;

} // TH2* get2DHisto

//=============================================================================
void DimInfoHisto::set2DData() {
  
  const int   nBinsX   = (int) m_histoData[1];
  const float xMin     = m_histoData[2];
  const float xMax     = m_histoData[3];
  const int   nBinsY   = (int) m_histoData[4];
  const float yMin     = m_histoData[5];
  const float yMax     = m_histoData[6];
  const float entries  = m_histoData[7];  
  
  // if the histogram does not exist, book it
  // -> assumes that histogram does not change
  if (!m_bookedHistogram) {

    if (m_verbosity > 0) {
      std::cout << "DimInfoHisto book 2D histogram " << std::endl;
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

    m_bookedHistogram = true;


    if (m_verbosity > 1)
      std::cout << "DimInfoHisto booked 2D histogram " << std::endl;
    
  } // if ! histo

  if (m_serviceSize == 0)
    m_serviceSize = getSize()/sizeof(float);

  if (m_verbosity > 2){
    std::cout << "service size " << m_serviceSize << std::endl;    
    for (int i=0; i< m_serviceSize; i++) {
        std::cout << "DimInfoHisto::update2DData bin, data " << i << " , " << m_histoData[i] << std::endl;
    } //for
  } // if verbosity
   

  //
  // fill histogram
  //

  if (m_verbosity > 2)
    std::cout << "histogram address " << m_histogram2D << std::endl;
  

  int   iData = 8;  //current position in stream
  float data  = 0;  

  if (m_verbosity > 1)
    std::cout << "DimInfoHisto::update2DData fill histogram " << std::endl;
  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      data = (float) m_histoData[iData];      
      if (m_verbosity > 2 )
        std::cout << "set bin i,j = " << i << " , " << j << " = " << data  << std::endl;
      m_histogram2D -> SetBinContent(i,j,data);
      iData ++;
    } //for j    
  } //for i

  if (m_verbosity > 1)
    std::cout << "DimInfoHisto::update2DData fill histogram error " << std::endl;  
  for (int i=0; i<= nBinsX+1; ++i) {
    for (int j=0; j <= nBinsY+1; ++j) {
      data = (float) m_histoData[iData];      
      if (m_verbosity > 2)
        std::cout << "set bin i,j = " << i << " , " << j << " = " << data << std::endl;
      m_histogram2D -> SetBinError(i,j,data);
      iData ++;
    } //for j    
  } //for i

  //
  // set number of entries
  //
  m_histogram2D -> SetEntries(entries);

  if (m_verbosity > 0){
    std::cout << " #entries "    << entries
              << "  from histo " << m_histogram2D -> GetEntries() << std::endl;

    std::cout << "DimInfoHisto <x> " << m_histogram2D->GetMean(1) << " <y> " <<  m_histogram2D->GetMean(2) << std::endl;
  } // if verbose

  if (m_verbosity > 2){    
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
void DimInfoHisto::setProfileData(){


  /** format of data (private communication from Beat)
   *
   *  <normal 1-d header>
   * float array containing the number of entries per bin                 (nbin+2 elements)
   * float array containing the sum of the weights per bin                (nbin+2 elements)
   * float array containing the sum of the squares of the weights per bin (nbin+2 elements)
   *
   */
  const int   nBins   = (int) m_histoData[1];
  const float xMin    = m_histoData[2];
  const float xMax    = m_histoData[3];
  const int   entries = (int) m_histoData[4];
  float *entriesPerBin;
  float *sumWTPerBin;
  float *sumWT2PerBin;
  

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
    
    m_histogramProfile = new TH1F(m_serviceName.c_str(), m_serviceName.c_str(),
                                  nBins, xMin, xMax);
  } // if !histo


  //
  // read out data
  //
  
  const int offsetEntries = 5;
  const int offsetWT      = 5 + nBins+2;
  const int offsetWT2     = 5 + nBins+2 + nBins+2;

  entriesPerBin = &m_histoData[offsetEntries];
  sumWTPerBin  = &m_histoData[offsetWT];
  sumWT2PerBin  = &m_histoData[offsetWT2];

  m_histogramProfile -> Reset();
  float yvalue = 0; 
  float yerr = 0;
  // bin 0: underflow, nBins+1 overflow ?
  for (int i = 0; i <= nBins+2; i++) {
    yvalue = 0;    
    if (entriesPerBin[i] > 0)
      yvalue = sumWTPerBin[i]/entriesPerBin[i];  // mean in Y    
    m_histogramProfile->SetBinContent(i, yvalue);    

    yerr = 0;
    if (entriesPerBin[i] > 0)
      yerr = TMath::Sqrt(sumWT2PerBin[i]/entriesPerBin[i]-yvalue*yvalue);  // RMS = sqrt(E[x**2]-E[x]**2)
    m_histogramProfile->SetBinError(i, yerr);
    
  } //for
  

  //
  // set #entries
  //
  m_histogramProfile -> SetEntries(entries);

  if (m_verbosity > 0) {    
    std::cout << " #entries "    << entries
              << "  from histo " << m_histogramProfile -> GetEntries() << std::endl;
  } // if verbose

} //void setProfileData
//============================================================================= 
bool DimInfoHisto::serviceOK() {
  return m_serviceOK;
} // bool serviceOK
//=============================================================================
bool DimInfoHisto::serviceUpdated() {  
  return m_serviceUpdated;  
} // bool service updated
//=============================================================================
void DimInfoHisto::ResetServiceUpdated() {
  m_serviceUpdated = false;
  
  if (m_verbosity > 0)
    std::cout << "DimInfoHisto set flag serviceUpdated to "  << m_serviceUpdated 
              << std::endl;
  
} // void ResetServiceUpdated
//=============================================================================
void DimInfoHisto::infoHandler()  {

  if (m_verbosity > 0)
    std::cout << "DimInfoHisto::infoHandler service has been updated " << std::endl;
  m_serviceUpdated = true;
  

  // wait until data has arrived
  m_serviceSize = getSize()/sizeof(float);
  while (m_serviceSize <= 0) {
    m_serviceOK = false;
    m_nWait++;
    std::cout << m_nWait << "th wait for service " << m_serviceName << std::endl;
    mysleep(m_waitTime);
    m_serviceSize = getSize()/sizeof(float);
  } //while

  m_serviceOK = true;
  m_nWait     = 0;                     //reset
  m_histoData = (float*) getData();

  if (m_verbosity > 0)
    std::cout << "DimInfoHisto::infoHandler got new histogram with dimension " << m_histoData[0] << std::endl;  
  

  if (m_verbosity > 0)
    std::cout << "DimInfoHisto::infoHandler check if histogram dimension matches expectation " << std::endl;
  
  if (m_histoDimension != m_histoData[0]) {
    std::cout << "DimInfoHisto: dimensions of received histogram do not match expectation " << std::endl;
    std::cout << "expect  " << m_histoDimension << " got " << m_histoData[0]                << std::endl;
    m_serviceOK = false;    
    return;    
  }// if histoDimension

  if (m_serviceType == DimInfoHisto::h2D)
    DimInfoHisto::set2DData();

  if (m_serviceType == DimInfoHisto::h1D)
    DimInfoHisto::set1DData();

  if (m_serviceType == DimInfoHisto::hProfile)
    DimInfoHisto::setProfileData();
  
  
} // bool infoHandler
 
