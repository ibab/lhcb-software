// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:08 GMT
#include "GenWithPhaseSpace.h"
#include <complex>
#include <iostream>
using namespace std;

GenWithPhaseSpace::GenWithPhaseSpace(const char* name
		     , const char* title
		     , RooAbsReal& _Resonances
		     , RooAbsReal& _PhaseSpace
		     )
  : RooAbsPdf(name, title)
  , Resonances("Resonances", "Resonances", this, _Resonances)
  , PhaseSpace("PhaseSpace", "PhaseSpace", this, _PhaseSpace)
{
  
}

GenWithPhaseSpace::GenWithPhaseSpace(const GenWithPhaseSpace& other
		     , const char* name
		     )
  : RooAbsPdf(other, name)
  , Resonances("Resonances", this, other.Resonances)
  , PhaseSpace("PhaseSpace",this, other.PhaseSpace)
{
  
Int_t GenWithPhaseSpace::getGenerator(const RooArgSet &dirVars
				       , RooArgSet &genVars
				       , Bool_t staticInitOK
				       ) const{
  cout << " getGenerator for GenWithPhaseSpace got called" << endl;
  cout << " dirVars: " << dirVars << endl;
  cout << " genVars: " << genVars << endl;
  if(matchArgs(dirVars, genVars, m12sq, m23sq)) return 3;
  cout << " getGenerator returning 0" << endl;
  return 0;
}

void GenWithPhaseSpace::generateEvent(Int_t code){
  if(code == 3){
    PhaseSpace
  }
}

Double_t GenWithPhaseSpace::evaluate() const{
  Double_t ps = PhaseSpace;
  if(ps == 0.0) return 0;
  return Resonances * ps;
}

//
