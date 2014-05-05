// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT
#ifndef ASSOCIATING_DECAY_TREE
#define ASSOCIATING_DECAY_TREE

/*
  This is a decay tree whose elements have been associated 
  to one or more final state particle in the event record.
  This is done by storing the indices of the particles in the
  event record.

  Stable particles in the tree get associated to one stable
  particle in the event records.
  Unstable particles 'carry' the association to all their
  daughter particles.
*/

#include "Mint/DecayTree.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/DalitzEventPattern.h"

#include "Mint/AssociatedDecayTree.h"
#include "Mint/counted_ptr.h"

class AssociatingDecayTree{
 protected:
  mutable AssociatedDecayTree _theDecay; // 'mutable' probably not so nice
  mutable DalitzEventPattern _prevPattern;     // ditto
  //  mutable IDalitzEvent* _evtPtr;

  DalitzEventPattern _theTreesPattern;

  bool associate(const DalitzEventPattern& pat) const;
  bool associate(const IDalitzEvent& evt) const;

  bool associateFinalStates(const DalitzEventPattern& pat) const;
  bool associateFinalStates(const IDalitzEvent& evt) const;
  bool associateResonances() const;
  std::vector<int>  addAssociations(MINT::counted_ptr<AssociatedDecayTree> ctree) const;
  std::vector<int> addAssociations(AssociatedDecayTree* tree) const;

  bool patternHasChanged(const DalitzEventPattern& pat) const;
  bool patternHasChanged(const IDalitzEvent& evt) const;

 public:
  AssociatingDecayTree(const DecayTree& tree)
    : _theDecay(tree)
    , _prevPattern()
    , _theTreesPattern(tree)
    {
      //associate();
      //      std::cout << "associated the decay tree\n" << *this << std::endl;
    }
  
  AssociatingDecayTree(const AssociatingDecayTree& other)
    : _theDecay(other._theDecay)
    , _prevPattern(other._prevPattern)
    , _theTreesPattern(other._theTreesPattern)
    {}

  const AssociatedDecayTree* getTreePtr(const DalitzEventPattern& pat) const;
  const AssociatedDecayTree* getTreePtr(const IDalitzEvent& evt) const;

  const AssociatedDecayTree& getTree(const DalitzEventPattern& pat) const;
  const AssociatedDecayTree& getTree(const IDalitzEvent& evt) const;

  DecayTree getBareTree() const{
    return _theDecay;
  }

  const DalitzEventPattern& getTreePattern() const;

};

#endif
//
