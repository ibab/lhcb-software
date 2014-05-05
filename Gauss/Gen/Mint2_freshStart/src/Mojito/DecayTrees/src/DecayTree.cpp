// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/DecayTree.h"
#include "Mint/DecayTreeComparisons.h"
#include "Mint/NamedDecayTreeList.h"

#include <sstream>
#include <iostream>

using namespace std;

/* DecayTree itself is just a typedef 
   Here we add a few functions.
*/

void anti(DecayTree& dt){
  //  if(! dt.getVal().antiThis()) return;
  // no point continuing if this has no antiparticle.
  // .. there is a point!
  dt.getVal().antiThis();

  for(int i=0; i<dt.nDgtr(); i++){
    anti(*(dt.getDgtrTreePtr(i)));
  }
}

std::string uniqueName(const DecayTree& dt_in){
  DecayTree dt(dt_in);
  standardSort(dt);
  return dt.oneLiner();
}

MultiQuarkContent netQuarkContent(const DecayTree& dt_in){
  if(dt_in.getVal().isNonResonant()){
    MultiQuarkContent quarks;
    for(int i=0; i < dt_in.nDgtr(); i++){
      quarks += netQuarkContent(*dt_in.getDgtrTreePtr(i));
    }
    return quarks;
  }else{
    return dt_in.getVal().props()->netQuarkContent();
  }
}
