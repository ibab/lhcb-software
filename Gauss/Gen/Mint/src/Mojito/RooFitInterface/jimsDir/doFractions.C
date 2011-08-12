// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#include "doFractions.h"

using namespace std;

std::map<std::string, Double_t> doFractions(const ComplexSum& comsum
					    , RooDataSet* data
					    , int maxEvents
					    ){
  bool debug = true;

  std::string newName = "doFractionsCopyOf_";
  newName += comsum.GetName();
  ComplexSum* comsum_clone = (ComplexSum*) comsum.clone(newName.c_str());
  comsum_clone->attachDataSet(*data);

  std::map<std::string, Double_t> valMap;
  int printEverySoOften = data->numEntries()/10;
  if(printEverySoOften<=0) printEverySoOften=1;

  for(int i=0; i< data->numEntries(); i++){
    if(i==maxEvents) break;
    const RooArgSet* row = data->get(i);
    comsum_clone->addValues(valMap);
    if(debug && i%printEverySoOften==0){
      cout << "===========  " << i << "  ===================";
      for(std::map<std::string, Double_t>::iterator it = valMap.begin();
	  it != valMap.end(); it++){
	cout << "\n    " << it->first << ": " << it->second;
      }
      cout << endl;
    }
  }

  delete comsum_clone;

  Double_t totalVal = valMap["total"];
  if(totalVal <= 0){
    cout << "warning: total = 0 - no fractions calculated" << endl;
    return valMap;
  }

  for(std::map<std::string, Double_t>::iterator it = valMap.begin();
      it != valMap.end(); it++){
    it->second /= totalVal;
  }

  if(debug){
    cout << "\n\n\n===========  FRACTIONS  ===================";
    for(std::map<std::string, Double_t>::iterator it = valMap.begin();
	it != valMap.end(); it++){
      cout << "\n    " << it->first << ": " << it->second;
    }
    cout << "\n\n" << endl;
  }

  return valMap;
}
