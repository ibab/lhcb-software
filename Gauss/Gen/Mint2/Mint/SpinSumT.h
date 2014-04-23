#ifndef SPIN_SUM_T_HH
#define SPIN_SUM_T_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/SpinSumV.h"

#include "TLorentzVector.h"

#include <iostream>

class SpinSumT{
 protected:
  SpinSumV _sv;
 public:
  SpinSumT(const TLorentzVector& p, double mR)
    : _sv(p, mR)
    {}
  double Sandwich(  const TLorentzVector& lm
		    , const TLorentzVector& ln
		    , const TLorentzVector& ra
		    , const TLorentzVector& rb
		    ){
    
    double manb = _sv.Sandwich(lm, ra) * _sv.Sandwich(ln, rb);
    double mbna = _sv.Sandwich(lm, rb) * _sv.Sandwich(ln, ra);
    double mnab = _sv.Sandwich(lm, ln) * _sv.Sandwich(ra, rb);

    /*
    std::cout << "manb " << manb
	 << ", mbna " << mbna
	 << ", mban " << mnab
	 << std::endl;
    */
    return (1./2.)*(manb + mbna) - (1./3.)*mnab;
    

    
  }
};

#endif

//
