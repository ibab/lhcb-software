#ifndef DKsPiPiResonances_HH
#define DKsPiPiResonances_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:11 GMT
#include "TMath.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooRealVar.h"
#include "DcyAmplitude.h"
#include "DcyGSAmplitude.h"
#include "DcyNonRes.h"

RooArgList& makeResonance(RooAbsReal& m12sq
			  , RooAbsReal& m23sq
			  , RooAbsReal& m13sq
			  , std::string Prefix=""
			  );
#endif
//
