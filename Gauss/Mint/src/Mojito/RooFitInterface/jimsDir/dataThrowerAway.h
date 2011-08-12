// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#ifndef DATATHROWERAWAY
#define DATATHROWERAWAY

#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooArgList.h"
#include "RooDataSet.h"
double getLargestValue(const RooAbsReal& pdf
		       , RooArgList& emms
		       , RooDataSet* data
		       );
RooDataSet* dataThrowerAway(const RooAbsReal& pdf
			    , RooArgList& emms
			    , RooDataSet* data
			    , int maxEvents = -1
			    , double largestValue = -9999
			    , bool saveOldData = false
			    );

#endif
//
