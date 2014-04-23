// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT

#include "DcyAmplitude.h"

#include <iostream>
using namespace std;

Double_t DcyAmplitude::twoBody_dgtPsq_in_MumsFrame(Double_t mumM
						   , Double_t mA
						   , Double_t mB
						   ) const{
  double Msq = mumM*mumM;
  if(Msq <=0) return -9999;

  double mpsq = (mA + mB)*(mA + mB);
  double mmsq = (mA - mB)*(mA - mB);
  double num = (Msq - mpsq)*(Msq - mmsq);

  return num/(4*Msq);

}

Double_t DcyAmplitude::prSq() const{
  Double_t returnVal 
    = twoBody_dgtPsq_in_MumsFrame( mR, m1_nominal, m2_nominal);
  //= twoBody_dgtPsq_in_MumsFrame( mR, m1, m2);
  if(returnVal < 0){
    return 0;
    cout << " DcyAmplitude::prSq()"
	 << " suspicious... pr < 0. This is for:"
	 << "\n   mumM = " << mR 
	 << ", m1_nominal " << m1_nominal
	 << ", m2_nominal " << m2_nominal
	 << endl;
  }
  return returnVal;
}
Double_t DcyAmplitude::pABSq() const{
  Double_t returnVal = twoBody_dgtPsq_in_MumsFrame( sqrt(s), m1, m2);
  if(returnVal < 0){
    cout << " DcyAmplitude::prSq()"
	 << " suspicious... pr < 0. This is for:"
	 << "\n   mumM = " << mR 
	 << ", m1 " << m1
	 << ", m2 " << m2
	 << endl;
  }
  return returnVal;
}
Double_t DcyAmplitude::Fr() const{
  // (ratio of) Blatt-Weisskopf penetration factors
  // as in BELLE note hep-ex/0406067, pg 5.
  if(spin == 0) return 1;
  
  if(s < 0) return -9999;
  double m12 = sqrt(s);
  
  double R_pr_sq  = R*R * prSq();
  double R_pAB_sq = R*R * pABSq();
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
  std::cout << "DcyAmplitude::Fr() of instance " 
	    << GetName() 
	    << ": ERROR! Can't deal with spin == " 
	    << spin
	    << ". Please improve me!"
	    << std::endl;
  return -9999;


}
Double_t DcyAmplitude::GofM() const{
  double densq = prSq();  
  if(false && densq <=0){
    cout << "DcyAmplitude::GofM(), instance " << GetName()
	 << ": ERROR: prSq() = " << prSq() << endl;
    return -9999;
  }
  double numsq = pABSq();
  if(numsq < 0){
    cout << "DcyAmplitude::GofM(), instance " << GetName()
	 << ": ERROR: pABSq() = " << pABSq() << endl;
    return -9999;
  }

  double pratio = 1;
  if(densq > 0 && numsq >=0){
    pratio = sqrt(numsq/densq);
  }
  double pratio_to_2Jplus1 = 1;
  for(int i=0; i < 2*spin + 1; i++){
    pratio_to_2Jplus1 *= pratio;
  }

  densq = s;

  if(densq <= 0){
    cout << "DcyAmplitude::GofM(), instance " << GetName()
	 << ": ERROR: s = " << s << endl;
    return -9999;
  }
  double mRatio = mR/sqrt(densq);
  
  double thisFr = Fr();
  return G * pratio_to_2Jplus1 * mRatio * thisFr*thisFr;
}
Double_t DcyAmplitude::sJ() const{
  //  cout << " I, " << GetName() << " am calling spinFactor: " << endl;
  Double_t returnVal = spinFactor;
  if(returnVal < -9998){
    cout << " in DcyAmplitude " << GetName()
	 << " spin factor screwed: " << returnVal
	 << endl;
  }
  return returnVal;
  //  cout << " >>>> done" << endl;
}
Double_t DcyAmplitude::den() const{
  Double_t r = mR*mR - s;
  Double_t c = mR*GofM();
  return r*r + c*c; 
}

Double_t DcyAmplitude::ReBW() const{
  return (mR*mR - s)/den();
}
Double_t DcyAmplitude::ImBW() const{
  return  mR*GofM()/den();
} 
std::complex<Double_t> DcyAmplitude::BW() const{
  return std::complex<Double_t>(ReBW(), ImBW());
}
Double_t DcyAmplitude::ReM() const{
  return amp*cos(phase);
}
Double_t DcyAmplitude::ImM() const{
  return amp*sin(phase);
}
std::complex<Double_t> DcyAmplitude::M() const{
  return std::complex<Double_t>(ReM(), ImM());
}
Double_t DcyAmplitude::evaluate() const{ 
  if(s < 0){
    _result = 0;
    return 0;
  }
  if(sqrt(s) < (m1 + m2)){
    _result = 0;
    return 0;
  }
  
  _result = Fr() * sJ() * M()*BW();
  return _result.real()*_result.real() + _result.imag()*_result.imag();
}
std::complex<Double_t> DcyAmplitude::getCVal() const{
  getVal();
  if(false)std::cout << " for " << s.arg().GetName() 
		     << " = " << s 
		     << " I, " << this->GetName() 
		     << ", return "  << _result 
		     << " (" << abs(_result) << ")" 
		     << std::endl;
  return _result;
}


ClassImp(DcyAmplitude)

//
