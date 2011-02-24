// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT

#include "RooArgSet.h"
#include "RooRandom.h"
#include "mark.h"

RooDataSet* mark( RooDataSet* data
		  , RooCategory c
		  ){  
  bool debug = false;
  RooDataSet dc("dc", "dc", RooArgSet(c));

  for(int i=0; i< data->numEntries(); i++){
    dc.add(RooArgSet(c));
  }
  data->merge(&dc);

  return data;
}
