
#include "FragmentedCircle.h"

std::ostream & RichMarkov::FragmentedCircle::ArcPart::printMeTo(std::ostream & os) const {
  if (_whole) {
    os << "WholeCircleArcPart";
  } else {
    os << "ArcPart[start="<<_thetaStart<<", deltaPhi="<<_phi<<"]";
  };
  return os;
};

inline std::ostream & RichMarkov::FragmentedCircle::printMeTo(std::ostream & os) const {
  os << "FragmentedCircle[centre="<<_centre<<", radius="<<_radius<<", ArcPartList=";
  // now output arcpartlist.
  for (ArcPartList::const_iterator it=_arcPartList.begin();
       it!=_arcPartList.end();
       ++it) {
    os << "{";
    it->printMeTo(os);
    os <<"}";
  };
  return os <<"]";
};
