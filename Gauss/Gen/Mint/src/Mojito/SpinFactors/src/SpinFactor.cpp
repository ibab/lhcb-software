// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT
#include "Mint/Mojito/SpinFactors/SpinFactor.h"
#include "Mint/Mint/Utils/Utils.h"
#include <algorithm> // for swap
#include <cstdlib>

using namespace std;
using namespace MINT;

void SpinFactor::normalOrder( const_counted_ptr<AssociatedDecayTree>& a
			      , const_counted_ptr<AssociatedDecayTree>& b){
  // forget it for now. Sorting done (more conistently) 
  // by "standardSort" called from AmpInitialiser
  // (algorithm in DecayTreeComparisons.h)
  // Just didn't have the guts to remove this, yet.
  return;
  
  // heavier first:
  if(a->getVal().mass() < b->getVal().mass()){
    std::swap(a,b);
    return;
  }else if(a->getVal().mass() > b->getVal().mass()){
    return;
  }else{
    // ok, same mass. Now absolute PDG number
    // (gets weirder ones first)
    if(abs(a->getVal().pdg()) < abs(b->getVal().pdg())){
      std::swap(a,b);
      return;
    }else if(abs(a->getVal().pdg()) < abs(b->getVal().pdg())){
      return;
    }else{
      // only charge, particle/antiparticle different.
      // now, finally, to make it unique, use signed
      // pdg number. Such that positive charges come first.

      if(a->getVal().pdg() < b->getVal().pdg()){
	std::swap(a,b);
	return;
      }
    }
  }
  return; 
}


TLorentzVector SpinFactor::p(int i){
  // momenta of the fsPS.
  if(i >= (int) fsPS.size() || i < 0){
    throw "index out of range";
  }else{
    if(! getEvent()){
      throw "no event";
    }
    if(! fsPS[i] ){
      throw "tree not parsed";
    }

    /*
      debug code, leave it in for now:
    std::cout << " SpinFactor p(" << i << ") called" << endl;
    cout << " fsPs[i] " << fsPS[i] << endl;
    cout << "fsPS[i]->getVal() " << fsPS[i]->getVal() << endl;
    cout << "fsPS[i]->getVal().asi(0) " << fsPS[i]->getVal().asi(0) << endl;

    std::cout << " getEvent() gettin p ("
	      << fsPS[i]->getVal().asi(0)
	      << ")" << std::endl;
    */
    return getEvent()->p(fsPS[i]->getVal().asi(0));
  }
}

double SpinFactor::mRes(const MINT::const_counted_ptr<AssociatedDecayTree>& adt){
  return mRes(adt->getVal());
}
double SpinFactor::mRes(const AssociatedDecayTreeItem& adt){
  return adt.mRes(getEvent());
}

void SpinFactor::printParsing(std::ostream& os) const{
  for(int i=0; i < _nFinal; i++){
    if(0 == fsPS[i]) continue;
    if(i > 0) os << ", ";
    os << " fsPs[" << anythingToString(i) << "] ="
       << fsPS[i]->getVal().name();
  }
  os << endl;
}
