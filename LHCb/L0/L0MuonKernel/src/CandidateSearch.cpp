//#include <time.h>
#include "L0MuonKernel/CandidateSearch.h"
#include <iostream>


L0Muon::CandidateSearch::CandidateSearch(){

  m_extraM1[0] =  0 ;     
  m_extraM1[1] = +4 ;
  m_extraM1[2] = -4 ;
  m_extraM1[3] = +7 ;
  m_extraM1[4] = -7 ;
  m_extraM1[5] = +11 ;
  m_extraM1[6] = -11 ;
  m_extraM1[7] = +14 ;
  m_extraM1[8] = -14 ;
  m_extraM1[9] = +18;
  m_extraM1[10] = -18;
  m_extraM1[11] = +21;
  m_extraM1[12]= -21;
  m_extraM1[13] = +25;
  m_extraM1[14]= -25;

  
  
  m_offset =0;
  m_offsetM1 =0;
  
  for (int sta =0; sta< 5; sta++){
    m_found[sta] = false; 
  }

  for (int sta =0; sta< 5; sta++){
    if (sta ==2){
      m_hitpos[sta] = 0;
    }else {
      m_hitpos[sta] = 999;
    }
    
  }
  
  m_ignoreM1 = false;
  
}



L0Muon::CandidateSearch::~CandidateSearch(){
}


/*
   Look for a hit in the input bitset. 
   If a hit is found, set the m_found flag for 
   the corresponding station and fill the hitpos
   with the bit position.
   For station M2, fill the m_offset variable with 
   the hit position,idem for M1 with the m_offsetM1.
*/
void L0Muon::CandidateSearch::searchInSta(int sta, 
                                          boost::dynamic_bitset<> & bits){

  
  // Station M3
  if (sta ==2){
    m_found[sta] = true;
    return;
  }

  // Station M1,M2,M4 and M5
  bool val = false ;
  for (boost::dynamic_bitset<>::size_type i=0; i <bits.size(); i++){
    val = bits.test(i);
    if (val == true ) {
      m_hitpos[sta] = i;
      break ;
    }
  }

  m_found[sta]=val;	    

  //  Station M2
  if (sta==1) m_offset = m_hitpos[sta];

  // Station M1
  if (sta==0) m_offsetM1 = m_hitpos[sta];


}






void L0Muon::CandidateSearch::anyBitsInSta(int sta, 
					   boost::dynamic_bitset<> & bits){
  if (bits.any()== true){
    m_found[sta]= true;
  } else {
   m_found[sta]=false;
   
  }
}




int L0Muon::CandidateSearch::makeExtrapolation( ){
  //m_extraM1[m_offset] ;
  //*log << MSG::DEBUG << "parameter for extrapolation in M1" << " " 
  //   << m_extraM1[m_offset] << " " << "m_offset" << " " << m_offset
  //   << endreq ;
  return m_extraM1[m_offset];
}




bool L0Muon::CandidateSearch::CandidateFound(){
  if (m_ignoreM1 ){
    if (m_found[1]==true && m_found[3]==true&& 
        m_found[4]==true){
      m_isFull=true;
    } else {
      m_isFull = false;
    }
  } else if (!m_ignoreM1){
    if (m_found[0]==true && m_found[1]==true && m_found[3]==true&& 
        m_found[4]==true){
      m_isFull=true;
    }  else {
      m_isFull = false;
    }
    
  }
  

  
  return m_isFull;
}



 
void L0Muon::CandidateSearch::setCandidateAddrs(std::pair<int, int> & sd){

  if (m_isFull == true){
   
    // address in M1 
    signed long int offsetM1=0 ;
    if (m_offsetM1 !=0){
      
      if (m_offsetM1%2 ==0){
        offsetM1 = - (m_offsetM1/2);
      } else {
        offsetM1 = (m_offsetM1 +1)/2;
      }
    }


    if (offsetM1>0) {
      offsetM1 = int((offsetM1+1)/2);
    } else {
      offsetM1 = int((offsetM1-1)/2);
    }


    boost::dynamic_bitset<> addrM1(3, offsetM1);
    m_addrM1 = addrM1 ;
  

  // address in M2 (delta M3-M2) 
    signed long int dm2m3 =0 ;
      if (m_offset !=0 ){
      if (m_offset%2 ==0){
        dm2m3 = - (m_offset/2);
      } else {
        dm2m3 = (m_offset +1)/2;
      }
      
      }
      
    
    boost::dynamic_bitset<> addrM2(4, dm2m3);
    m_addrM2 = addrM2 ;


  // address in M3
    unsigned long int nbx = sd.first ;
    unsigned long int nby = sd.second ;
    boost::dynamic_bitset<> addrM3x(7, nbx );
    boost::dynamic_bitset<> addrM3y(2, nby);
    addrM3x[5] = addrM3y[0];
    addrM3x[6] = addrM3y[1];
    m_addrM3 = addrM3x ;
   
    
  } else {

    unsigned long int nb = 0;
    boost::dynamic_bitset<> addrM3(7,nb);
    m_addrM3 = addrM3;
    boost::dynamic_bitset<> addrM2(4,nb);
    m_addrM2 = addrM2;
    boost::dynamic_bitset<> addrM1(3,nb);
    m_addrM1 = addrM1;
  }
       

}





boost::dynamic_bitset<>  L0Muon::CandidateSearch::getAddrs(int sta){
  boost::dynamic_bitset<> bits;
  if (sta ==2){ 
    //for (boost::dynamic_bitset<>::size_type i=0; i< m_addrM3.size(); i++){
    //}
    //return m_addrM3;
    bits = m_addrM3;

  }
  if (sta ==1){
    //for (boost::dynamic_bitset<>::size_type i=0; i< m_addrM2.size(); i++){
    // }
    //return m_addrM2;
    bits = m_addrM2;
    
  }
  if (sta ==0){
    // for (boost::dynamic_bitset<>::size_type i=0; i< m_addrM1.size(); i++){
    //}
    //return m_addrM1;
    bits = m_addrM1;
    
  }
  return bits;
  

}



boost::dynamic_bitset<>  L0Muon::CandidateSearch::getCandidateAddrs(){

  if (m_addr.size()!=0){
    m_addr.clear();
  }

  boost::dynamic_bitset<> bit ;
  
  bit = getAddrs(0);
  for (boost::dynamic_bitset<>::size_type ibit=0; ibit < bit.size(); ibit++){
    bool val = bit[ibit];
    m_addr.push_back(val);
  }
  m_addr.push_back(0);
  bit = getAddrs(1);
  for (boost::dynamic_bitset<>::size_type ibit=0; ibit < bit.size(); ibit++){
    bool val = bit[ibit];
    m_addr.push_back(val);
  }
  bit = getAddrs(2);
  for (boost::dynamic_bitset<>::size_type ibit=0; ibit < bit.size(); ibit++){
    bool val = bit[ibit];
    m_addr.push_back(val);
  }

  return m_addr ;
}


void L0Muon::CandidateSearch::resetBits()
{
  if (m_addrM3.any()){
    m_addrM3.reset();
  }
  
  if (m_addrM2.any()){
    m_addrM2.reset();
  }
  
  if (m_addrM1.any()){
    m_addrM1.reset();
  }
  
  if (m_addr.any()){
    m_addr.reset();
  }
  
  m_offset =0;
  m_offsetM1 =0;
  
  for (int sta =0; sta< 5; sta++){
    m_found[sta] = false; 
  }

  for (int sta =0; sta< 5; sta++){
    if (sta ==2){
      m_hitpos[sta] = 0;
    }else {
      m_hitpos[sta] = 999;
    }    
  }
  //m_offset =0;
  //m_offsetM1 =0;
  
  //for (int sta =0; sta< 5; sta++){
  //m_found[sta] = false; 
  //}

  //for (int sta =0; sta< 5; sta++){
  //m_hitpos[sta] = 999; 
  //}
  
  //m_useful = 0;

}


/*
  Method to acces the m_hitpos for station sta ?
  (obscure to me (Julien): why not symply:
  int L0Muon::CandidateSearch::getHitPos(int sta){
     if (sta<0 && sta>=5) return 999; // if necessary ??
     return m_hitpos[sta];
  }
*/
int L0Muon::CandidateSearch::getHitPos(int sta)
{
  int hitpos =999;
  for (int i =0; i<5; i++){
    if ( i == sta){
      hitpos= m_hitpos[i];
    }
  }
  return hitpos;
}
















