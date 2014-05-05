// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:07 GMT
#include "DKsPiPi.h"
#include "DKsPiPiResonances.h"
using namespace DKsPiPiResonances;

ClassImp(DKsPiPi)

Dalitz3Body::DKsPiPi(const char* name
		     , const char* title
		     , RooAbsReal& _m12sq
		     , RooAbsReal& _m23sq
		     , RooArgList& ResonanceList
		     )
  : AbsComplexPdf(name, title)
  , _result(0,0)
  , m13sq("m13sq", "m13sq"
	  , m12sq, m23sq, DZeroM, piPlusM, KsM, piMinusM)
  , resSum("resSum", "resSum", ResonanceList)
{
}

Dalitz3Body::DKsPiPi(const Dalitz3Body& other
			 , const char* name
			 )
  : AbsComplexPdf(other, name)
  , _result(other._result)
 

}
//
