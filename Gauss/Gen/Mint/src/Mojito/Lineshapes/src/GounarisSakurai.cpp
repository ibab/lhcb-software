// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/GounarisSakurai.h"
#include "Mint/CLHEPPhysicalConstants.h"

using namespace std;

std::complex<double> GounarisSakurai::getVal() {
  resetInternals();

  const std::complex<double> returnVal = Fr_PDG_BL()*sJ()*InvGSDen();

  return returnVal;
}

double GounarisSakurai::k( const double& mpipi ) const {
  const double mpi = daughterRecoMass(0);
  if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
  return sqrt(0.25*mpipi-pow(mpi,2));
}

double GounarisSakurai::h( const double& mpipi ) const {
  const double mpi = daughterRecoMass(0);
  const double logterm = log((sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
  return (2.0*k(mpipi)*logterm)/(pi*sqrt(mpipi));
}

double GounarisSakurai::hprime( const double& mpipi ) const {
  return h(mpipi)*(0.125/pow(k(mpipi),2)-0.5/mpipi)+0.5/(pi*mpipi);
}

double GounarisSakurai::d() const {
  const double m = mumsPDGMass();
  const double mpi = daughterRecoMass(0);

  const double mpipi = pow(m,2);
  const double logfactor = log((m+2*k(mpipi))/(2.0*mpi));
  const double term1 = (3.0/pi)*pow(mpi/k(mpipi),2)*logfactor;
  const double term2 = m/(2.0*pi*k(mpipi));
  const double term3 = pow(mpi,2)*m/(pi*pow(k(mpipi),3));

  return term1+term2-term3;
}

double GounarisSakurai::sJ() const {
  const double G = mumsWidth();
  const double m = mumsPDGMass();

  return m*m*(1.0+d()*G/m);
}

double GounarisSakurai::ReGSDen() const {
  const double m   = mumsPDGMass();
  const double m2  = m*m;
  const double km  = k(m2);
  const double km2 = km*km;
  const double km3 = km*km*km;

  const double s   = mumsRecoMass2();
  const double ks  = k(s);
  const double ks2 = ks*ks;

  const double G = mumsWidth();

  const double rterm2 =
    G*(  m2/km3  ) *
    (ks2 *( h(s)-h(m2) )+ km2 * hprime(m2)*(m2-s));

  return m2 - s+rterm2;
}

double GounarisSakurai::ImGSDen() const {
  const double m = mumsPDGMass();
  const double s = mumsRecoMass2();
  const double G = mumsWidth();

  //also do we really need a barrier factor in here? None in paper
  const double returnVal =  -m*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s));

  return returnVal;
} 

std::complex<double> GounarisSakurai::InvGSDen() const {
  return 1.0/std::complex<double>(ReGSDen(), ImGSDen());
}
