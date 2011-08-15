// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT
#include "Mint/AllPossibleSij.h"
#include "Mint/Utils.h"
using namespace MINT; // for stuff in Utils.h

bool AllPossibleSij::nextLevel(namedVMap& thisMap){
  namedVMap nextMap;
  if(thisMap.empty()){
    for(int i=1; i<= _nd; i++){
      std::vector<int> v;
      v.push_back(i);
      thisMap[anyVectorToString(v)]=v;
    }
  }

  for(namedVMap::iterator it = thisMap.begin();
      it != thisMap.end(); it++){
    std::vector<int> v(it->second);
    int n=v.size();
    if(n >= _nd -1) return false;
    int il;
    if(v.empty()) il=0;
    else il = v[v.size()-1];
    
    v.resize(v.size()+1);
    for(int i=il+1; i<= _nd; i++){
      v[n] = i;
      nextMap[anyVectorToString(v)]=v;
    }
  }
  thisMap = nextMap;
  return true;
}

void AllPossibleSij::print(std::ostream& os) const{
  int counter=0;
  os << " printing AllPossibleSij for n = " << _nd
     << ": ";
  for(namedVMap::const_iterator it = this->begin();
      it != this->end(); it++, counter++){
    if(counter > 0) os << ", ";
    os << it->first;
  }
}


//
