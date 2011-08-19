#ifndef AMPLITUDE_HH
#define AMPLITUDE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT

#include <complex>
#include <vector>
#include <iostream>

#include "TRandom.h"

#include "Mint/AmpInitialiser.h"
#include "Mint/AssociatingDecayTree.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/ISpinFactor.h"
#include "Mint/LineshapeMaker.h"

#include "Mint/ILineshape.h"

#include "Mint/IReturnReal.h"
#include "Mint/IReturnComplex.h"

#include "Mint/DalitzBox.h"
#include "Mint/DalitzBWBox.h"
#include "Mint/DalitzBoxSet.h"
#include "Mint/DalitzBWBoxSet.h"
#include "Mint/DalitzEventPattern.h"

#include "Mint/Permutation.h"

#include "Mint/DalitzEvent.h"

#include "Mint/counted_ptr.h"

class Amplitude 
: public DalitzEventAccess
  , virtual public MINT::IGetRealEvent<IDalitzEvent>
  , virtual public MINT::IReturnComplex
{
 protected:
  AssociatingDecayTree _associatingDecayTree;
  ISpinFactor* _spinFactor;
  char _spd;
  std::string _lopt;

  DalitzEventPattern _pat; // extracted from tree - stored for speed.

  std::vector<ILineshape*> LineshapeList;

  virtual double boxFactor(){ return 1;}

  bool createLineshapes(const MINT::const_counted_ptr< AssociatedDecayTree >& 
			counted_tree_ptr);
  bool createLineshapes(const AssociatedDecayTree* treePtr=0);
  bool deleteLineshapes();

  bool deleteDependants();
  bool createDependants();

  bool renew();

  std::complex<double> LineshapeProduct();
  std::complex<double> LineshapeProductAtResonance();
  std::complex<double> LineshapeProductSmootherLarger();
  //  double LineshapeGaussProduct();
  double SpinFactorValue();
  std::complex<double> getOnePermutationsVal();
  std::complex<double> getOnePermutationsSmootherLargerVal();
 public:
  Amplitude( const DecayTree& decay
	     , IDalitzEventAccess* events
	     , char SPD_Wave='?'
	     , const std::string& opt=""
	     );
  Amplitude( const DecayTree& decay
	     , IDalitzEventList* events
	     , char SPD_Wave='?'
	     , const std::string& opt=""
	     );

  Amplitude( const AmpInitialiser& ampInit
	     , IDalitzEventAccess* events
	     );

  Amplitude( const AmpInitialiser& ampInit
	     , IDalitzEventList* events
	     );

  Amplitude( const Amplitude& other);
  Amplitude( const Amplitude& other, IDalitzEventAccess* newEvents);
  Amplitude( const Amplitude& other, IDalitzEventList* newEvents);

  bool resetTree(const DecayTree& dt);
  bool CPConjugate();
 
  virtual std::complex<double> getVal();
  virtual std::complex<double> getVal(IDalitzEvent* evt);

  virtual std::complex<double> getSmootherLargerVal(); // for expert-use...
  virtual std::complex<double> getSmootherLargerVal(IDalitzEvent* evt);//..only!


  virtual std::complex<double> getValAtResonance();
  double Prob(){
    std::complex<double> res = getVal();
    return res.real()*res.real() + res.imag()*res.imag();
  }

 //  virtual double gaussProb();

  virtual double RealVal(){return Prob();}
  virtual std::complex<double> ComplexVal(){return getVal();}

  const AssociatedDecayTree& theDecay(){
    return _associatingDecayTree.getTree();
  }
  const AssociatedDecayTree& theDecay() const{
    return _associatingDecayTree.getTree();
  }
  DalitzEventPattern getTreePattern() const{
    return _associatingDecayTree.getTreePattern();
  }
  

  std::string name() const;
  void print(std::ostream& out = std::cout) const;

  ISpinFactor* spinFactor(){
    return _spinFactor;
  }

  
  virtual DalitzBoxSet MakeBox(const DalitzEventPattern &pat
			       , const Permutation& perm
			       , double nSigma = 3
			       );

  virtual DalitzBoxSet MakeBoxes(const DalitzEventPattern& pat
				 , double nSigma=3);

  
  virtual DalitzBWBox MakeBWBox(const DalitzEventPattern &pat
				, const Permutation& perm
				, TRandom* rnd=gRandom
				);
  virtual DalitzBWBoxSet MakeBWBoxes(const DalitzEventPattern &pat
				     , TRandom* rnd=gRandom
				     );


  virtual ~Amplitude();

};

std::ostream& operator<<(std::ostream& out, const Amplitude& amp);

#endif
//
