#include <iostream>
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/CandidateTower.h"

#include <algorithm>  

L0Muon::CandidateTower::CandidateTower(){
  m_seed = seedind(0,0);
  m_flag = false ;
  
  for (int sta =0; sta<5;sta++){
    m_xyindex[sta] =seedind(999,999);
  }
  
  m_debug = false;
}

L0Muon::CandidateTower::CandidateTower(std::pair<int, int> seedindex){
  m_seed = seedindex;
  m_flag = false ;
  for (int sta =0; sta<5;sta++){
    m_xyindex[sta] =seedind(0,0);
  }
}


L0Muon::CandidateTower::~CandidateTower(){
}

void L0Muon::CandidateTower::setFoi(int sta, int xfoi, int yfoi){
  m_xFoi[sta] = xfoi;
  m_yFoi[sta] = yfoi;
}

L0Muon::CandidateTower::seedind L0Muon::CandidateTower::getFoi(int sta) {
  std::pair<int, int> foiXY;
  foiXY = seedind(m_xFoi[sta], m_yFoi[sta]);
  return foiXY ;
}

void L0Muon::CandidateTower::setSeed(std::pair<int, int> seedindex){
  m_seed = seedindex ;
}

void  L0Muon::CandidateTower::
setBit(int sta, std::vector<boost::dynamic_bitset<> >  table, int maxXFoi, 
       int maxYFoi, int offset){

  //unsigned 
  int x, y;

  //unsigned 
  int xfoi = maxXFoi;
  //unsigned 
  int yfoi = maxYFoi;
  
  x = m_seed.first + xfoi +offset;
  y = m_seed.second + yfoi;

  
  
 
  int size = (2*m_xFoi[sta]+1)*(2*m_yFoi[sta]+1);
 
  boost::dynamic_bitset<> bits(size);
 

  m_bits = bits;
  if (m_yFoi[sta] == 0){
        bits[0]= table[y][x];
        if (bits.size()>1) {
          bits[1]=table[y][x+1];
          bits[2]=table[y][x-1];
          //for (boost::dynamic_bitset<>::size_type i =1; 
          for (int i =1; 
               //i<= (bits.size()-2)/2; i++){
               i<= (size-2)/2; i++){
            if ( (x+i+1)< 24+2*xfoi ) {
              bits[2*i+1]=table[y][x+i+1];
            
        }
            // if ( (x-i-1)>=0 && (x-i-1)< 999999){
            if ( (x-i-1)< 999999){
            bits[2*i+2]=table[y][x-i-1];
           
              
            } else {
            
                // else if  ( (x-i-1)<0 && (x-i-1)> 999999) {
                bits[2*i+2]=0;
                
              }
              
          }
          
        }
     
        for (boost::dynamic_bitset<>::size_type i =0; i< bits.size(); i++){
          m_bits[i] =bits[i];
        }
   
     } else  if (m_yFoi[sta] > 0){

       //unsigned 
       int maxxfoi = m_xFoi[sta];
       //for (boost::dynamic_bitset<>::size_type i=0; i<(2*maxxfoi+1); i++){
       for (int i=0; i<(2*maxxfoi+1); i++){
       //for (boost::dynamic_bitset<>::size_type i=0; i<(2*m_xFoi[sta]+1); i++){
         if ( y< 4+2*m_yFoi[sta] ) {
           //bits[i] = table[y][x- maxxfoi+i];
           bits[i] = table[y][x- m_xFoi[sta]+i];
         }
         
       }
       //for (boost::dynamic_bitset<>::size_type i=(2*maxxfoi+1);
       //     i< 2*(2*maxxfoi+1); i++){
       //for (boost::dynamic_bitset<>::size_type i=(2*m_xFoi[sta]+1);
       for (int i=(2*m_xFoi[sta]+1);
            i< 2*(2*m_xFoi[sta]+1); i++){
         bits[i] = table[y-1][x - m_xFoi[sta]+i-(2*m_xFoi[sta]+1)];

       }
       //for (boost::dynamic_bitset<>::size_type i=2*(2*m_xFoi[sta]+1); 
       for (int  i=2*(2*m_xFoi[sta]+1); 
            i< 3*(2*m_xFoi[sta]+1); i++){
         bits[i] = table[y+1][x - m_xFoi[sta]+i-2*(2*m_xFoi[sta]+1)];

         
       }
       
     
     
     for (boost::dynamic_bitset<>::size_type i =0; i< bits.size(); i++){
       m_bits[i] =bits[i];
     }
     
     }
     
}


//=========== ordered bits in M4-M5 =========================
/*
void  L0Muon::CandidateTower::
setOrderedBit(int sta, std::vector<boost::dynamic_bitset<> >  table, 
              int maxXFoi, int maxYFoi, int offset){
  unsigned int x, y;

  unsigned int xfoi = maxXFoi;
  unsigned int yfoi = maxYFoi;
  
  x = m_seed.first + xfoi +offset;
  y = m_seed.second + yfoi;  
  
  int size = (2*m_xFoi[sta]+1)*(2*m_yFoi[sta]+1);
  boost::dynamic_bitset<> tmpbits(size);
  
  tmpbits[0]= table[y][x];
  tmpbits[1]=table[y][x+1];
  tmpbits[2]=table[y][x-1];

  for (int i=1; i<(2*m_xFoi[sta]+1); i++){
    if ( y< 4+2*m_yFoi[sta] ) {
      if (tmpbits.size()>1) {        
        if ( i< ((2*m_xFoi[sta]+1)-1)/2){
     
          if ( (x+i+1)< 24+2*xfoi ) {
            tmpbits[2*i+1]=table[y][x+i+1];
            
           
          }
          if ( (x-i-1)< 999999){
            tmpbits[2*i+2]=table[y][x-i-1];
           
          } else if ( (x-i-1)> 999999) {
            tmpbits[2*i+2]=0;
           
          }
               
        }
        
          
      }
      
              
    }
    }
  
    
  tmpbits[2*m_xFoi[sta]+1]= table[y-1][x];
  tmpbits[1+2*m_xFoi[sta]+1]=table[y-1][x+1];
  tmpbits[2+2*m_xFoi[sta]+1]=table[y-1][x-1];
   
  for (int i=1;  i<(2*m_xFoi[sta]+1); i++){
    
    if ( i< ((2*m_xFoi[sta]+1)-1)/2){
      if ( (x+i+1) < 24+2*xfoi ) {
        tmpbits[2*i+1+2*m_xFoi[sta]+1]=table[y-1][x+i+1];
                     
      }
            
      if ( (x-i-1)< 999999){
        tmpbits[2*i+2+2*m_xFoi[sta]+1]=table[y-1][x-i-1];
               
      } else  if ( (x-i-1) > 999999) {
        tmpbits[2*i+2+2*m_xFoi[sta]+1]=0;
      }
      
      
    }
    
          
    
  }
  
      
  tmpbits[2*(2*m_xFoi[sta]+1)]= table[y+1][x];   
  tmpbits[1+2*(2*m_xFoi[sta]+1)]=table[y+1][x+1];
  tmpbits[2+2*(2*m_xFoi[sta]+1)]=table[y+1][x-1];
 
  for (int i=1; i<(2*m_xFoi[sta]+1); i++){
        
    if ( i< ((2*m_xFoi[sta]+1)-1)/2){
      if ( (x+i+1)< 24+2*xfoi ) {
        
        tmpbits[2*i+1+2*(2*m_xFoi[sta]+1)]=table[y+1][x+i+1];
      
      }
      if ( (x-i-1)< 999999){
        tmpbits[2*i+2+2*(2*m_xFoi[sta]+1)]=table[y+1][x-i-1]; 
       
      } 
      else if  ( (x-i-1)> 999999) {
        tmpbits[2*i+2+2*(2*m_xFoi[sta]+1)]=0;
       
        
      }
    }
    }
   
  m_bits = tmpbits;
  for (boost::dynamic_bitset<>::size_type j=0; j< tmpbits.size(); j++){
    m_bits[j]= tmpbits[j];
   }
  
  
   }
*/
//=========== ordered bits in M4-M5 including yFoi 0=========================
void  L0Muon::CandidateTower::
setOrderedBit(int sta, std::vector<boost::dynamic_bitset<> >  table, 
              int maxXFoi, int maxYFoi, int offset){
  //unsigned 
  int x, y;

  //unsigned int xfoi = maxXFoi;
  //unsigned int yfoi = maxYFoi;

  int xfoi = maxXFoi;
  int yfoi = maxYFoi;

  
  x = m_seed.first + xfoi +offset;
  y = m_seed.second + yfoi;  
  
  int size = (2*m_xFoi[sta]+1)*(2*m_yFoi[sta]+1);
  boost::dynamic_bitset<> tmpbits(size);
  
  tmpbits[0]= table[y][x];
  

  if (1 < size){
    tmpbits[1]=table[y][x+1];
  }
  
  if (2 < size){
    tmpbits[2]=table[y][x-1];
  }
  

  for (int i=1; i<(2*m_xFoi[sta]+1); i++){
    if ( y< 4+2*m_yFoi[sta] ) {
      if (tmpbits.size()>3) {
        if ( i< ((2*m_xFoi[sta]+1)-1)/2){
          
          if ( (x+i+1)< 24+2*xfoi ) {
            tmpbits[2*i+1]=table[y][x+i+1];
          }
          if ( (x-i-1)< 999999){
            tmpbits[2*i+2]=table[y][x-i-1];
            
          } else if ( (x-i-1)> 999999) {
            tmpbits[2*i+2]=0;
            
          }
          
        }
        
      }
      
    }
    
    
  }
  
  

  if ((2*m_xFoi[sta]+1) < size ){
    tmpbits[2*m_xFoi[sta]+1]= table[y-1][x];
  }
    
    

  if ((1+2*m_xFoi[sta]+1) < size){
    tmpbits[1+2*m_xFoi[sta]+1]=table[y-1][x+1];
  }
  

  if ((2+2*m_xFoi[sta]+1) < size){
    tmpbits[2+2*m_xFoi[sta]+1]=table[y-1][x-1];
  }
  

  
  for (int i=1;  i<(2*m_xFoi[sta]+1); i++){
    
    if ( i< ((2*m_xFoi[sta]+1)-1)/2){
      if ( (x+i+1) < 24+2*xfoi ) {
        
        if ( (2*i+1+2*m_xFoi[sta]+1) < size ){
          tmpbits[2*i+1+2*m_xFoi[sta]+1]=table[y-1][x+i+1];
        }
        
      }
            
      if ( (x-i-1)< 999999){
        
        if ( (2*i+2+2*m_xFoi[sta]+1) < size ){ 
          tmpbits[2*i+2+2*m_xFoi[sta]+1]=table[y-1][x-i-1];
        }
        
      } else  if ( (x-i-1) > 999999) {
        
        if ( (2*i+2+2*m_xFoi[sta]+1) < size ){  
          tmpbits[2*i+2+2*m_xFoi[sta]+1]=0;
        }
        
       
      }
      
      
      
    }
    
    
          
    
  }
  

  
  if ((2*(2*m_xFoi[sta]+1)) < size ){
    tmpbits[2*(2*m_xFoi[sta]+1)]= table[y+1][x];
  }

  if ( (1+2*(2*m_xFoi[sta]+1)) < size){
    tmpbits[1+2*(2*m_xFoi[sta]+1)]=table[y+1][x+1];
  }
    
  if ( (2+2*(2*m_xFoi[sta]+1)) < size )  {
    tmpbits[2+2*(2*m_xFoi[sta]+1)]=table[y+1][x-1];
  }
  
  
  
  for (int i=1; i<(2*m_xFoi[sta]+1); i++){
        
    if ( i< ((2*m_xFoi[sta]+1)-1)/2){
      if ( (x+i+1)< 24+2*xfoi ) {
        if ( ( 2*i+1+2*(2*m_xFoi[sta]+1) ) < size){
          tmpbits[2*i+1+2*(2*m_xFoi[sta]+1)]=table[y+1][x+i+1];
        }
        
      }
      if ( (x-i-1)< 999999){
        if ( (2*i+2+2*(2*m_xFoi[sta]+1)) < size ){
          tmpbits[2*i+2+2*(2*m_xFoi[sta]+1)]=table[y+1][x-i-1]; 
        }
        
      } 
      else if  ( (x-i-1)> 999999) {
        if ( (2*i+2+2*(2*m_xFoi[sta]+1)) < size ){
          tmpbits[2*i+2+2*(2*m_xFoi[sta]+1)]=0;
        }
        
        
      }
      
    }
    
    
  }
  
  
  
  m_bits = tmpbits;
  for (boost::dynamic_bitset<>::size_type j=0; j< tmpbits.size(); j++){
    m_bits[j]= tmpbits[j];
  }
}




//=======================================================


//boost::dynamic_bitset<>  L0Muon::CandidateTower::getBit(int sta){
boost::dynamic_bitset<>  L0Muon::CandidateTower::getBit(){
  return m_bits ;
}

void L0Muon::CandidateTower::reset(){   
  m_bits.reset();

  for (int sta =0; sta<5;sta++){
    m_xyindex[sta] = seedind(999,999);
  }
}


void L0Muon::CandidateTower::drawBits(int sta) {

  if (m_debug) std::cout << "Bitset size in sta"<< " " << sta 
       << " " << m_bits.size() << std::endl;
  
  for ( boost::dynamic_bitset<>::size_type ir = 0; ir < m_bits.size(); ir++) {
    if (m_debug) std::cout << " " << m_bits[ir] ;
  }
  if (m_debug) std::cout << std::endl ;
  

}


 void L0Muon::CandidateTower::setPadIndex(int sta, int maxXFoi, int maxYFoi, 
                                          int offset, int ind)
{
  
  int xfoi = maxXFoi;
  int yfoi = maxYFoi;
  
  int x = m_seed.first + xfoi +offset;
  int y = m_seed.second + yfoi;

  int index = ind;
  
   if (sta ==2){
    m_xyindex[sta]= std::make_pair(m_seed.second, m_seed.first);

  } 
  else {
    if (m_yFoi[sta] == 0){   
      if (index ==0) {
        m_xyindex[sta] = std::make_pair(y,x);
       
      }
        
      if (index >0){
        if (index %2 ==0){
          int xtmp = x -(index/2);
          m_xyindex[sta] = std::make_pair(y,xtmp);
         
        }
        if (index %2 !=0){
          int xtmp = x+ (index+1)/2;
          
          m_xyindex[sta] = std::make_pair(y,xtmp);
         
        }
        
      }
      
    }
    
    
  
    if (m_yFoi[sta] > 0){
      //sta 4 & 5
      if (index >=0 && index <(2* m_xFoi[sta]+1)){
        int xtmp =x+index;
        m_xyindex[sta] = std::make_pair(y-1,xtmp-m_xFoi[sta]);
      }
      if (index >=(2* m_xFoi[sta]+1) && index < 2* (2* m_xFoi[sta]+1)){
        int xtmp = x+index -(2 *m_xFoi[sta]+1);
        m_xyindex[sta] = std::make_pair(y-2,xtmp-m_xFoi[sta]);
      }
      if (index >=(2*(2* m_xFoi[sta]+1)) && index < 3* (2* m_xFoi[sta]+1)){
        int xtmp = x+index -2*(2 *m_xFoi[sta]+1);
        m_xyindex[sta] = std::make_pair(y,xtmp-m_xFoi[sta]);
      }
      
    
      
    }
    
    
  }

}


// Ordered bits also in M4&M5

void L0Muon::CandidateTower::setOrderedPadIndex(int sta, 
                                                int maxXFoi, 
                                                int maxYFoi,   
                                                int offset, int ind)
{
  

 
  int xfoi = maxXFoi;
  int yfoi = maxYFoi;
  
  int x = m_seed.first + xfoi +offset;
  int y = m_seed.second + yfoi;

  int index = ind;
  
  
  if (sta ==2){
    
    m_xyindex[sta]= std::make_pair(m_seed.second, m_seed.first);

  } 
  else {
    if (m_yFoi[sta] == 0){   
      if (index ==0) {
        m_xyindex[sta] = std::make_pair(y-yfoi,x);

  }
  
  
      
      if (index >0){
        if (index %2 ==0){
          int xtmp = x -(index/2);
          m_xyindex[sta] = std::make_pair(y-yfoi,xtmp);

        }
        if (index %2 !=0){
          int xtmp = x+ (index+1)/2;
          
          m_xyindex[sta] = std::make_pair(y-yfoi,xtmp);

        }
        
      }
      
    }
    
    
    
    
  
    if (m_yFoi[sta] > 0){
 
      if (index ==0) {
        m_xyindex[sta] = std::make_pair(y-yfoi,x);

      } 
      else if (index<(2*m_xFoi[sta]+1) && index>0){

        if (index %2 ==0){
          int xtmp = x -(index/2);
          m_xyindex[sta] = std::make_pair(y-yfoi,xtmp);

        }
        if (index %2 !=0){
          int xtmp = x+ (index+1)/2;
          m_xyindex[sta] = std::make_pair(y-yfoi,xtmp);
          
        }
        
      }
      
      else if (index == (2*m_xFoi[sta]+1)){

        m_xyindex[sta] = std::make_pair(y-yfoi-1,x);
      }
      else if (index >(2*m_xFoi[sta]+1) && index<2*(2*m_xFoi[sta]+1)){

        if ((index-2*m_xFoi[sta]+1) %2 ==0){
          int xtmp = x -((index-2*m_xFoi[sta]+1))/2;
          m_xyindex[sta] = std::make_pair(y-yfoi-1,xtmp);
        }
        if ((index-2*m_xFoi[sta]+1) %2 !=0){
          int xtmp = x+ (index-(2*m_xFoi[sta]+1)+1)/2;
          m_xyindex[sta] = std::make_pair(y-yfoi-1,xtmp);
         
        }
      }

      else if (index == 2*(2*m_xFoi[sta]+1)){
  
        m_xyindex[sta] = std::make_pair(y,x);
      }
      else if (index >2*(2*m_xFoi[sta]+1) && index<3*(2*m_xFoi[sta]+1)){
  
        if ((index-2*(2*m_xFoi[sta]+1)) %2 ==0){
          int xtmp = x -((index-(2*(2*m_xFoi[sta]+1)))/2);
          m_xyindex[sta] = std::make_pair(y,xtmp);
        }
        if ((index-2*(2*m_xFoi[sta]+1)) %2 !=0){
          int xtmp = x+ (index-2*(2*m_xFoi[sta]+1)+1)/2;
          m_xyindex[sta] = std::make_pair(y,xtmp);
        }
        
      }
      
           
    
      
    }
    
    
    
    
  }
  
  
}




