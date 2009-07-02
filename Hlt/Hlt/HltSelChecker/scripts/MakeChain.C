#include "TChain.h"
#include "TFile.h"
#include <iostream>
#include <sstream>

TChain* MakeChain(TString head, int max, TString loc = "TriggerTuple/EventTuple"){
  TChain* MB = new TChain(loc);
  
  for ( int i=0 ; i<=max; i++){
    TString s;
    std::stringstream out;
    out << i;
    s = out.str();
    TString name = head+s+TString(".root") ;
    std::cout << name << std::endl;
    TFile f;
    if (f.Open(name)){
      MB->Add(name);
    } // else std::cout << " No " << name << std::endl ;
  }
  
  return MB;
}
