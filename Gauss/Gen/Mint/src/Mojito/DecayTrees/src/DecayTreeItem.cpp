// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/DecayTreeItem.h"
#include "Mint/ParticlePropertiesList.h"
#include <cmath>

DecayTreeItem::DecayTreeItem(int i)
  : _pdg_id(i)
  , _L_angMom(-9999)
{
}
DecayTreeItem::DecayTreeItem(const DecayTreeItem& other)
  : _pdg_id(other._pdg_id)
  , _L_angMom(other._L_angMom)
{}
DecayTreeItem& DecayTreeItem::operator=(const DecayTreeItem& other){
  _pdg_id    = other._pdg_id;
  _L_angMom  = other._L_angMom;
  return *this;
}

const ParticleProperties* DecayTreeItem::props() const{
  const ParticlePropertiesList* ppl = ParticlePropertiesList::getMe();
  if(0 == ppl){
    std::cout << "Error in DecayTreeItem::props(): "
	      << "can't find ParticlePropertiesList."
	      << std::endl;
    return 0;
  }
  return ppl->get(_pdg_id);
}
std::string DecayTreeItem::name() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return "";
  }
  return pp->name();
}
std::string DecayTreeItem::SVPAT() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return "?";
  }
  return pp->SVPAT();
}
double DecayTreeItem::mass() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return -1;
  }
  return pp->mass();
}
bool DecayTreeItem::isNonResonant() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return false;
  }
  return pp->isNonResonant();
}
double DecayTreeItem::width() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return -1;
  }
  return pp->width();
}
std::string DecayTreeItem::charge() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return "-9999";
  }
  return pp->charge();
}
bool DecayTreeItem::hasDistinctAnti() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return -1;
  }
  return pp->hasDistinctAnti();
}
int DecayTreeItem::pdg() const{
  return _pdg_id;
}

std::string DecayTreeItem::J() const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    return "";
  }
  return pp->J();
}


bool DecayTreeItem::ChargeConThis(){
  if(! hasDistinctAnti()) return false;
  _pdg_id *= -1;
  return true;
}

void DecayTreeItem::setPDG(int id){
  _pdg_id = id;
}
char DecayTreeItem::L_as_SPD()const{
  if(L() < 0) return ' ';

  if(L()==0)      return 'S';
  else if(L()==1) return 'P';
  else if(L()==2) return 'D';
  else if(L()==3) return 'F';
  else if(L()==4) return 'G';
  else if(L()==5) return 'H';
  else if(L()==6) return 'I';
  else{
    // J's missing, next (L=7) is K, then alphabetical
    char Kplus='K';
    int advanceK = L()-7;
    for(int i=0; i < advanceK; i++) Kplus++;
    return Kplus;
  }
}

void DecayTreeItem::print(std::ostream& out) const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    out << _pdg_id;
  }else{
    out << pp->name() 
	<< "(" << pp->SVPAT() <<")";
  }
  if(L() > -9998){
    out << L_as_SPD() << "-wave";
  }
}
void DecayTreeItem::printShort(std::ostream& out) const{
  const ParticleProperties* pp = props();
  if(0 == pp){
    out << _pdg_id;
  }else{
    out << pp->name();
  }
  if(L() > -9998){
    out << "[" << L_as_SPD() << "]";
  }
}
DecayTreeItem DecayTreeItem::operator-() const{
  DecayTreeItem dt(*this);
  dt.antiThis();
  return dt;
}
DecayTreeItem DecayTreeItem::operator+() const{
  return *this;
}

std::ostream& operator<<(std::ostream& os
			 , const DecayTreeItem& dti){
  dti.print(os);
  return os;
}

std::stringstream& operator<<(std::stringstream& seam
			      , const DecayTreeItem& dti){
  dti.printShort(seam);
  return seam;
}

std::stringstream& operator<<(std::stringstream& seam
			      , int i){
  ((std::ostream&) seam) << i;
  return seam;
}


//
