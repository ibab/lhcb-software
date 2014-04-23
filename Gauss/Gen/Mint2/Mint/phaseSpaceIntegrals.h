#ifndef PHASE_SPACE_INTEGRALS_HH
#define PHASE_SPACE_INTEGRALS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "Mint/DalitzEventPattern.h"
#include "Mint/IGenFct.h"
#include "Mint/lambda.h"
#include "Mint/CLHEPPhysicalConstants.h"

#include "TF1.h"

#include <iostream>
#include <cmath>


//double phaseSpaceIntegral2body(const DalitzEventPattern& _pat);
//double phaseSpaceIntegral2body(double mum, double d1, double d2);

inline double phaseSpaceIntegral2body(double mum, double d1, double d2){
  if(mum <=0 ) return 0;
  if(mum < d1 + d2) return 0;

  double la = lambda(mum*mum, d1*d1, d2*d2);
  if(la <= 0) return 0;

  return pi * sqrt(la)/(2*mum*mum);
}

inline double phaseSpaceIntegral2body(const DalitzEventPattern& _pat){
  if(_pat.size() != 3){
    std::cout << "phaseSpaceIntegral2body: wrong pattern " << _pat << std::endl;
  }
  double mum = _pat[0].mass();
  double d1  = _pat[1].mass();
  double d2  = _pat[2].mass();
  return phaseSpaceIntegral2body(mum, d1, d2);
}

class PhaseSpaceIntegral3body{
 protected:
  static TF1* _f;
 public:
  PhaseSpaceIntegral3body();
  double getVal(const DalitzEventPattern& _pat);
  double getVal(double mum, double d1, double d2, double d3);
};

class PhaseSpaceIntegral4body{
 protected:
  static TF1* _f;
 public:
  PhaseSpaceIntegral4body();
  double getVal(const DalitzEventPattern& _pat);
  double getVal(double mum, double d1, double d2
		, double d3, double d4);
  double getValCheck(const DalitzEventPattern& _pat);
  double getValCheck(double mum, double d1, double d2
		, double d3, double d4);
};

double d3body_by_ds12(Double_t* x, Double_t* p);
double d4body_by_ds12(Double_t* x, Double_t* p);


class PhaseSpaceIntegral4bodyWith_s12s34{
 protected:
  IGenFct* _s12Fct, *_s34Fct;
  DalitzEventPattern _pat;
 public:
  PhaseSpaceIntegral4bodyWith_s12s34(IGenFct* s12f
				     , IGenFct* s34f
				     , const DalitzEventPattern& pat
				     );
  double getVal() const;
};

class PhaseSpaceIntegral4bodyWith_s123s12{
 protected:
  IGenFct* _s123Fct, *_s12Fct;
  DalitzEventPattern _pat;
 public:
  PhaseSpaceIntegral4bodyWith_s123s12(IGenFct* s123f
				     , IGenFct* s12f
				     , const DalitzEventPattern& pat
				     );
  double getVal() const;
};

double phaseSpaceIntegral_upTo4body(const DalitzEventPattern& pat);

//double phaseSpaceIntegralNbody(const DalitzEventPattern& _pat);

#endif
//
