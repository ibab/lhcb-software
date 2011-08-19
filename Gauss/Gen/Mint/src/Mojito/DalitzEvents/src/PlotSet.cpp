// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/PlotSet.h"

void PlotSet::save(const std::string& filename){
  TFile f(filename.c_str(), "RECREATE");
  f.cd();
  for(unsigned int i=0; i< size(); i++){
    ((*this)[i])->Write();
  }
  f.Close();
}
//
