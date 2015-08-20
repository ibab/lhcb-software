#ifndef MINTLINESHAPES_SINGLE_HAT_SHAPE_HH
#define MINTLINESHAPES_SINGLE_HAT_SHAPE_HH

#include <complex>
#include <string>
//
#include <complex>
#include <string>
#include <iostream>

#include "Mint/FitParDependent.h"

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEvent.h"

#include "Mint/DalitzCoordinate.h"
#include "Mint/IGenFct.h"

class singleTopHatShape : virtual public ILineshape, public MINT::FitParDependent{
 private:
  mutable MINT::counted_ptr<IGenFct> _genFct;
  void makeGeneratingFunction() const;
  
 protected:

  const AssociatedDecayTree& _theDecay; 
  double _min_sij, _max_sij;

  double min()const{return _min_sij;}
  double max()const{return _max_sij;}

  bool startOfDecayChain() const{return !(_theDecay.hasParent());}
  double mumsRecoMass2(IDalitzEvent& evt) const;
  
 public:
  singleTopHatShape( const AssociatedDecayTree& decay
		     , double mini, double maxi);
  singleTopHatShape(const singleTopHatShape& other);

  virtual std::complex<double> getVal(IDalitzEvent& evt);
  virtual std::complex<double> getValue(IDalitzEvent& evt) const; // not required, but useful

  virtual DalitzCoordinate getDalitzCoordinate(double nSigma=3) const;
  virtual void print(IDalitzEvent& evt, std::ostream& out = std::cout) const;
  virtual void print(std::ostream& out = std::cout) const;
  virtual std::string name() const;
  virtual MINT::counted_ptr<IGenFct> generatingFunction() const;


  virtual ~singleTopHatShape(){};
};
std::ostream& operator<<(std::ostream& out, const singleTopHatShape& amp);

#endif
//
