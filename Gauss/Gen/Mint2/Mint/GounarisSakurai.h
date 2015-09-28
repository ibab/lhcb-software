#ifndef GOUNARIS_SAKURAI_LINESHAPE_HH
#define GOUNARIS_SAKURAI_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/CLHEPSystemOfUnits.h"

// copied from Jim's DcyGSAmplitude.
// See also: Gounaris and Sakurai, Rev. Lett. 21, 244
//           http://link.aps.org/doi/10.1103/PhysRevLett.21.244
//
class GounarisSakurai : public BW_BW, virtual public ILineshape{

 protected:
  double d() ;
  double k(const double&  mpipi) const ;
  //double kprime(double mpipi) ;
  double h(const double& mpipi) const ;
  double hprime(const double& mpipi) const ;
  //double den();
  double sJ();//should be taken to SpinFactor3 one day.

  //  double mumsWidth()const{ return 0.1464*GeV;}// (fixes it to the value in Jim's code)

  //virtual double ReBreitWigner();// actually, this is the GS shape
  //virtual double ImBreitWigner();// and not Breit Wigner - will
  //virtual std::complex<double> BreitWigner(); // rename one day.
  
  double ReGSDen() const;
  double ImGSDen() const; 
  std::complex<double> InvGSDen() const;
    
 public:
  
  GounarisSakurai( const AssociatedDecayTree& decay)
    : BW_BW(decay){}

  virtual std::complex<double> getVal(IDalitzEvent& evt);

  virtual std::string name() const{
    return "GounarisSakurai("+_theDecay.oneLiner() +")";
  }

  virtual ~GounarisSakurai(){}
};

#endif
//
