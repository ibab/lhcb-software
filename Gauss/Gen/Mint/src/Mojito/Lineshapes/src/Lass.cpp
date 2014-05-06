// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/Lass.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace MINT;

// Lass parameterised a la BaBar, Phys.Rev.D78:012004,2008
// (and of course originally developed by Lass,
// D. Aston et al. (LASS), Nucl. Phys. B296, 493 (1988).

Lass::Lass( const AssociatedDecayTree& decay
	    , IDalitzEventAccess* events)
  : BW_BW(decay, events)
  , _a("Lass::a", 2.07/GeV)
  , _r("Lass::r", 3.32/GeV) // same as b in Lass paper
  , _phi("Lass::phi", 0*pi/180.0)
  , _bgCutOff("Lass::bgCutOff", 1.8*GeV)
{}

Lass::Lass(const Lass& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , ILineshape()
  , BW_BW(other)
  , _a(other._a)
  , _r(other._r) // same as b in Lass paper
  , _phi(other._phi)
  , _bgCutOff(other._bgCutOff)
{}

double Lass::q(){
  double q2 = BW_BW::pABSq();
  double qu=0;
  if(q2 > 0) qu=sqrt(q2);
  return qu;
}
double Lass::q0() const{
  double q02 = BW_BW::prSq();
  double qu0=0;
  if(q02 > 0) qu0=sqrt(q02);
  return qu0;
}

complex<double> Lass::BG(){
  const double M_Kpi = BW_BW::mumsRecoMass();

  if( M_Kpi > bgCutOff() ) return 0;
  if( q() <=0 ) return 0;

  const complex<double> den( q()*cotDeltaBg(), -q() );

  const double M_0    = BW_BW::mumsPDGMass();
  const double Gamma0 = BW_BW::mumsWidth();

  const double num = M_Kpi;
  const double myNormalisation = q0()/(M_0*M_0*Gamma0);

  return num * myNormalisation / den * GeV*GeV;
}

double Lass::paraA() const{
  return _a;
}
double Lass::paraR() const{
  return _r;
}
double Lass::paraPhi() const{
  return _phi;
}
double Lass::bgCutOff() const{
  return _bgCutOff;
}
double Lass::cotDeltaBg(){
  double x,y;
  return cotDeltaBg(y,x);
}
double Lass::cotDeltaBg(double& y, double& x){
  double qu=q();
 
  y = 2.0 * paraA()*qu;
  x = 2.0 + paraA()*paraR()*qu*qu;

  return x/y;
}
double Lass::deltaBg(){
  /*
    cot(delta_bg) = 1/(aq) + rq/2
    with q = sqrt(pABSq());

    .               2 + ba q^2
    cot(delta_bg) = --------------
    .                  2aq

    .                  2aq
    tan(delta_bg) = --------------
    .               2 + ba q^2
   */

  double x,y;
  cotDeltaBg(y, x);
  return atan2(y, x);
}

std::complex<double> Lass::getVal(){
  resetInternals();
  complex<double> expIPhi(cos(2*deltaBg()), sin(2*deltaBg()));
  return BG() + BW_BW::getVal()*expIPhi;
}
