#ifndef ALLPOSSIBLESIJ_HH
#define ALLPOSSIBLESIJ_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:59 GMT

#include <vector>
#include <map>
#include <string>
#include <iostream>

//#include "Mint/Utils.h"


typedef  std::map<std::string, std::vector<int> > namedVMap;

class AllPossibleSij : public namedVMap{

  bool nextLevel(namedVMap& nextMap);
  int _nd;
 public:
  AllPossibleSij(int nDaughters)
    : _nd(nDaughters)
    {
      std::vector<int> dummy;
      namedVMap mapA;
      while(nextLevel(mapA)){
	this->insert(mapA.begin(), mapA.end());
      }
    }
  void print(std::ostream& os = std::cout) const;
};

#endif
//
