// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#include "Dalitz3Body.h"

Dalitz3Body::RooRealVar piM  ("piM", "piMass", 0.14);
Dalitz3Body::RooRealVar rhoM ("rhoM","rhoMass", 0.7758);
Dalitz3Body::RooRealVar rhoW ("rhoW","rhoWidth", 0.1503);
Dalitz3Body::RooRealVar BM("BM", "BM", 5, 4.8, 5.2);

Dalitz3Body::Dalitz3Body(const char* name
			 , const char* title
			 , RooAbsReal& _m12sq
			 , RooAbsReal& _m23sq
			 )
  : AbsComplexPdf(name, title)
  , _result(0,0)
  , m13sq("m13sq", "m13sq"
	  , m12sq, m23sq, BM, piM, piM, piM)
  , bw12("bw12", "bw12", m12sq, rhoM, rhoW)
  , bw13("bw13", "bw13", m13sq, rhoM, rhoW)
  , bw23("bw23", "bw23", m23sq, rhoM, rhoW)
  , resonances("res", "res"
	       , RooArgList(bw12, bw23, bw13)
	       )
  , phaseSpace("phaseSpace", "phaseSpace"
	       , m12sq, m23sq, BM, piM, piM, piM)
{

}

Dalitz3Body::Dalitz3Body(const Dalitz3Body& other
			 , const char* name
			 )
  : AbsComplexPdf(other, name)
  , _result(other._result)
 

}
//
