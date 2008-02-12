#include "DimInfoHistos.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep win::Sleep
#else
# define mysleep usleep
#endif

//constructor
DimInfoHistos::DimInfoHistos(std::string hSvcname, int refreshTime)
  : DimInfo((hSvcname).c_str(),refreshTime,-1.0),
    m_hist(0),m_dimension(0),m_data(0),m_hasData(false){
  std::cerr << "DimInfoHistos constructor called "<< hSvcname.c_str() 
            << " refresh time: " << refreshTime << " seconds" << std::endl;
  m_rtime=refreshTime;
  int len = hSvcname.length();
  m_histType=hSvcname.substr(0,3);
  m_histoname=hSvcname.substr(3,len-3);
  if(       "H1D" == m_histType ) m_dimension = 1;
  else  if( "H2D" == m_histType ) m_dimension = 2;
  else  if( "HPD" == m_histType ) m_dimension = 1;
  else  {
  	std::cerr << "DimInfoHistos(" << m_histoname << "): Unexpected histogram type: " << m_histType << " or dimension: " << m_dimension << std::endl;
    exit(1);
  }
  
  //std::cerr << "DimInfoHistos(" << m_histoname << ") leaving constructor. " << std::endl;
  
}


DimInfoHistos::DimInfoHistos(std::string hSvcname) : 
  DimInfo((hSvcname).c_str(),-1.0){
  std::cerr << "DimInfoHistos(" << hSvcname << ") called with no refresh time. Using 5 s" << std::endl;
  DimInfoHistos(hSvcname,5);
}

//destructor
DimInfoHistos::~DimInfoHistos() {  
  //std::cerr << "DimInfoHistos(" << m_histoname << "): Destructor called" << std::endl;
  // if (m_hist!=0) delete m_hist;  // Leave the deletion to root, after closing any root file.
}

void DimInfoHistos::infoHandler() 
{ 
  //std::cerr << "DimInfoHistos(" << m_histoname << ") " << " histo address: " << m_hist << std::endl;
  //mysleep(m_rtime);
  
  m_data = (float*) getData();
  //int dim = getSize()/sizeof(float);
  //for(int i=0;i<dim;i++) {
  //  std::cerr << "DimInfoHistos(" << m_histoname << "): i = " << i 
  //            << " data[i] = " << m_data[i] << std::endl;
  //	}
  if( m_dimension != m_data[0] ){
    std::cerr 
      << "DimInfoHistos(" << m_histoname << "): Conflicting histogram dimensions. From DimService dim= " 
      << m_data[0] << std::endl;
    exit(2);
  }
  // Initialize histos
	if( 0 == m_hist ) {
    if( "H1D" == m_histType ) {
      // 1Hd m_data: dimension,nXBins,xMin,xMax,2*(UNDERFLOW,"in range" bins, OVERFLOW): entries and errors
      m_hist=new TH1F(m_histoname.c_str(),m_histoname.c_str(),(int)m_data[1],m_data[2],m_data[3]);	
    }      
    else if( "H2D" == m_histType ) {
      // 2Hd data: dimension,nXBins,xMin,xMax,nYBins,yMin,yMax,
      // 2*((UNDERFLOW,"in range" bins, OVERFLOW)_x*(UNDERFLOW,"in range" bins,OVERFLOW)_y)
      m_hist=new TH2F(m_histoname.c_str(),m_histoname.c_str(),(int)m_data[1],m_data[2],
                      m_data[3],(int)m_data[4],m_data[5],m_data[6]);	
    } 
    else if( "HPD" == m_histType ) {
    // 1d profile data: dimension,nXBins,xMin,xMax,entries,3*(UNDERFLOW,"in range" bins, OVERFLOW): 
    // entries, weights and squares of weights
 m_hist=new TProfile(m_histoname.c_str(),m_histoname.c_str(),(int)m_data[1],m_data[2],m_data[3]);
    }

	}
	if( 0 == m_hist ) {
    std::cerr << "DimInfoHistos(" << m_histoname << "): Unable to create root histogram"  << std::endl;
    exit(3);
	}
  if( "H1D" == m_histType ) setH1Data();
  if( "H2D" == m_histType ) setH2Data();
  if( "HPD" == m_histType ) set1DProfData();
} 


TH1* DimInfoHistos::getHisto() {
  //std::cerr << "DimInfoHistos(" << m_histoname << "), get_hist " << m_hist 
  //          << " hasData: " << std::boolalpha << m_hasData << std::endl;
  if(m_hasData == true ) return m_hist;
  return 0;
}


void DimInfoHistos::setH1Data(){
  // Set total number of entries
  m_hist->SetEntries(m_data[4]);
  std::cerr << "DimInfoHistos(" << m_histoname << "), setH2Data: total entries: " << m_data[4] << std::endl;
  
  // Remember root histo: bin 0: underflows, bin N+1: overflows
  int nofbins = m_hist->GetNbinsX();
  int iData=5;
  for (int i=0;i<=nofbins+1;i++) {
    m_hist->SetBinContent(i,m_data[iData++]);
    //std::cerr << "DimInfoHistos(" << m_histoname << "), setH1Data: entries: index in DimInfo data: "
    //          << iData-1 << " value: " << m_data[iData-1] << std::endl;
  }
  
  for (int i=0;i<=nofbins+1;i++) {
    m_hist->SetBinError(i,m_data[iData++]);
    //std::cerr << "DimInfoHistos(" << m_histoname << "), setH1Data: errors: index in DimInfo data:  "
    //          << iData-1 << " value: " << m_data[iData-1] <<std::endl;
  }
  // Debug: Check the root histo just filled
  for (int i=0;i<=nofbins+1;i++) {
    std::cerr << "DimInfoHistos(" << m_histoname << "), setH1Data: Root histo content: bin: " << i 
              << " entries: " << m_hist->GetBinContent(i) << " error: " << m_hist->GetBinError(i) << std::endl;
  }   
  m_hasData=true;
}

void DimInfoHistos::setH2Data(){ 
  // Set total number of entries
  m_hist->SetEntries(m_data[7]);
  std::cerr << "DimInfoHistos(" << m_histoname << "), setH2Data: total entries: " << m_data[7] << std::endl;
  
  int nofbinsX = m_hist->GetNbinsX();
  int nofbinsY = m_hist->GetNbinsY();
  int iData=8;
  for (int i=0; i<=nofbinsX+1; ++i) {
    for (int j=0; j<=nofbinsY+1; ++j) {
      m_hist->SetBinContent(i,j,m_data[iData++]); 
      //std::cerr << "DimInfoHistos(" << m_histoname << "), setH2Data: entries:  xbin "<< i << " ybin " << j 
      //          << " index in DimInfo data: " << iData-1 << " value: " << m_data[iData-1] << std::endl;
    }
  }
  
  for (int i=0; i<=nofbinsX+1; ++i) {
    for (int j=0; j<=nofbinsY+1; ++j) {
      m_hist->SetBinError(i,j,m_data[iData++]); 
      //std::cerr << "DimInfoHistos(" << m_histoname << "), setH2Data: errors: xbin "<< i << " ybin " << j 
      //          << " index in DimInfo data: " << iData-1 << " value: " << m_data[iData-1] << std::endl;
    }
  }
  
  // Debug: Check the root histo just filled
  for (int i=0; i<=nofbinsX+1; ++i) {
    for (int j=0; j<=nofbinsY+1; ++j) {
      std::cerr << "DimInfoHistos(" << m_histoname << "), setH2Data: Root histo content: xbin: " 
                << i  << " ybin: " << j 
                << " entries: " << m_hist->GetBinContent(i,j) 
                << " error: " << m_hist->GetBinError(i,j) << std::endl;
    }
  }
  
  m_hasData=true;
}


void DimInfoHistos::set1DProfData(){
TProfile * hprof = dynamic_cast<TProfile * >(m_hist);
    // 1d profile data: dimension,nXBins,xMin,xMax,entries,3*(UNDERFLOW,"in range" bins, OVERFLOW): entries, weights and squares of weights  // Set total number of entries
  hprof->SetEntries(m_data[4]);
  std::cerr << "DimInfoHistos(" << m_histoname << "), setProf1DData: total entries: " << m_data[4] << std::endl;
  
  int nofbins = hprof->GetNbinsX();
  int iDataEntries =5;
  int iDataWeights=iDataEntries+nofbins+2;
  int iDataWeightsSquared  =iDataEntries+2*(nofbins+2);
  // Remember root histo: bin 0: underflows, bin N+1: overflows
  for (int i=0;i<=nofbins+1;i++) {
  double entries = m_data[iDataEntries++]; 
  double weight = m_data[iDataWeights++]; 
                // AIDA::IProfile1D doesn't provide the sum of the squares of the weights.
                // It has been recalculated from the #entries, the rms and the mean (sum of eights)
                // Assuming the relation error^2 = sum(y^2)/entries - (sum(y)/entries)^2
  double weightSquared = m_data[iDataWeightsSquared++]; 
  // in Root the SetBinError method was not overriden in the TProfile class, so the error definition is the one for TH1 histos base class.
  // However, the GetBinError was overriden and gives the correct error for the 1D TProfile.
  //double error = sqrt(weightSquared/entries - (weight/entries)*(weight/entries));
  double error = sqrt(weightSquared);
    hprof->SetBinEntries(i,entries);
    hprof->SetBinContent(i,weight);
    hprof->SetBinError(i,error);
    //std::cerr << "DimInfoHistos(" << m_histoname << "), set1DProfData: index in data: " << iDataEntries-1 << " xbin: " << i 
    //<< " entries: " << entries << " content: " << weight << " weightSquared: " << weightSquared 
    //<< " input error: " << error << std::endl;
  }
  
  // Debug: Check the root profile histo just filled
  for (int i=0;i<=nofbins+1;i++) {
    std::cerr << "DimInfoHistos(" << m_histoname << "), set1DProfData: Root histo: bin: " << i 
              << " entries: " << hprof->GetBinEntries(i) << " content: " 
	      <<  hprof->GetBinContent(i) << " error: " << hprof->GetBinError(i) << std::endl;
  }   
  m_hasData=true;
}
