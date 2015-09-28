#ifndef FLATTE_LINESHAPE_HH
#define FLATTE_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/NamedParameter.h"

#include <complex>
// Flatte parameterised a la BaBar, Phys.Rev.D78:012004,2008
// also available as:
// arXiv:0803.4451[hep-ex], SLAC-PUB-13189, BABAR-PUB-08/005 
// (and of course originally developed by Flatte,
// S. M. Flatt´e, Phys. Lett. B63, 224 (1976))

class Flatte : public BW_BW, virtual public ILineshape{
 protected:
  double _pi0Mass;
  double _piPlusMass;
  double _K0Mass;
  double _KPlusMass;

  double gPi()const;
  double gK() const;

  virtual double GofM(); // <--the trick is all here
  std::complex<double> complexGofM();
  virtual std::complex<double> BreitWigner();
  
  double mPi0();
  double mPiPlus();
  double mK0();
  double mKPlus();
  static std::complex<double> aSqrtTerm( const double& a, const double& b);  
  static double pdgMass(int PDG_ID);
 
 public:
  
  Flatte( const AssociatedDecayTree& decay)
    : BW_BW(decay)
    , _pi0Mass(-9999.0)
    , _piPlusMass(-9999.0)
    , _K0Mass(-9999.0)
    , _KPlusMass(-9999.0)
    {
    }

  //  virtual std::complex<double> getVal(){
  // std::cout << "Flatte!!!" << std::endl; return 1;}
  // this uses BW_BW's getVal, but with a different GofM()
  // (magic of 'virtual' functions)

  virtual std::string name() const{
    return "FLATTE_alaBaBar_pipi("+_theDecay.oneLiner() +")";
  }

  virtual ~Flatte(){}
};

#endif
//
