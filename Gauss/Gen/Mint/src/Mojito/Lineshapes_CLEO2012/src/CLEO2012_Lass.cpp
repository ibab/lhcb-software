// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/CLEO2012_Lass.h"

#include <iostream>
#include <cmath>

using namespace std;
using namespace MINT;

// Lass parameterised a la BaBar, Phys.Rev.D78:012004,2008
// (and of course originally developed by Lass,
// D. Aston et al. (LASS), Nucl. Phys. B296, 493 (1988).

CLEO2012_Lass::CLEO2012_Lass( const AssociatedDecayTree& decay
	    , IDalitzEventAccess* events)
  : CLEO2012_BW_BW(decay, events)
  , _a("CLEO2012_Lass::a", 2.07/GeV)
  , _r("CLEO2012_Lass::r", 3.32/GeV) // same as b in Lass paper
  , _phi("CLEO2012_Lass::phi", 0*pi/180.0)
  , _bgCutOff("CLEO2012_Lass::bgCutOff", 1.8*GeV)
{}

CLEO2012_Lass::CLEO2012_Lass(const CLEO2012_Lass& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , ILineshape()
  , CLEO2012_BW_BW(other)
  , _a(other._a)
  , _r(other._r) // same as b in Lass paper
  , _phi(other._phi)
  , _bgCutOff(other._bgCutOff)
{}

double CLEO2012_Lass::q(){
  double q2 = CLEO2012_BW_BW::pABSq();
  double qu=0;
  if(q2 > 0) qu=sqrt(q2);
  return qu;
}
double CLEO2012_Lass::q0() const{
  double q02 = CLEO2012_BW_BW::prSq();
  double qu0=0;
  if(q02 > 0) qu0=sqrt(q02);
  return qu0;
}
complex<double> CLEO2012_Lass::BG(){
  double M_Kpi  = CLEO2012_BW_BW::mumsRecoMass();
  if(M_Kpi > bgCutOff()) return 0;
  if(q() <=0 ) return 0;

  complex<double> den(q()*cotDeltaBg(), -q());
  
  double M_0    = CLEO2012_BW_BW::mumsPDGMass();
  double Gamma0 = CLEO2012_BW_BW::mumsWidth();
  
  double num = M_Kpi;
  double myNormalisation = q0()/(M_0*M_0*Gamma0);

  return num * myNormalisation / den;
}

double CLEO2012_Lass::paraA() const{
  return _a;
}
double CLEO2012_Lass::paraR() const{
  return _r;
}
double CLEO2012_Lass::paraPhi() const{
  return _phi;
}
double CLEO2012_Lass::bgCutOff() const{
  return _bgCutOff;
}
double CLEO2012_Lass::cotDeltaBg(){
  double x,y;
  return cotDeltaBg(y,x);
}
double CLEO2012_Lass::cotDeltaBg(double& y, double& x){
  double qu=q();
 
  y = 2.0 * paraA()*qu;
  x = 2.0 + paraA()*paraR()*qu*qu;

  return x/y;
}
double CLEO2012_Lass::deltaBg(){
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

std::complex<double> CLEO2012_Lass::getVal(){
  resetInternals();
  complex<double> expIPhi(cos(2*deltaBg()), sin(2*deltaBg()));
  return BG() + CLEO2012_BW_BW::getVal()*expIPhi;
}
