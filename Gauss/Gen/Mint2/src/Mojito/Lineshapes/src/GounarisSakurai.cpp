// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

// copied from Jim's DcyGSAmplitude.
// See also: Gounaris and Sakurai, Rev. Lett. 21, 244
//           http://link.aps.org/doi/10.1103/PhysRevLett.21.244
//

#include "Mint/GounarisSakurai.h"
//#include "Mint/CLHEPPhysicalConstants.h"
#include "TMath.h"

#include <cmath>
#include <complex>

using namespace std;

Double_t GounarisSakurai::d() 
{
  double m = mumsMass();
  double mpi = daughterRecoMass(0);

  Double_t mpipi = pow(m,2);
  Double_t logfactor = log((m+2*k(mpipi))/(2.0*mpi));
  Double_t term1 = (3.0/TMath::Pi())*pow(mpi/k(mpipi),2)*logfactor;
  Double_t term2 = m/(2.0*TMath::Pi()*k(mpipi));
  Double_t term3 = pow(mpi,2)*m/(TMath::Pi()*pow(k(mpipi),3));
  
  return term1+term2-term3;
  
}

double GounarisSakurai::k(const double& mpipi) const 
{
  const double mpi = daughterRecoMass(0);
  if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
  return sqrt(0.25*mpipi-pow(mpi,2));
}


double GounarisSakurai::h(const double& mpipi) const
{
  const double mpi = daughterRecoMass(0);
  const double logterm = log((sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
  return (2.0*k(mpipi)*logterm)/(TMath::Pi()*sqrt(mpipi));
}
double GounarisSakurai::hprime(const double& mpipi) const 
{
  return h(mpipi)*(0.125/pow(k(mpipi),2)-0.5/mpipi)+0.5/(TMath::Pi()*mpipi);
}


double GounarisSakurai::sJ(){
  double G = mumsWidth();
  double m = mumsMass();
  return m*m*(1.0+d()*G/m);
}

double GounarisSakurai::ReGSDen() const {
    const double m   = mumsMass();
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
    const double m = mumsMass();
    const double s = mumsRecoMass2();
    const double G = mumsWidth();
    
    //also do we really need a barrier factor in here? None in paper
    const double returnVal =  -m*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s)); //*Fr()*Fr(); ?
    
    return returnVal;
}

std::complex<double> GounarisSakurai::InvGSDen() const {
    return 1.0/std::complex<double>(ReGSDen(), ImGSDen());
}

std::complex<double> GounarisSakurai::getVal(IDalitzEvent& evt){
  bool dbThis=false;
  setEventPtr(evt);
  resetInternals();
  
  double formFactor= 1.;
  if( _normBF == 1 ) formFactor = Fr();
  else if( _normBF == 0 ) formFactor = Fr_PDG_BL();  
  else if(_normBF == 2 ) formFactor = Fr_BELLE(0.);    
    
  if(startOfDecayChain()){
    return formFactor;
  }

  std::complex<double> returnVal = formFactor*sJ()*InvGSDen();

  if(dbThis) cout << " GounarisSakurai for " 
		  << _theDecay.oneLiner() << endl; // dbg
  if(dbThis) cout << "\n    >  nominalMass " << mumsMass()
		  << "\n    > nominalWidth " << mumsWidth()
		  << "\n    > Fr() " << formFactor
		  << "\n    > a part from sJ() " << sJ()
		  << "\n    > d()  " << d()
		  << "\n    > BW   " << InvGSDen()
		  << "\n    > recoMass " << mumsRecoMass()
		  << endl;
  
  return returnVal;
}



//
