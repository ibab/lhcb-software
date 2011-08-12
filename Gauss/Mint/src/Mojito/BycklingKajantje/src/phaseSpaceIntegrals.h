#ifndef PHASE_SPACE_INTEGRALS_HH
#define PHASE_SPACE_INTEGRALS_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT

#include "DalitzEventPattern.h"

#include "TF1.h"

#include "IGenFct.h"

double phaseSpaceIntegral2body(const DalitzEventPattern& _pat);
double phaseSpaceIntegral2body(double mum, double d1, double d2);

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

//double phaseSpaceIntegralNbody(const DalitzEventPattern& _pat);

#endif
//
