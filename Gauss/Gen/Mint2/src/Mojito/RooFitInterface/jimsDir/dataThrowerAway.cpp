// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:06 GMT
#include "dataThrowerAway.h"

#include "RooRealVar.h"
#include "RooRandom.h"

#include <string>

double getLargestValue(const RooAbsReal& pdf
		       , RooArgList& emms
		       , RooDataSet* data
		       ){  
  std::string newName = "dataThrowerAwaysCopyOf_";
  newName += pdf.GetName();
  RooAbsReal* pdf_clone = (RooAbsPdf*) pdf.clone(newName.c_str());
  pdf_clone->attachDataSet(*data);
  double largestVal = -9999;
  for(int i=0; i< data->numEntries(); i++){
    const RooArgSet* row = data->get(i);
    /*
    TIterator* emmIter = emms.createIterator();
    RooRealVar* emmPtr;
    while((emmPtr = (RooRealVar*) emmIter->Next())){
      *emmPtr = row->getRealValue(emmPtr->GetName());
    }
    */
    double val = pdf_clone->getVal();
    if(val > largestVal || i==0) largestVal = val;
  }
  delete pdf_clone;
  return largestVal;
}

RooDataSet* dataThrowerAway(const RooAbsReal& pdf
			    , RooArgList& emms
			    , RooDataSet* data
			    , int maxEvents
			    , double largestVal
			    , bool saveOldData
			    ){  
  bool debug = false;
  RooRealVar dta_weight("dta_weight", "dta_weight", 0);
  RooDataSet dw("dw", "dw", RooArgSet(dta_weight));

  RooArgSet original = *(data->get(0));
  original.Print("v");

  std::string newName = "dataThrowerAwaysCopyOf_";
  newName += pdf.GetName();
  RooAbsReal* pdf_clone = (RooAbsPdf*) pdf.clone(newName.c_str());
  pdf_clone->attachDataSet(*data);

  double thisLargestVal=-99999;
  for(int i=0; i< data->numEntries(); i++){
    const RooArgSet* row = data->get(i);
    /*
    TIterator* emmIter = emms.createIterator();
    RooRealVar* emmPtr;
    while((emmPtr = (RooRealVar*) emmIter->Next())){
      *emmPtr = row->getRealValue(emmPtr->GetName());
    }
    */
    double val = pdf_clone->getVal();
    dta_weight.setVal(val);
    dw.add(RooArgSet(dta_weight));
    if(debug)cout << " " << i << ") " << val << endl;
    if(val > thisLargestVal || i==0) thisLargestVal = val;
  }
  if(largestVal < -9998) largestVal = thisLargestVal;

  cout << "dataThrowerAway: largestVal " << largestVal << endl; 
  //  data->merge(&dw);
  RooRealVar dta_acc("dta_acc", "dta_acc", 0);
  RooDataSet dacc("dacc", "dacc", RooArgSet(dta_acc));

  int nEvents=0;
  for(int i=0; i< dw.numEntries(); i++){
    const RooArgSet* row = dw.get(i);
    double val = row->getRealValue("dta_weight");
    double rnd = RooRandom::randomGenerator()->Rndm() * largestVal;

    double acc = (rnd < val ? 1.0 : 0.0);
    if(acc > 0.5) nEvents++;
    if(nEvents > maxEvents && maxEvents > 0) acc = 0.;
    dta_acc.setVal(acc);
    dacc.add(RooArgSet(dta_acc));
    if(debug)cout << " " << i << ") " << val << ", " << acc << endl;
  }
  data->merge(&dacc);
  RooDataSet* tmpData = (RooDataSet*) data->reduce("dta_acc > 0.5");
  RooDataSet* newData = (RooDataSet*) tmpData->reduce(original);
  //  delete tmpData;
  if( false && ! saveOldData){
    delete data;
    data = newData;
  }

  if(debug){
    for(int i=0; i< newData->numEntries(); i++){
      const RooArgSet* row = newData->get(i);
      row->Print("v");
    }
  }
  delete pdf_clone;
  return newData;
}
