// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "BlattWeisskopf.h"
#include <iostream>

Double_t twoBody_dgtPsq_in_MumsFrame(Double_t mumM
				   , Double_t m1
				   , Double_t m2
				   ){
  double Msq = mumM*mumM;
  if(Msq <=0) return -9999;

  double mpsq = (m1 + m2)*(m1 + m2);
  double mmsq = (m1 - m2)*(m1 - m2);
  double num = (Msq - mpsq)*(Msq - mmsq);

  return num/(4*Msq);

}
Double_t twoBody_dgtP_in_MumsFrame(Double_t mumsM
				   , Double_t m1
				   , Double_t m2
				   ){
  double psq = twoBody_dgtPsq_in_MumsFrame(mumsM, m1, m2);
  if(psq < 0) return -9999;
  return sqrt(psq);
}

Double_t BlattWeisskopf::evaluate() const{
  if(spin == 0) return 1;
  
  if(s12 < 0) return -9999;
  double m12 = sqrt(s12);
  double pABsq = twoBody_dgtPsq_in_MumsFrame( m12, m1, m2);
  double prsq  = twoBody_dgtPsq_in_MumsFrame(mumM, m1, m2);
  
  double R_pr_sq  = R*R * prsq;
  double R_pAB_sq = R*R * pABsq;
  if(spin == 1){
    return sqrt( (1. + R_pr_sq)/(1. + R_pAB_sq) );
  }
  if(spin == 2){
    double R_pr_4  = R_pr_sq*R_pr_sq;
    double R_pAB_4 = R_pAB_sq*R_pAB_sq;
    
    return sqrt( (  9. + 3.*R_pr_sq  + R_pr_4)
		 /( 9. + 3.*R_pAB_sq + R_pAB_4) 
		 );
  }
  std::cout << "BlattWeisskopf, " << GetName() 
	    << ": ERROR! Can't deal with spin == " 
	    << spin
	    << ". Please improve me!"
	    << std::endl;
  return -9999;
}

ClassImp(BlattWeisskopf)

//
