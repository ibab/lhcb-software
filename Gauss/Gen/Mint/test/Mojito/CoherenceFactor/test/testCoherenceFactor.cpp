// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/Mint/FitParameter/FitParameter.h"
#include "Mint/Mint/NamedParameter/NamedParameter.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventPattern.h"
#include "Mint/Mojito/FitAmplitude/FitAmpSum.h"

#include "Mint/Mojito/CoherenceFactor/CoherenceFactor.h"

#include <ctime>

#include <iostream>

using namespace std;
using namespace MINT;

int coherenceFactor(){
  time_t startTime = time(0);
  NamedParameter<int> EventPattern("Event Pattern", 421, -321, 211, 211, -211);
  NamedParameter<double> CSFactor("CSFactor", 1);

  DalitzEventPattern pat(EventPattern);
  DalitzEventPattern patBar(EventPattern);
  patBar[0].antiThis();

  cout << "Good morning from coherenceFactor()!" << endl;
  
  FitAmpSum Amps(pat);
  FitAmpSum AmpsBar(patBar);
  if(pat.selfConjugateFS())AmpsBar = Amps.GetCPConjugateSameFitParameters();
  //FitAmpSum AmpsBar(Amps.GetCPConjugateSameFitParameters());

  cout << " I made the amplitudes. Now comes the coherence factor"
       << endl;

  CoherenceFactor CF(Amps, AmpsBar, CSFactor);

  cout << " I created the coherence factor class. Now calculate."
       << endl;

  CF.getCVal();

  cout << "CoherenceFactor() calculated, it's "
       << CF.getCVal()
       << " = " << abs(CF.getCVal()) << " * exp( i * " <<  arg(CF.getCVal()) << " )"
       << " and this took "
       << difftime(time(0), startTime)
       << " s."
       << endl;

  CF.printResult();

  return 0;
}


int main(){

  return coherenceFactor();

}
//
