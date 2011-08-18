#ifndef MINTLINESHAPES_SINGLE_HAT_SHAPE_HH
#define MINTLINESHAPES_SINGLE_HAT_SHAPE_HH

#include <complex>
#include <string>
//
#include <complex>
#include <string>
#include <iostream>

#include "Mint/Mojito/Lineshapes/ILineshape.h"
#include "Mint/Mojito/DecayTrees/AssociatedDecayTree.h"
#include "Mint/Mojito/DalitzEvents/DalitzEventAccess.h"
#include "Mint/Mojito/DalitzEvents/IDalitzEventAccess.h"

#include "Mint/Mojito/BreitWignerMC/DalitzCoordinate.h"
#include "Mint/Mojito/BreitWignerMC/IGenFct.h"

using namespace std;
using namespace MINT;

class singleTopHatShape : public DalitzEventAccess, virtual public ILineshape{
 private:
  mutable MINT::counted_ptr<IGenFct> _genFct;
  void makeGeneratingFunction() const;
  
 protected:

  const AssociatedDecayTree& _theDecay; 
  double _min_sij, _max_sij;

  double min()const{return _min_sij;}
  double max()const{return _max_sij;}

  bool startOfDecayChain() const{return !(_theDecay.hasParent());}
  double mumsRecoMass2() const;
  
 public:
  singleTopHatShape( const AssociatedDecayTree& decay
		     , IDalitzEventAccess* events
		     , double mini, double maxi);
  singleTopHatShape(const singleTopHatShape& other);

  virtual std::complex<double> getVal();
  virtual std::complex<double> getValue() const; // not required, but useful
  virtual std::complex<double> getValAtResonance(){return 1;}
  virtual std::complex<double> getSmootherLargerVal(){return getVal();}

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3) const;
  virtual void print(std::ostream& out = std::cout) const;
  virtual std::string name() const;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const=0;


  virtual ~singleTopHatShape(){};
};
std::ostream& operator<<(std::ostream& out, const singleTopHatShape& amp);

#endif
//
