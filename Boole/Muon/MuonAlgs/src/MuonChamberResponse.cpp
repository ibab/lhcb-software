#ifndef MUONALGS_MUONCHAMBERRESPONSE_CPP 
#define MUONALGS_MUONCHAMBERRESPONSE_CPP 1
#include "MuonChamberResponse.h"


MuonChamberResponse::MuonChamberResponse(Rndm::Numbers* flat,Rndm::Numbers* 
                                         poisson, double mean) {
  p_flat=flat ;
  p_poisson=poisson ;
  m_chamberNoiseRate= mean;
}
int MuonChamberResponse::extractNoise(){
  if(m_chamberNoiseRate>0){
    return(int) (*p_poisson)();
  }else{
    return 0;
  }  
} 
#endif
