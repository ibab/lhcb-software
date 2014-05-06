// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT
#ifndef SPINFACTOR
#define SPINFACTOR

#include "Mint/IDalitzEventAccess.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/AssociatingDecayTree.h"
#include "Mint/ISpinFactor.h"
#include "Mint/counted_ptr.h"
#include <string>
#include <iostream>

class SpinFactor 
: public DalitzEventAccess
, virtual public ISpinFactor{
 protected:
  AssociatingDecayTree _associatingDecayTree;
  static void normalOrder( MINT::const_counted_ptr<AssociatedDecayTree>& a
			   , MINT::const_counted_ptr<AssociatedDecayTree>& b);
  int _nFinal;
  std::vector<MINT::const_counted_ptr<AssociatedDecayTree> > fsPS; // final state PseudoScalars.
  TLorentzVector p(int i);

 public:
  virtual double getVal()=0;
  virtual double RealVal(){return getVal();}// some dublication here...

  double mRes(const AssociatedDecayTreeItem& adt);
  double mRes(const MINT::const_counted_ptr<AssociatedDecayTree>& adt);
	      // will return PDG mass, exceept for "non-resonant particles", 
	      // where it's the reconstructed mass.

  const AssociatedDecayTree& theDecay() const{
    return _associatingDecayTree.getTree();
  }
  SpinFactor(IDalitzEventAccess* events, const DecayTree& decay, int nFinal=3) 
    : IDalitzEventAccess()
    , MINT::IReturnReal()
    , ISpinFactor()
    , DalitzEventAccess(events)
    , _associatingDecayTree(decay, events)
    , _nFinal(nFinal)
    , fsPS(nFinal, (MINT::const_counted_ptr<AssociatedDecayTree>) 0)
    {}
  
  SpinFactor(const SpinFactor& other) // just create a new one
    : MINT::IBasicEventAccess<IDalitzEvent>()
    , MINT::IEventAccess<IDalitzEvent>()
    , IDalitzEventAccess()
    , MINT::IReturnReal()
    , ISpinFactor()
    , DalitzEventAccess(other)
    , _associatingDecayTree(other._associatingDecayTree)
    , _nFinal(other._nFinal)
    , fsPS(other._nFinal, (MINT::const_counted_ptr<AssociatedDecayTree>) 0)
    {}
  SpinFactor(const SpinFactor& other, IDalitzEventAccess* newEvents)
    : MINT::IBasicEventAccess<IDalitzEvent>()
    , MINT::IEventAccess<IDalitzEvent>()
    , IDalitzEventAccess()
    , MINT::IReturnReal()
    , ISpinFactor()
    , DalitzEventAccess(newEvents)
    , _associatingDecayTree(other._associatingDecayTree, newEvents)
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
