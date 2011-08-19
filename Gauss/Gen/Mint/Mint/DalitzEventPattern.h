#ifndef DALITZEVENTPATTERN_HH
#define DALITZEVENTPATTERN_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <iostream>
#include <vector>

#include "Mint/DecayTreeItem.h"

#include "Mint/DecayTree.h"
//#include "Mint/AmpInitialiser.h"

class AmpInitialiser;

class DalitzEventPattern : public std::vector<DecayTreeItem>{
 public:
  static const DalitzEventPattern NoPattern;
  DalitzEventPattern(){
    this->clear();
  }
  DalitzEventPattern(int pdg_ids[], int arrayDimension );
  // note: arrayDimension = num mums (=1) + num daughters.
  DalitzEventPattern(std::vector<int> pdg_ids);
  DalitzEventPattern(const std::vector<DecayTreeItem>& yetAnother);
  DalitzEventPattern(const DalitzEventPattern& other);

  DalitzEventPattern(int mum, int d1, int d2);
  DalitzEventPattern(int mum, int d1, int d2, int d3);
  DalitzEventPattern(int mum, int d1, int d2, int d3, int d4);
  DalitzEventPattern(const DecayTree& dt_in);

  DalitzEventPattern makeCPConjugate() const;

  bool equal(const DalitzEventPattern& other) const;
  bool lt(const DalitzEventPattern& other) const;
  bool operator==(const DalitzEventPattern& rhs) const;
  bool operator!=(const DalitzEventPattern& rhs) const;
  bool operator<(const DalitzEventPattern& rhs) const;
  bool operator<=(const DalitzEventPattern& rhs) const;
  bool operator>=(const DalitzEventPattern& rhs) const;
  bool operator>(const DalitzEventPattern& rhs) const;

  bool compatibleWithFinalState(const AmpInitialiser& ampInitContainsTree)const;
  bool compatibleWithFinalState(const DecayTree& tree)const;
  bool compatibleWithFinalState(const DalitzEventPattern& pat)const;
  bool compatibleWithFinalState(const std::vector<int>& otherFs)const;

  bool compatibleWith(const DecayTree& tree)const; // mother & final state

  bool selfConjugateFS() const;

  void print(std::ostream& os = std::cout)const;
  std::vector<int> finalStates() const;

  double sijMin(const std::vector<int>& indices) const;
  double sijMax(const std::vector<int>& indices) const;

  double sijMin(int i, int j) const;
  double sijMax(int i, int j) const;

  double sijMin(int i, int j, int k) const;
  double sijMax(int i, int j, int k) const;

  int numDaughters() const{
    return this->size() -1 ;
  }

  std::vector<int> getVectorOfInts() const;
  std::string name() const;
};

std::ostream& operator<<(std::ostream& os, const DalitzEventPattern& ep); 

// DalitzEventPattern contains a list of the mother
// and of all final state
// particles in the order in which they appear in 
// the event record, identified by pdg-id.
// 
// So it defines what 1 and 2 is in s_{12} etc
//
#endif
//
