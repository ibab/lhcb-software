#ifndef ASSOCIATED_DECAY_TREEITEM_HH
#define ASSOCIATED_DECAY_TREEITEM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:02 GMT

#include "Mint/DecayTreeItem.h"
#include "TLorentzVector.h"

class AssociatingDecayTree;
class IDalitzEvent;

class AssociatedDecayTreeItem : public DecayTreeItem {
 protected:
  std::vector<int> _asi;
 public:
  AssociatedDecayTreeItem(int i=0);
  AssociatedDecayTreeItem(const AssociatedDecayTreeItem& other);
  AssociatedDecayTreeItem(const DecayTreeItem& other);

  const std::vector<int>& asi() const;
  int asi(int i) const;

  virtual void print(std::ostream& out = std::cout) const;

  // main point of these routines is to handle non-resonant particles
  // they simply add up the relevant properties of the daughter
  // particles where appropriate
  TLorentzVector p(const IDalitzEvent* evt) const;
  double mReco2(const IDalitzEvent* evt) const;
  double mReco(const IDalitzEvent* evt) const;
  double mRes(const IDalitzEvent* evt=0) const;
  // mRes will return mReco for the "non-resonant particles"
  // for normal particle: pdg mass

  friend class AssociatingDecayTree;
};

#endif
//
