#include "L0MuonKernel/Cleaning.h"

L0Muon::Cleaning::Cleaning(){
}

L0Muon::Cleaning::~Cleaning(){
}

std::vector< L0Muon::Cleaning::HitIndex >   L0Muon::Cleaning::cleanSeed(
                                       std::vector< HitIndex >& seedsvec){
  std::vector< HitIndex >::iterator iv;
  std::vector< HitIndex >::iterator jv;
  
  if (seedsvec.end()-seedsvec.begin()>1){
  for (iv = seedsvec.begin(); iv <= seedsvec.end(); iv++){
    
    for (jv = iv+1 ; jv <= seedsvec.end(); jv++){
      
      if ( (*iv).first !=0 || (*iv).second!=0){
        if ( (*jv).first !=0 || (*jv).second!=0){
          if ((((*iv).first-(*jv).first)<=1 
               && ((*iv).first-(*jv).first>=-1))  
              && (((*iv).second-(*jv).second)<=1 
                  && ((*iv).second-(*jv).second>=-1))){
            seedsvec.erase(jv);
            return seedsvec;
          }
        }
      }
    }
  }
  }
  
  return seedsvec;
  
}
	


 L0Muon::Cleaning::SeedsMap   L0Muon::Cleaning::cleanSeed(SeedsMap & map){

   std::vector< boost::dynamic_bitset<> >::iterator irow;
   
   for (irow = map.begin(); irow != map.end(); irow ++){
     
     for (boost::dynamic_bitset<>::size_type icol=0; icol <(*irow).size();
          icol++){
       (*irow).test(icol) ;
       if ( (*irow).test(icol) ) {
         boost::dynamic_bitset<>::size_type jcol=icol+1;
         if (jcol < (*irow).size()){
           
           for (boost::dynamic_bitset<>::size_type jcol=icol+1; 
              jcol < icol+2;jcol++){
           
             if ( (*irow).test(jcol))
             {
               (*irow).reset(jcol);
               
             }
             
           }
         }
         
         
       }
       
       
       
     }
     
     
   }
   
   
   
   
            
  return map;
  
 }



	                                   
