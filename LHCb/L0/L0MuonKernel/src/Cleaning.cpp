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
              //&& (((*iv).second-(*jv).second)<=1 
              // && ((*iv).second-(*jv).second>=-1))
              ){
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
   //std::vector< boost::dynamic_bitset<> >::iterator irow2;
   
   for (irow = map.begin(); irow != map.end(); irow ++){
     
     for (boost::dynamic_bitset<>::size_type icol=0; icol <(*irow).size();
          icol++){
       (*irow).test(icol) ;
       if ( (*irow).test(icol) ) {
         //===========================
         /*
         for (irow2 = irow+1; irow2 < irow+2; irow2++){
           if (irow2 != map.end()) {
             
           for (boost::dynamic_bitset<>::size_type icol2=0; icol2 <(*irow2).size();
          icol2++){
             if ( icol2 < (*irow).size()){
             if (icol2 ==icol){               
               if ( (*irow2).test(icol2) ) {

                 (*irow2).reset(icol2);
               }
             }
             }
             
           
             
           }
           
         }
         
         }
         
          
         */       
         
         
         //
         boost::dynamic_bitset<>::size_type jcol=icol+1;
         if (jcol < (*irow).size()){
           
           unsigned int k =2;
           for (boost::dynamic_bitset<>::size_type jcol=icol+1; 
                // jcol < icol+5;jcol++){
                jcol < icol+k;jcol++){
             
               if ( jcol < (*irow).size()){
               //jcol < (*irow).size(); jcol++){
           
               if ( (*irow).test(jcol))
               {
                 (*irow).reset(jcol);
                  k++;
                 
               }
               
               
               
               
               
               }
               
               
               
               
           }
           
           
           
           
           
           
           
         }
         
         
         
        
         
       }
       
       
       
       
       
       
     }
     
     
       
     
   }
   
   
   
   
   
   //}

   
   
   
   
            
  return map;

}




	                                   
