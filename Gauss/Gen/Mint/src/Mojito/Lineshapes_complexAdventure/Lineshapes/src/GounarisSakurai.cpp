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

std::complex<double> GounarisSakurai::ReBreitWigner() {
  // usually a real number that will form the real part of
  // the "Breit Wigner".
  double m   = mumsPDGMass();
  double m2  = m*m;
  double km  = k(m2);
  double km2 = km*km;
  double km3 = km*km*km;
  
  double s   = mumsRecoMass2();
  double ks  = k(s);
  double ks2 = ks*ks;
  
  double G = mumsWidth();
  
  double rterm2 = G*(  m2/km3  )
    *(ks2 *( h(s)-h(m2) )+ km2 * hprime(m2)*(m2-s));

  return (GeV*GeV)*(m2 - s+rterm2)/den();
}

std::complex<double> GounarisSakurai::ImBreitWigner() {
  // (usually a real number, that will form the
  // imaginary part of the BW
  double m   = mumsPDGMass();
  double s   = mumsRecoMass2();
  double G = mumsWidth();


  /*
  double m2  = m*m;
  double km  = k(m2);

  double ks  = k(s);
  
  
  double sqrt_s = mumsRecoMass(); 

  double returnVal =  sqrt_s * G * pow(ks/km,3)*(m/sqrt_s)*pow(Fr(),2)/den();
  */

  /*
  cout << " Jonas GS: ImBW: m, s, G " << m << ", " << s << ", " << G << endl;
  cout << "  ... Fr() " << Fr() << endl;
  */
  std::complex<double> returnVal =  (GeV*GeV)*sqrt(s)*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s))*pow(Fr(),2)/den();

  //  cout << "Jonas GS: ImBW returning " << returnVal << endl;
  return returnVal;
} 

std::complex<double> GounarisSakurai::den() {
  double m   = mumsPDGMass();
  
  double s   = mumsRecoMass2();

  double G = mumsWidth();
  
  /*
  double ks  = k(s);
  double ks2 = ks*ks;

  double m2  = m*m;
  double km  = k(m2);
  double km2 = km*km;
  double km3 = km*km*km;

  double sqrt_s = mumsRecoMass();

  double rterm2 = G*(  m2/km3 )
    *(ks2*(h(s)-h(m2))
      +km2* hprime(m2)*(m2-s));
  
  double r = m*m - s+rterm2;
  double c = sqrt_s*G*pow(ks/km2,3)*(m/sqrt_s)*pow(Fr(),2);
  
  return r*r + c*c; 
  */
  
  double rterm2 = G*(pow(m,2)/pow(k(pow(m,2)),3))
    *(pow(k(s),2)*(h(s)-h(pow(m,2)))
      +pow(k(pow(m,2)),2)*hprime(pow(m,2))*(pow(m,2)-s));
  
  double r = m*m - s+rterm2;
  std::complex<double> c = sqrt(s)*G*pow(k(s)/k(pow(m,2)),3)*(m/sqrt(s))*pow(Fr(),2);
  
  return r*r + c*c; 
}


double GounarisSakurai::d() 
{
  double m = mumsPDGMass();
  double mpi = daughterRecoMass(0);

  Double_t mpipi = pow(m,2);
  Double_t logfactor = log((m+2*k(mpipi))/(2.0*mpi));
  Double_t term1 = (3.0/TMath::Pi())*pow(mpi/k(mpipi),2)*logfactor;
  Double_t term2 = m/(2.0*TMath::Pi()*k(mpipi));
  Double_t term3 = pow(mpi,2)*m/(TMath::Pi()*pow(k(mpipi),3));
  
  return term1+term2-term3;
  
}

double GounarisSakurai::k(double mpipi) 
{
  double mpi = daughterRecoMass(0);
  if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
  return sqrt(0.25*mpipi-pow(mpi,2));
}

double GounarisSakurai::kprime(double mpipi) 
{
  double mpi = daughterRecoMass(0);// reco=pdg for final state
  if (0.25*mpipi-pow(mpi,2)<=0.0) return 0.0;
  return 1.0/(8.0*sqrt(0.25*mpipi-pow(mpi,2)));
}

double GounarisSakurai::h(double mpipi) 
{
  double mpi = daughterRecoMass(0);
  double logterm = log((sqrt(mpipi)+2.0*k(mpipi))/(2.0*mpi));
  return (2.0*k(mpipi)*logterm)/(TMath::Pi()*sqrt(mpipi));
}
double GounarisSakurai::hprime(double mpipi) 
{
  
  return h(mpipi)*(0.125/pow(k(mpipi),2)-0.5/mpipi)+0.5/(TMath::Pi()*mpipi);
  
}
std::complex<double> GounarisSakurai::BreitWigner() {
  std::complex<double> i(0, 1);
  return std::complex<Double_t>(ReBreitWigner() + i * ImBreitWigner());
}

double GounarisSakurai::sJ(){
  double G = mumsWidth();
  double m = mumsPDGMass();
  return (1.0+d()*G/m);// the 2nd factor is calc. in SpinFactor3: *(mAC-mBC);
}

std::complex<double> GounarisSakurai::getVal(){
  bool dbThis=false;
  resetInternals();
  if(startOfDecayChain()){
    // in principle there is no need to distinguish the start
    // of the decay chain from the rest - it could just get
    // a Breit Wigner (with a constant value of the width of 
    // the D is zero, as usual). However, 
    // this is to comply with the usual convention: Only the
    // form factor, not the BW-propagator.
    return Fr();
  }



  std::complex<double> returnVal = Fr()*sJ()*BreitWigner();

  if(dbThis) cout << " GounarisSakurai for " 
		  << _theDecay.oneLiner() << endl; // dbg
  if(dbThis) cout << "\n    >  nominalMass " << mumsPDGMass()
		  << "\n    > nominalWidth " << mumsWidth()
		  << "\n    > Fr() " << Fr()
		  << "\n    > a part from sJ() " << sJ()
		  << "\n    > d()  " << d()
		  << "\n    > den() " << den()
		  << "\n    > BW   " << BreitWigner()
		  << "\n    > FR*BW = " << Fr() * BreitWigner()
		  << "\n    > recoMass " << mumsRecoMass()
		  << endl;
  
  return returnVal;
}


//
