// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
#include "Mint/AmpRatios.h"
#include "Mint/FitAmplitude.h"

#include <vector>

#include <iostream>

using namespace std;

AmpRatios::AmpRatios(const DalitzEventPattern& pat)
  : _pattern(pat)
{}

bool AmpRatios::getRatios(FitAmpSum& ampSum){
  
  if(! _integrator.initialised()){
    _integrator.initialise(_pattern, &ampSum);
  }

  double allInt = _integrator.getVal();
  cout << " integrated all: " << allInt << endl;

  vector<double> results(ampSum.size());
  
  double ratioSum = 0;
  for(unsigned int i=0; i< ampSum.size(); i++){
    _integrator.resetIntegrand((ampSum.getAmpPtr(i)));
    results[i] = _integrator.getVal();
    double ratio = results[i]/allInt;
    cout << " ratio " << i << ") "
	 << results[i] << " / " << allInt << " = "
	 << ratio << endl;
    results[i] = ratio;
    ratioSum += ratio;
  }

  cout << "\n============ FRACTIONS =================" << endl;
  for(unsigned int i=0; i< ampSum.size(); i++){
    std::string name = ampSum.getAmpPtr(i)->name();

    /*
    double a = ampSum.getAmpPtr(i)->AmpPhase().getAmp();
    double siga = ampSum.getAmpPtr(i)->AmpPhase().getAmp().err();

    double aa = a*a;
    double c = results[i]/aa;
    double sigma = 2*c*a*siga;
    */

    cout << name << "-fraction \t" << results[i] << endl;
    //<< " +/- " << sigma << endl;

  }
  cout << "----------------------------------------\n" << endl;

  cout << " Ratio sum " << ratioSum 
       << " (deviation from 1 due to interference effects) "
       << endl;
  cout << "\n========================================" << endl;


  _integrator.resetIntegrand( &ampSum);

  return true;
}

//

