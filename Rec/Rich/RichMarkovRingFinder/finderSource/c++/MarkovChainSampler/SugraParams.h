
#ifndef LESTER_SUGRA_PARAMS_H
#define LESTER_SUGRA_PARAMS_H

#include "MarkovChainSampler/Ntuple.h"

namespace Lester {
  
  class SugraParams :
    public Ntuple<5,double> {
  public:
    SugraParams() {
      // Beware! Variables are uninitialised!
    };
    SugraParams(const double mZero,
	  const double mHalf,
	  const double A0,
	  const double tanBeta,
	  const int sgnMu) {
      setMZero(mZero);
      setMHalf(mHalf);
      setA0(A0);
      setTanBeta(tanBeta);
      setSgnMu(sgnMu);
    };
  public:
    void setMZero(const double mZero) { (*this)[0] = mZero; };
    void setMHalf(const double mHalf) { (*this)[1] = mHalf; };
    void setA0(const double A0) { (*this)[2] = A0; };
    void setTanBeta(const double tanBeta) { (*this)[3] = tanBeta; };
    void setSgnMu(const int sgnMu) { (*this)[4] = sgnMu; };
    inline double mZero() const { return (*this)[0]; };
    inline double mHalf() const { return (*this)[1]; };
    inline double A0() const { return (*this)[2]; };
    inline double tanBeta() const { return (*this)[3]; };
    inline int sgnMu() const { return static_cast<int>((*this)[4]); };
    inline bool isReasonable() const {
      return 
	mZero()>=0 &&
	mHalf()>=0 &&
	tanBeta()>=0 &&
	abs(sgnMu())==1;
    };
  };

};

#endif
