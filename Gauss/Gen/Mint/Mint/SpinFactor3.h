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
  bool parseTree(const DalitzEventPattern& pat);

  double nonResVal() const{return 1;}
  double spinZeroVal() const{ return 1;}
  // double GSSpinFactor(IDalitzEvent& evt);
  double spinOneVal(IDalitzEvent& evt);
  double spinTwoVal(IDalitzEvent& evt);

  double spinOneFromMasses(IDalitzEvent& evt); //db
  double spinTwoFromMasses(IDalitzEvent& evt); //db

  double spinOneFromZemach(IDalitzEvent& evt); //experimental

  bool setSpin();

 public:
  SpinFactor3(const DecayTree& theDecay)
    : SpinFactor(theDecay, 3)
    , R(0)
    , _spin(-1)
    , _nonResonant(false)
    {
      // we'll do the parsing now when we see the 1st event.
      //if( ! parseTree()){
      //throw "error in parseTree in constructor of SpinFactor3";
      //}
    }

  virtual double getVal(IDalitzEvent& evt);
  static const DecayTree& getExampleDecay();
  virtual const DecayTree& exampleDecay();
  virtual std::string name() const{
    return "SpinFactor3(" + theBareDecay().oneLiner() + ")";
  }
  virtual void printYourself(std::ostream& os = std::cout)const;
  virtual ~SpinFactor3(){}
};

#endif
//
