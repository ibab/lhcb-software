#ifndef CLEO2012_GOUNARIS_SAKURAI_LINESHAPE_HH
#define CLEO2012_GOUNARIS_SAKURAI_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/CLEO2012_BW_BW.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/CLHEPSystemOfUnits.h"

// copied from Jim's DcyGSAmplitude.
// See also: CLEO2012_Gounaris and Sakurai, Rev. Lett. 21, 244
//           http://link.aps.org/doi/10.1103/PhysRevLett.21.244
//
class CLEO2012_GounarisSakurai : public CLEO2012_BW_BW, virtual public ILineshape{

 protected:
  double d() ;
  double k(double  mpipi) ;
  double kprime(double mpipi) ;
  double h(double mpipi) ;
  double hprime(double mpipi) ;
  double den();
  double sJ();//should be taken to SpinFactor3 one day.

  //  double mumsWidth()const{ return 0.1464*GeV;}// (fixes it to the value in Jim's code)

  virtual double ReBreitWigner();// actually, this is the GS shape
  virtual double ImBreitWigner();// and not Breit Wigner - will
  virtual std::complex<double> BreitWigner(); // rename one day.
  
 public:
  
  CLEO2012_GounarisSakurai( const AssociatedDecayTree& decay
		   , IDalitzEventAccess* events)
    : CLEO2012_BW_BW(decay, events){}

  virtual std::complex<double> getVal();

  virtual std::string name() const{
    return "CLEO2012_GounarisSakurai("+_theDecay.oneLiner() +")";
  }

  virtual ~CLEO2012_GounarisSakurai(){}
};

#endif
//
