// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#ifndef SPINFACTOR
#define SPINFACTOR

#include "Mint/AssociatingDecayTree.h"
#include "Mint/ISpinFactor.h"
#include "Mint/counted_ptr.h"
#include <string>
#include <iostream>

class SpinFactor 
: virtual public MINT::IReturnRealForEvent<IDalitzEvent>
, virtual public ISpinFactor{
 protected:
  AssociatingDecayTree _associatingDecayTree;
  int _nFinal;
  std::vector<MINT::const_counted_ptr<AssociatedDecayTree> > fsPS; // final state PseudoScalars.
  TLorentzVector p(int i, IDalitzEvent& evt);

 public:
  virtual double getVal(IDalitzEvent& evt)=0;
  virtual double RealVal(IDalitzEvent& evt){return getVal(evt);}// some dublication here...

  double mRes(const AssociatedDecayTreeItem& adt, IDalitzEvent& evt);
  double mRes(const MINT::const_counted_ptr<AssociatedDecayTree>& adt
	      , IDalitzEvent& evt);
	      // will return PDG mass, exceept for "non-resonant particles", 
	      // where it's the reconstructed mass.

  const AssociatedDecayTree& theDecay(const DalitzEventPattern& pat) const{
    return _associatingDecayTree.getTree(pat);
  }
  const AssociatedDecayTree& theDecay(IDalitzEvent& evt) const{
    return theDecay(evt.eventPattern()); // for backw. compatibility
  }
  DecayTree theBareDecay() const{
    return _associatingDecayTree.getBareTree();
  }

  SpinFactor(const DecayTree& decay, int nFinal=3) 
    : MINT::IReturnRealForEvent<IDalitzEvent>()
    , ISpinFactor()
    , _associatingDecayTree(decay)
    , _nFinal(nFinal)
    , fsPS(nFinal, (MINT::const_counted_ptr<AssociatedDecayTree>) 0)
    {}
  
  SpinFactor(const SpinFactor& other) // just create a new one
    : MINT::IReturnRealForEvent<IDalitzEvent>()
    , ISpinFactor()
    , _associatingDecayTree(other._associatingDecayTree)
    , _nFinal(other._nFinal)
    , fsPS(other._nFinal, (MINT::const_counted_ptr<AssociatedDecayTree>) 0)
    {}
  virtual void printYourself(std::ostream& os=std::cout) const=0;
  virtual void printParsing(std::ostream& os=std::cout) const;
  virtual ~SpinFactor(){}
  virtual const DecayTree& exampleDecay()=0;
  virtual std::string name() const=0;
};

#endif
//
