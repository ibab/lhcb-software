#ifndef GOUNARIS_SAKURAI_LINESHAPE_HH
#define GOUNARIS_SAKURAI_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "ILineshape.h"
#include "BW_BW.h"
#include "AssociatedDecayTree.h"
#include "IDalitzEventAccess.h"
#include "CLHEPSystemOfUnits.h"

// copied from Jim's DcyGSAmplitude.
// See also: Gounaris and Sakurai, Rev. Lett. 21, 244
//           http://link.aps.org/doi/10.1103/PhysRevLett.21.244
//
class GounarisSakurai : public BW_BW, virtual public ILineshape{

 protected:
  double d() ;
  double k(double  mpipi) ;
  double kprime(double mpipi) ;
  double h(double mpipi) ;
  double hprime(double mpipi) ;
  std::complex<double> den();
  double sJ();//should be taken to SpinFactor3 one day.

  //  double mumsWidth()const{ return 0.1464*GeV;}// (fixes it to the value in Jim's code)

  virtual std::complex<double> ReBreitWigner();// actually, this is the GS shape
  virtual std::complex<double> ImBreitWigner();// and not Breit Wigner - will
  virtual std::complex<double> BreitWigner(); // rename one day.
  
 public:
  
  GounarisSakurai( const AssociatedDecayTree& decay
		   , IDalitzEventAccess* events)
    : BW_BW(decay, events){}

  virtual std::complex<double> getVal();

  virtual std::string name() const{
    return "GounarisSakurai("+_theDecay.oneLiner() +")";
  }

  virtual ~GounarisSakurai(){}
};

#endif
//
