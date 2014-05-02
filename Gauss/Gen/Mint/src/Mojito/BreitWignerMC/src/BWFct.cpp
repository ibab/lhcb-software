// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/BWFct.h"

#include "Mint/CLHEPPhysicalConstants.h"

#include <cmath>
#include <iostream>

using namespace std;

BWFct::BWFct(const DalitzCoordinate& c, double M, double Gamma)
  : _coord(c)
  , _M(M)
  , _G(Gamma)
{}
BWFct::BWFct(const BWFct& other)
  : IGenFct()
  , _coord(other._coord)
  , _M(other._M)
  , _G(other._G)
{}

/*
counted_ptr<IGenFct> BWFct::Clone()const{
  counted_ptr<IGenFct> cptr ( (IGenFct*) new BWFct(*this) );
  return cptr;
}
*/

void BWFct::setLimits(double sMin, double sMax){
  //  cout << "setting limits to " << sMin << ", " << sMax << endl;
  _coord.setMin(sMin);
  _coord.setMax(sMax);
}

double BWFct::rhoMi() const{
  double rhoMi = -0.5*pi;
  if(_coord.min() < _coord.max() ){
    rhoMi = coordTransformFromS(_coord.min());
  }
  return rhoMi;
}
double BWFct::rhoMa() const{
  double rhoMa = 0.5*pi;
  if(_coord.min() < _coord.max() ){
    rhoMa = coordTransformFromS(_coord.max());
  }
  return rhoMa;
}
double BWFct::generateRho(TRandom* rnd) const{
  //  bool dbThis = false;

  double rho = rhoMi() + rnd->Rndm()*(rhoMa() - rhoMi());
  return rho;
}

double BWFct::transformedFctMax()const{
  return unitFactor()/(_G*_M);
}
double BWFct::generate(TRandom* rnd) const{
  bool dbThis = false;

  double rho = generateRho(rnd);
  double   s = coordTransformToS(rho);

  if(dbThis) cout << "_M = " << _M
		  << ", _G = " << _G
		  << ", rho = " << rho
		  << ", rhoMi = " << rhoMi()
		  << ", rhoMa = " << rhoMa()
		  << ", sMi = " << _coord.min()
		  << ", sMa = " << _coord.max()
		  << ", s = " << s
		  << endl;

  return s;
}

double BWFct::unitFactor() const{
  return (GeV*GeV)*(GeV*GeV);
}
double BWFct::coordTransformFromS(double s)const{
  return atan( ( s - _M*_M)/(_M*_G) );
}
double BWFct::coordTransformToS(double rho)const{
  return _M * _G * tan(rho) + _M*_M;
}

double BWFct::transformedPDFValue(double rho)const{
  if(rho > rhoMa() || rho < rhoMi()) return 0;
  return 1.0*unitFactor()/(rhoMa() - rhoMi());
}

double BWFct::transformedFctValue(double rho)const{
  if(rho > rhoMa() || rho < rhoMi()) return 0;
  return 1.0*unitFactor()/(_G*_M);
}
double BWFct::transformedIntegral()const{
  return 1.0*unitFactor()*(rhoMa() - rhoMi())/(_G*_M);
}


double BWFct::integral() const{
  return unitFactor()*(rhoMa() - rhoMi())/(_G*_M);
}

double BWFct::generatingPDFValue(double sij) const{
  bool dbThis=false;

  double nr = integral();
  if(nr <= 0) return 0;
  double fct = generatingFctValue(sij);
  if(dbThis){
    cout << " BWFct::generatingPDFValue "
	 << _coord
	 << " _M^2 " << _M*_M
	 << " _G = " << _G
	 << ", sij = " << sij
	 << " returning " << fct << " / " << nr
	 << " = " << fct/nr
	 << endl;
  }
  return fct/nr;
}
double BWFct::generatingFctValue(double sij) const{
  //  bool dbThis = false;
  if(sij < _coord.min() || sij > _coord.max()) return 0;

  double m2diff = _M * _M - sij;
  double fct = unitFactor()/(m2diff*m2diff + _M*_M * _G*_G);
  return fct;
}

DalitzCoordinate BWFct::getCoordinate() const{
  return _coord;
}
void BWFct::setCoordinate(const DalitzCoordinate& c){
  _coord = c;
}

//
