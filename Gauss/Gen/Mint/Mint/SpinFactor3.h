#ifndef SPINFACTOR_THREE_BODY_HH
#define SPINFACTOR_THREE_BODY_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:13 GMT

#include "Mint/SpinFactor.h"
#include "Mint/counted_ptr.h"
#include <iostream>

class SpinFactor3 : public SpinFactor{
 protected:
  static DecayTree* _exampleDecay;
  MINT::const_counted_ptr<AssociatedDecayTree> R; // resonance.
  int _spin;
  bool _nonResonant;
  bool parseTree();

  double nonResVal() const{return 1;}
  double spinZeroVal() const{ return 1;}
  double spinOneVal();
  double spinTwoVal();

  double spinOneFromMasses(); //db
  double spinTwoFromMasses(); //db

  double spinOneFromZemach(); //experimental

  bool setSpin();

 public:
  SpinFactor3(IDalitzEventAccess* events
	      , const DecayTree& theDecay)
    : SpinFactor(events, theDecay, 3)
    , R(0)
    , _spin(-1)
    , _nonResonant(false)
    {
      if( ! parseTree()){
	throw "error in parseTree in constructor of SpinFactor3";
      }
    }

  virtual double getVal();
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor3(" + theDecay().oneLiner() + ")";
  }
  virtual void printYourself(std::ostream& os = std::cout)const;
  virtual ~SpinFactor3(){}
};

#endif
//
