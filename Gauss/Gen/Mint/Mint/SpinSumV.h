#ifndef SPIN_SUM_V_HH
#define SPIN_SUM_V_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "TLorentzVector.h"
#include "Mint/Utils.h"
#include <iostream>

class SpinSumV{ // spin sum for Vector->PP
 protected:
  TLorentzVector _p; // pA + pB (pA, pB are the 4-mom of dgtrs)
  double _mR;        // nominal mass of resonance.
 public:
  SpinSumV(const TLorentzVector& p, double mR)
    : _p(p)
    , _mR(mR)
    {}

  TLorentzVector Dot(const TLorentzVector& rhs) const{
    /*
    std::cout << " rhs = " << rhs
	      << ", _p = " << _p
	      << ", _mR " << _mR
	      << " _p.Dot(rhs) " << _p.Dot(rhs)
	      << " _mR * _mR " << _mR*_mR
	      << ", ( _p.Dot(rhs) / _mR*_mR ); " << ( _p.Dot(rhs) / _mR*_mR )
	      << std::endl;
    */
    return -rhs + _p * ( _p.Dot(rhs) / (_mR*_mR) );
  }
  double Sandwich( const TLorentzVector& lhs
			   , const TLorentzVector& rhs) const{
    return lhs.Dot(this->Dot(rhs));
  }
  
};

// this implements, I hope, 
// hep-ex 0011065v2, page 5 (CLEO)
#endif
//
