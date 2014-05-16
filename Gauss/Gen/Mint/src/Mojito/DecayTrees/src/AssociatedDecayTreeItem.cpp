// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#include <vector>

#include "Mint/AssociatedDecayTreeItem.h"
#include "Mint/DecayTreeItem.h"
#include "Mint/IDalitzEvent.h"

AssociatedDecayTreeItem::AssociatedDecayTreeItem(int i)
  : DecayTreeItem(i)
{
  _asi.clear();
}
AssociatedDecayTreeItem::AssociatedDecayTreeItem(const AssociatedDecayTreeItem& other)
  : DecayTreeItem(other)
    , _asi(other._asi)
{}
AssociatedDecayTreeItem::AssociatedDecayTreeItem(const DecayTreeItem& other)
  : DecayTreeItem(other)
{
  _asi.clear();
}
const std::vector<int>& AssociatedDecayTreeItem::asi() const{ 
  return _asi;
}
int AssociatedDecayTreeItem::asi(int i) const{ 
  return _asi[i];
}

void AssociatedDecayTreeItem::print(std::ostream& out) const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    out << _pdg_id;
  }else{
    out << pp->name() 
	<< "(" << pp->SVPAT() <<")";
  }
  if(! this->_asi.empty()){
    out << "[";
    for(unsigned int i=0; i< _asi.size(); i++){
      out << this->_asi[i]; 
    }
    out << "]";
  }
}


TLorentzVector AssociatedDecayTreeItem::p(const IDalitzEvent* evt) const{
  TLorentzVector pV;
  if(0 == evt) return pV;
  for(unsigned int i=0; i < asi().size(); i++){
    pV += evt->p(this->asi(i));
  }
  return pV;
}

double AssociatedDecayTreeItem::mReco2(const IDalitzEvent* evt) const{
  if(0 == evt) return 0;
  return evt->sij(this->asi());
}
double AssociatedDecayTreeItem::mReco(const IDalitzEvent* evt) const{
  if(0 == evt) return 0;
  double m2 = mReco2(evt);
  if(m2 >= 0.0) return sqrt(m2);
  else return -sqrt(m2);
}
double AssociatedDecayTreeItem::mRes(const IDalitzEvent* evt) const{
  // will return actual mass for the "non-resonant particles"
  if(this->isNonResonant() && 0 != evt){
    return mReco(evt);
  }else{
    return this->mass();
  }
}
//
