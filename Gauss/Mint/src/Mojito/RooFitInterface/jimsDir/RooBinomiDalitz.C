// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
#include "RooBinomiDalitz.h"
#include <complex>
#include <iostream>
using namespace std;

RooBinomiDalitz::RooBinomiDalitz(const char* name
				 , const char* title
				 , RooAbsReal& _ResonancesPlus
				 , RooAbsReal& _ResonancesMinus
				 , RooAbsCategory& _bcharge
				 , RooAbsReal& _prodRatio
		     )
  : RooAbsPdf(name, title)
  , counter(0)
  , ResonancesPlus("ResonancesPlus", "ResonancesPlus", this, _ResonancesPlus)
  , ResonancesMinus("ResonancesMinus", "ResonancesMinus", this, _ResonancesMinus)
  , bcharge("bcharge", "bcharge", this, _bcharge)
  , prodRatio("prodRatio", "prodRatio", this, _prodRatio){
  
}

RooBinomiDalitz::RooBinomiDalitz(const RooBinomiDalitz& other
		     , const char* name
		     )
  : RooAbsPdf(other, name)
  , counter(other.counter)
  , ResonancesPlus("ResonancesPlus", this, other.ResonancesPlus)
  , ResonancesMinus("ResonancesMinus", this, other.ResonancesMinus)
  , bcharge("bcharge", this, other.bcharge)
  , prodRatio("prodRatio", this, other.prodRatio)
{
  
}

Double_t RooBinomiDalitz::evaluate() const{
  Double_t p = ResonancesPlus;
  Double_t m = ResonancesMinus;

  m *= prodRatio; // prodRatio = B^- production / B^+ production

  counter = (counter+1)%1000000;
  if(counter==0) cout << (bcharge == 1 ? " 1" : " not 1" )<< endl;
  if(counter==0) cout << "    " << ResonancesPlus << ", " << ResonancesMinus << endl;
  if(bcharge == 1){
    if(counter==0)cout << " for B+ " << p/(p+m) << endl;
    return p/(p+m);
  } else if(bcharge == -1){
    if(counter==0)cout << " for B- " << m/(p+m) << endl;
    return m/(p+m);
  }else{
    cout << "shit! " << endl;
    return 0;
  }
}

//
