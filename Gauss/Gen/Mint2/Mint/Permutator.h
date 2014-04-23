#ifndef PERMUTATOR_HH
#define PERMUTATOR_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/Permutation.h"
#include "Mint/DalitzEventPattern.h"

#include <vector>
#include <iostream>

class Permutator : public std::vector<Permutation> {
  DalitzEventPattern _pat;

  int findAllPermutations();
 public:
  Permutator();
  Permutator(const DalitzEventPattern& pat);
  void setPattern(const DalitzEventPattern& pat);
  void print(std::ostream& os = std::cout) const;
};

std::ostream& operator<<(std::ostream& os, const Permutator& p);

#endif
//
