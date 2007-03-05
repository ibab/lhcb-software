//====================================================================
//  DimInfoHistos.cpp
//--------------------------------------------------------------------
//
//  Package    : Adder
//
//  Description: These classes subscribe to dim histograms buffers,
//               does summing of buffers, and provides a dim service
//               to publish the results 
//
//  Author     : M.Pepe-Altarelli, E.van Herwijnen
//  Created    : 31/01/2007
//  Changes    : 
//
//====================================================================
// Include files
#include "DimInfoHistos.h"
#include <string>
#include <iostream>
#include <math.h>

//the constructor only gets the dimension of the histogram  
DimInfoHistos::DimInfoHistos(std::string hSvcname, int refreshTime)
  : DimInfo((hSvcname).c_str(),refreshTime,-1.0),m_dimension(0),m_data(0),m_dimSvc(0){
  int len = hSvcname.length();
  m_histoname=hSvcname.substr(3,len-3);
  if(       "H1D" == hSvcname.substr(0,3) ) {m_dimension = 1;}
  else  if( "H2D" == hSvcname.substr(0,3) ) {m_dimension = 2;}
  else  if( "HPD" == hSvcname.substr(0,3) ) {m_dimension = 11;}
  else  {        
     std::cerr << "DimInfoHistos(" << m_histoname << "): Unexpected histogram dimension: " 
              << m_dimension << std::endl;
     exit(1);
  }
  
}


DimInfoHistos::DimInfoHistos(std::string hSvcname) : 
  DimInfo((hSvcname).c_str(),-1.0){
  std::cerr << "DimInfoHistos(" << hSvcname << ") called with no refresh time. Using 5 s" << std::endl;
  DimInfoHistos(hSvcname,5);
}

//destructor
DimInfoHistos::~DimInfoHistos() {  
}

//infoHandler gets a fresh copy of the histogram buffer that we are subscribed to
//it also calculates the size of the buffer (m_size) for later use (when publishing)
void DimInfoHistos::infoHandler() 
{ 
  m_data = (float*) getData();
  int tmpsize=(int) m_data[1];
  int tmpsizey=(int) m_data[4];
  m_size=0;
  if( m_dimension == 1) m_size=(5+2*(tmpsize+2))*sizeof(float); 
  else if( m_dimension == 2) m_size=(8+2*(tmpsize+2)*(tmpsizey+2))*sizeof(float);
  else if( m_dimension == 11) m_size=(5+3*(tmpsize+2))*sizeof(float); 
} 

//declareInfo creates a DimService to publish the 'summed' histogram
void DimInfoHistos::declareInfo(std::string SumSvcname) 
{ 
  m_dimSvc = new DimInfoHistoService(SumSvcname,m_data,m_size,m_dimension);
}

//declareTitleInfo creates a DimService to publish the title of the 'summed' histogram
//in a comment service
//this will disappear later
void DimInfoHistos::declareTitleInfo(std::string CommentSvcname, char* Title) 
{ 
  m_dimTitleSvc = new DimService(CommentSvcname.c_str(),Title);
}

//the add method adds the contents of another (1d) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::add(DimInfoHistos* h){
  int nofbins = (int)m_data[1];
  m_data[4]+= h->entries();
  int DataOffset=5;
  for (int i=0;i<=nofbins+1;i++) {
    m_data[i+DataOffset]+=h->getbuffercontent(i+DataOffset);
  }
  int ErrorOffset=DataOffset+nofbins+2;
  for (int i=0;i<=nofbins+1;i++) {
    //to sum errors, must square, then take square root   
    m_data[i+ErrorOffset]=sqrt(pow(m_data[i+ErrorOffset],2)+pow(h->getbuffercontent(i+ErrorOffset),2));
  }
   
  return true;
}

//the add2d method adds the contents of another (2d) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::add2d(DimInfoHistos* h){

  int nofxbins = (int)m_data[1];
  int nofybins = (int)m_data[4];
  
  m_data[7]+= h->entries();
  int DataOffset=7; 
  for (int i=0;i<=nofxbins+1;i++) {
     for (int j=0;j<=nofybins+1;j++) {
        DataOffset++;
        m_data[DataOffset]+=h->getbuffercontent(DataOffset);
     }
  }
  for (int i=0;i<=nofxbins+1;i++) {
     for (int j=0;j<=nofybins+1;j++) { 
        DataOffset++;
        float errorsquared = pow(h->getbuffercontent(DataOffset),2);
	float sumerrorsquared =  pow(m_data[DataOffset],2) + errorsquared; 
        
	m_data[DataOffset]=sqrt(sumerrorsquared);

     }	
  }   
  return true;
}

//the addp method adds the contents of another (profile) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::addp(DimInfoHistos* h){
  int nofbins = (int)m_data[1];
  m_data[4]+= h->entries();
  int DataOffset=5;
  for (int i=0;i<=nofbins+1;i++) {
    m_data[i+DataOffset]+=h->getbuffercontent(i+DataOffset);    
  }
  int SumofWeightsOffset=DataOffset+nofbins+2;
  for (int i=0;i<=nofbins+1;i++) {
    m_data[i+SumofWeightsOffset]+=h->getbuffercontent(i+SumofWeightsOffset);
  }
  int SumofSquaredWeightsOffset = SumofWeightsOffset + nofbins +2;
  //no need to square for profiles, already squared
  for (int i=0;i<=nofbins+1;i++) {
    m_data[i+SumofSquaredWeightsOffset]+=h->getbuffercontent(i+SumofSquaredWeightsOffset);
  } 
  return true;
}

//the updateSum method sets the contents of the current (summed) histogram buffer to the
//published Dim service
//everytime histogram adding is complete, the publishing service should be updated
void DimInfoHistos::updateSum() 
{   
  if (m_dimSvc!=0) {
     m_dimSvc->setData(m_data,m_size);
  } 
}  

//the entries method returns the number of entries depending on the histogram's dimension 
int DimInfoHistos::entries() const
{   
    if (m_dimension==1) return (int)m_data[4];
    else if (m_dimension==2) return (int)m_data[7];
    else if (m_dimension==11) return (int)m_data[4];
    return 0;
}   
 

//the getbuffercontent method is a helper method to return the content of a buffer entry
float DimInfoHistos::getbuffercontent(int ibuffer) const
{
 return m_data[ibuffer];
 }

//DimInfoHistoService constructor
//as we are publishing a raw dim buffer, we can't use the monitorsvc
//this service contains the added histograms
//calculate size and dimension
DimInfoHistoService::DimInfoHistoService(std::string hname, float* data, int size, int dimension)
   : DimService(hname.c_str(),"F",data,size)
   {
    if( dimension == 1) size= (5+2*((int)data[1]+2))*sizeof(float);
    else  if( dimension == 2) size= (8+2*((int)data[1]+2)*((int)data[2]+2))*sizeof(float);
    else  if( dimension == 11) size= (5+3*((int)data[1]+2))*sizeof(float);
   }

//destructor
DimInfoHistoService::~DimInfoHistoService() {

}


//serviceHandler does nothing 
void DimInfoHistoService::serviceHandler() { 
    return;
}     
   
