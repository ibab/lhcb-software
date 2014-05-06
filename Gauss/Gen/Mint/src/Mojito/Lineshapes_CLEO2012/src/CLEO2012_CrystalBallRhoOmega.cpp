// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/CLEO2012_CrystalBallRhoOmega.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/ParticlePropertiesList.h"

#include <iostream>

using namespace std;

CLEO2012_CrystalBallRhoOmega::CLEO2012_CrystalBallRhoOmega( const AssociatedDecayTree& tree
					  , IDalitzEventAccess* events)
  : CLEO2012_BW_BW(tree, events)
  , _omegaBW(tree, events, 223)
  , _eps("CLEO2012_CrystalBallRhoOmega::eps", 1.14)
  , _beta("ChristalBallRhoOmega::beta", 5.16 *pi/180) //-4.3*pi/180.0)
  , _phi("ChristalBallRhoOmega::phi", 100*pi/180) //107*pi/180.0)
  , _delta("ChristalBallRhoOmega::delta", 2.5*MeV)
  , _aRatioSq("ChristalBallRhoOmega::aRatioSq", 3.9)
  , _mRho(-9999), _mOmega(-9999), _GRho(-9999), _GOmega(-9999)
{
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::rhoBWVal(){
  return CLEO2012_BW_BW::getVal();
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::omegaBWVal(){
  return _omegaBW.getVal();
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::rhoBWValAtResonance(){
  return CLEO2012_BW_BW::getValAtResonance();
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::omegaBWValAtResonance(){
  return _omegaBW.getValAtResonance();
}

double CLEO2012_CrystalBallRhoOmega::p() const{
  return 1.0;
}

double CLEO2012_CrystalBallRhoOmega::q() const{
  return 1.0;
}

double CLEO2012_CrystalBallRhoOmega::eps() const{
  return _eps;
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::expIBeta() const{
  return complex<double>(cos(_beta), sin(_beta));
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::expIPhi() const{
  return complex<double>(cos(_phi), sin(_phi));
}

double CLEO2012_CrystalBallRhoOmega::delta() const{
  return _delta;
}

double CLEO2012_CrystalBallRhoOmega::mRho() const{
  if(_mRho < 0) _mRho = ParticlePropertiesList::mass(113);
  return _mRho;
}

double CLEO2012_CrystalBallRhoOmega::GammaRhoFixed() const{
  if(_GRho < 0) _GRho = ParticlePropertiesList::width(113);
  return _GRho;
}

double CLEO2012_CrystalBallRhoOmega::mOmega() const{
  if(_mOmega < 0) _mOmega = ParticlePropertiesList::mass(223);
  return _mOmega;
}

double CLEO2012_CrystalBallRhoOmega::GammaOmegaFixed() const{
  if(_GOmega < 0) _GOmega = ParticlePropertiesList::width(223);
  return _GOmega;
}

double CLEO2012_CrystalBallRhoOmega::normRho() const{
  return 1;
}

double CLEO2012_CrystalBallRhoOmega::normOmega(){
  double m = mOmega()/mRho();
  double G = GammaOmegaFixed()/GammaRhoFixed();

  double r = sqrt(prSq()/_omegaBW.prSq());

  double d = sqrt((1. + _omegaBW.pABSq())/(1. + pABSq()));
  return m*G*r*d;
}

double CLEO2012_CrystalBallRhoOmega::omegaToRhoAmpRatio() const{
  return sqrt(_aRatioSq);
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::offDiagonalTerm(){
  std::complex<double> num = expIPhi() * delta() * (mRho() + mOmega());
  double dm2 = mOmega()*mOmega() - mRho()*mRho();
  double dmG = mOmega()*GammaOmegaFixed() - mRho()*GofM();
  
  std::complex<double> den(dm2, -dmG);

  return num/den;
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::getVal(){
  resetInternals();
  //  cout << "CLEO2012_CrystalBallRhoOmega called " << endl;
  std::complex<double> rho = rhoBWVal() * normRho();
  std::complex<double> om  = omegaBWVal()
    * eps() * expIBeta()* normOmega();
    cout << "\n \n first term = " << abs(rho) << " second term = " << abs(omegaToRhoAmpRatio() * om * offDiagonalTerm())<< endl;
    std::cout << " Om value = " << abs(omegaBWVal()* normOmega()*omegaToRhoAmpRatio() ) << " offdiaganol term = " << abs(offDiagonalTerm())<<  endl;
   
  return p()*q()*(rho + omegaToRhoAmpRatio() * om * offDiagonalTerm());
}

std::complex<double> CLEO2012_CrystalBallRhoOmega::getValAtResonance(){
  std::complex<double> rho = rhoBWValAtResonance() * normRho();
  std::complex<double> om  = omegaBWValAtResonance()
    * eps() * expIBeta()* normOmega();

  return p()*q()*(rho + omegaToRhoAmpRatio()* om * offDiagonalTerm());
}

DalitzCoordinate CLEO2012_CrystalBallRhoOmega::getDalitzCoordinate(double nSigma)const{
  return CLEO2012_BW_BW::getDalitzCoordinate(nSigma);
}

void CLEO2012_CrystalBallRhoOmega::print(std::ostream& out) const{
  out << "CLEO2012_CrystalBallRhoOmega\n\t with rho:\n";
  CLEO2012_BW_BW::print(out);
  out << "\n\t and with omega:\n";
  _omegaBW.print(out);
}

void CLEO2012_CrystalBallRhoOmega::print(std::ostream& out){
  out << "CLEO2012_CrystalBallRhoOmega\n\t with rho:\n";
  CLEO2012_BW_BW::print(out);
  out << "\n\t and with omega:\n";
  _omegaBW.print(out);
}
    
MINT::counted_ptr<IGenFct> CLEO2012_CrystalBallRhoOmega::generatingFunction() const{
  return CLEO2012_BW_BW::generatingFunction();
}

std::ostream& operator<<(std::ostream& out, const CLEO2012_CrystalBallRhoOmega& amp){

  amp.print(out);
  return out;
}
