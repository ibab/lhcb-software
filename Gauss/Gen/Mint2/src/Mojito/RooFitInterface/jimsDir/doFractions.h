// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#ifndef DOFRACTIONS_H
#define DOFRACTIONS_H

#include "RooDataSet.h"
#include "ComplexSum.h"

#include <string>
#include <map>

std::map<std::string, Double_t> doFractions(const ComplexSum& comsum
					    , RooDataSet* data
					    , int maxEvents=-9999
					    );
#endif
