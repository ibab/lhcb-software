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
  : DimInfo((hSvcname).c_str(),refreshTime,-1), m_dimension(0),m_data(0),m_dimSvc(0){
  int len = hSvcname.length();
  m_histoname=hSvcname.substr(3,len-3);
  if(       "H1D" == hSvcname.substr(0,3) ) {m_dimension = 1;}
  else  if( "H2D" == hSvcname.substr(0,3) ) {m_dimension = 2;}
  else  if( "HPD" == hSvcname.substr(0,3) ) {m_dimension = 11;}
  else  {        
     std::cerr << "DimInfoHistos(" << m_histoname << "): Unexpected histogram dimension: " << std::endl;
     exit(1);
  }
  
}


DimInfoHistos::DimInfoHistos(std::string hSvcname) : 
  DimInfo((hSvcname).c_str(),-1){
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
// std::cout << "DimInfoHistos making new dimservice " << SumSvcname << " size "
//  << m_size << " dimension " << m_dimension << std::endl;
  m_dimSvc = new DimInfoHistoService(SumSvcname,m_data,m_size,m_dimension);
  
//  if (0 !=m_data_sum) {
//     std::cout << "DimInfoHistos making new m_data_sum" <<  std::endl;
     m_data_sum = new float[m_size]; 
 // }   
}

//declareTitleInfo creates a DimService to publish the title of the 'summed' histogram
//in a comment service
//this will disappear later
void DimInfoHistos::declareTitleInfo(std::string CommentSvcname, char* Title) 
{ 
//   std::cout << "DimInfoHistos declareTitleInfo "  <<  CommentSvcname << std::endl;
 
  m_dimTitleSvc = new DimService(CommentSvcname.c_str(),Title);
}

//the add method adds the contents of another (1d) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::add(DimInfoHistos* h){
  //return false if we aren't adding 1d histos
  if (1 != m_dimension ) {
 //    std::cout << "DimInfoHistos not a 1d histo " << m_dimension << std::endl;
     return false;
  }   
  int nofbins = (int)m_data[1];
  int DataOffset=5;
  int ErrorOffset=DataOffset+nofbins+2;
  if (h==this) {
       //re-initialise the sum
   //  std::cout << "DimInfoHistos reinitializing 1dsum" << std::endl; 
       for (int i=0;i<=4;i++) {
          m_data_sum[i]=m_data[i];
       } 	  
       for (int i=0;i<=nofbins+1;i++) {
          m_data_sum[i+DataOffset]=m_data[i+DataOffset];
       }   

       for (int i=0;i<=nofbins+1;i++) {
          m_data_sum[i+ErrorOffset]=m_data[i+ErrorOffset];
       } 
  }
  else {
      // std::cout << "DimInfoHistos adding 1dsum" << std::endl; 
     m_data_sum[4]+= h->entries();
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+DataOffset]+=h->getbuffercontent(i+DataOffset);
     }   
     for (int i=0;i<=nofbins+1;i++) {
       //to sum errors, must square, then take square root   
       m_data_sum[i+ErrorOffset]=sqrt(pow(m_data_sum[i+ErrorOffset],2)+pow(h->getbuffercontent(i+ErrorOffset),2));
     }
  } 
  return true;
}

//the add2d method adds the contents of another (2d) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::add2d(DimInfoHistos* h){
  //return false if we aren't adding 2d histos
  if (2 != m_dimension ) {
     //  std::cout << "DimInfoHistos not a 2d histo " << m_dimension << std::endl;
     return false;
  }   
  int nofxbins = (int)m_data[1];
  int nofybins = (int)m_data[4];
  int DataOffset=7; 
  if (h==this) {
    //   std::cout << "DimInfoHistos reinitializing 2dsum" << std::endl; 
     for (int i=0;i<=7;i++) {
        m_data_sum[i]=m_data[i];
     }  
     for (int i=0;i<=nofxbins+1;i++) {
        for (int j=0;j<=nofybins+1;j++) {
           DataOffset++;
           m_data_sum[DataOffset]=m_data[DataOffset];
        }
     }
     for (int i=0;i<=nofxbins+1;i++) {
        for (int j=0;j<=nofybins+1;j++) { 
          DataOffset++;
	   float sumerrorsquared = pow(m_data[DataOffset],2);         
	   m_data_sum[DataOffset]=sqrt(sumerrorsquared);
        }	  	  
     }
  }
  else {
     m_data_sum[7]+= h->entries();
    // std::cout << "DimInfoHistos adding 2dsum" << std::endl; 
     for (int i=0;i<=nofxbins+1;i++) {
        for (int j=0;j<=nofybins+1;j++) {
           DataOffset++;
           m_data_sum[DataOffset]+=h->getbuffercontent(DataOffset);
        }
     }
     for (int i=0;i<=nofxbins+1;i++) {
        for (int j=0;j<=nofybins+1;j++) { 
           DataOffset++;
           float errorsquared = pow(h->getbuffercontent(DataOffset),2);
	   float sumerrorsquared =  pow(m_data_sum[DataOffset],2) + errorsquared; 
        
	   m_data_sum[DataOffset]=sqrt(sumerrorsquared);

        }
     }		
  }   
  return true;
}

//the addp method adds the contents of another (profile) histogram buffer to the current one
//performs the operation: this = this + h
bool DimInfoHistos::addp(DimInfoHistos* h){
  //return false if we aren't adding profile histos
  if (11 != m_dimension) {
   //   std::cout << "DimInfoHistos not a profile histo " <<  m_dimension << std::endl;
     return false;
  }   
  int nofbins = (int)m_data[1];
  int DataOffset=5;
  int SumofWeightsOffset=DataOffset+nofbins+2;
  int SumofSquaredWeightsOffset = SumofWeightsOffset + nofbins +2;

  if (h==this) {
     //re-initialise the sum
   // std::cout << "DimInfoHistos reinitializing prsum" << std::endl; 
     for (int i=0;i<=4;i++) {
       m_data_sum[i]=m_data[i];
     } 
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+DataOffset]=m_data[i+DataOffset];    
     }

     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+SumofWeightsOffset]=m_data[i+SumofWeightsOffset];
     }
     //no need to square for profiles, already squared
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+SumofSquaredWeightsOffset]=m_data[i+SumofSquaredWeightsOffset];
     } 
  }   
   else {  
     //  std::cout << "DimInfoHistos adding prsum" << std::endl; 
     m_data_sum[4]+= h->entries();
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+DataOffset]+=h->getbuffercontent(i+DataOffset);    
     }
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+SumofWeightsOffset]+=h->getbuffercontent(i+SumofWeightsOffset);
     }
     //no need to square for profiles, already squared
     for (int i=0;i<=nofbins+1;i++) {
       m_data_sum[i+SumofSquaredWeightsOffset]+=h->getbuffercontent(i+SumofSquaredWeightsOffset);
     } 
  }   
  return true;
}

//the updateSum method sets the contents of the current (summed) histogram buffer to the
//published Dim service
//everytime histogram adding is complete, the publishing service should be updated
void DimInfoHistos::updateSum() 
{   
  if (m_dimSvc!=0) {  
  // std::cout << "DimInfoHistos updating sum dim service" << std::endl; 
     m_dimSvc->setData(m_data_sum,m_size);
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
   
