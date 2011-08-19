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
#include "Mint/IDalitzEventAccess.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/IDalitzEventList.h"

#include "Mint/DalitzEventPattern.h"

#include "Mint/AssociatedDecayTree.h"
#include "Mint/counted_ptr.h"

class AssociatingDecayTree : public DalitzEventAccess{
 protected:
  mutable AssociatedDecayTree _theDecay; // 'mutable' probably not so nice
  mutable DalitzEventPattern _prevPattern;     // ditto
  
  DalitzEventPattern _theTreesPattern;

  bool associate() const;

  bool associateFinalStates() const;
  bool associateResonances() const;
  std::vector<int>  addAssociations(MINT::counted_ptr<AssociatedDecayTree> ctree) const;
  std::vector<int> addAssociations(AssociatedDecayTree* tree) const;

  const DalitzEventPattern& getPattern() const;

  bool patternHasChanged() const;
 public:
  AssociatingDecayTree(const DecayTree& tree, IDalitzEventAccess* dad)
    : IDalitzEventAccess()
    , DalitzEventAccess(dad)
    , _theDecay(tree)
    , _prevPattern()
    , _theTreesPattern(tree)
    {
      associate();
      //      std::cout << "associated the decay tree\n" << *this << std::endl;
    }
  
  AssociatingDecayTree(const DecayTree& tree, IDalitzEventList* dad)
    : IDalitzEventAccess()
    , DalitzEventAccess(dad)
    , _theDecay(tree)
    , _prevPattern()
    , _theTreesPattern(tree)
    {
      associate();
      //      std::cout << "associated the decay tree\n" << *this << std::endl;
    }
  
  AssociatingDecayTree(const AssociatingDecayTree& other)
    : MINT::IBasicEventAccess<IDalitzEvent>()
    , MINT::IEventAccess<IDalitzEvent>()
    , IDalitzEventAccess()
    , DalitzEventAccess(other)
    , _theDecay(other._theDecay)
    , _prevPattern(other._prevPattern)
    , _theTreesPattern(other._theTreesPattern)
    {}
  AssociatingDecayTree(const AssociatingDecayTree& other
		       , IDalitzEventAccess* newEvents)
    : IDalitzEventAccess()
    , DalitzEventAccess(newEvents)
    , _theDecay(other._theDecay)
    , _prevPattern(other._prevPattern)
    , _theTreesPattern(other._theTreesPattern)
    {
      associate();// not really needed, but safer.
    }


  const AssociatedDecayTree* getTreePtr() const;

  const AssociatedDecayTree& getTree() const;

  const DalitzEventPattern& getTreePattern() const;

};

#endif
//
