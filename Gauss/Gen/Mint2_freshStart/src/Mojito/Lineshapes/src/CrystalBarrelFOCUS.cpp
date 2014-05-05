#include "Mint/CrystalBarrelFOCUS.h"

#include "Mint/ParticlePropertiesList.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include <complex>
#include <cmath>

using namespace std;
using namespace MINT;

CrystalBarrelFOCUS::CrystalBarrelFOCUS( const AssociatedDecayTree& decay)
  : BW_BW(decay)
  , _mRho(-9999.0)
  , _GRho(-9999.0)
  , _mOmega(-9999.0)
  , _GOmega(-9999.0)
{
}
CrystalBarrelFOCUS::CrystalBarrelFOCUS( const CrystalBarrelFOCUS& other)
  : ILineshape()
  , BW_BW(other)
  , _mRho(other._mRho)
  , _GRho(other._GRho)
  , _mOmega(other._mOmega)
  , _GOmega(other._GOmega)
{}

double CrystalBarrelFOCUS::mRho() const{
  if(_mRho < 0) _mRho = ParticlePropertiesList::mass(113);
  return _mRho;
}

double CrystalBarrelFOCUS::GammaRhoFixed() const{
  if(_GRho < 0) _GRho = ParticlePropertiesList::width(113);
  return _GRho;
}

double CrystalBarrelFOCUS::mOmega() const{
  if(_mOmega < 0) _mOmega = ParticlePropertiesList::mass(223);
  return _mOmega;
}

double CrystalBarrelFOCUS::GammaOmegaFixed() const{
  if(_GOmega < 0) _GOmega = ParticlePropertiesList::width(223);
  return _GOmega;
}

std::complex<double> CrystalBarrelFOCUS::BW (double s, double m_w, double gamma) const{
  std::complex<double> invBW(m_w*m_w - s, -m_w * gamma);
  return -1.0/invBW;
  //return m_w * gamma / invBW;
}
std::complex<double> CrystalBarrelFOCUS::AlbertosFunction(double spipi_in){
  bool dbThis=false;

  double  gamma, gamma_r, gamma_w, rho, rho0, pstr, pstr0, pstrD;
  double  fr, fr0, fD, mD /*, s*/, m, spipi, mKK, m_r, m_w, mpi;
  double  alpha, beta, epsilon, delta /*, D1*/;
  complex<double>  bwr, bww;

  spipi = spipi_in/(GeV*GeV);// I use MeV, Alberto GeV

  m_w = 0.782;
  gamma_w = 0.00840;
  mpi = 0.139569950;
  alpha = 1.750;
  beta = 0.090;
  delta = 0.00250;
  epsilon = 1.140;
  //epsilon = -10;
  
  m = sqrt(spipi);
  mKK = 0.987340;
  mD = 1.90;
  m_r = mRho()/GeV;//m_rho;
  gamma_r = GammaRhoFixed()/GeV;//w_rho;
  
  double arg = m*m - 4.0*mpi*mpi;
  if(arg < 0) return 0;
  pstr  = sqrt (arg) / 2.0;
  pstr0 = sqrt (m_r*m_r - 4.0 * mpi*mpi) / 2.0;
  double arg2 = (mD*mD - (m + mKK)*(m + mKK)) *
    (mD*mD - (m - mKK)*(m - mKK) );
  if(arg2 < 0) return 0;

  pstrD = 1.0 / (2.0 * mD) * sqrt(arg2);

  double arg3 = 1.0 - 4.* mpi*mpi / m*m;
  if(arg3 < 0) return 0;
  rho  = sqrt (arg3);
  rho0 = sqrt (1.0 - 4.* mpi*mpi / m_r*m_r);

  fD  = sqrt (pstrD/0.20) * sqrt (2.0/( pow(pstrD/0.20,2)+1.0));
  fr  = sqrt (pstr/0.20)  * sqrt (2.0/( pow(pstr/0.20,2) +1.0));
  fr0 = sqrt (pstr0/0.20) * sqrt (2.0/( pow(pstr0/0.20,2)+1.0));
  
  gamma = gamma_r * (rho/rho0) * (fr/fr0)*(fr/fr0);
  bwr   = fD * m_r * (gamma_r / rho0) * BW (m*m, m_r, gamma);
  bwr   = bwr * (fr/fr0);

  rho0  = sqrt (1.0 - 4.* mpi*mpi / m_w*m_w);
  pstr0 = sqrt (m_w*m_w - 4.0 * mpi*mpi) / 2.0;

  fr0 = sqrt (pstr0/0.20) * sqrt (2.0/( pow(pstr0/0.20, 2)+1.0));

  gamma = gamma_w * (rho/rho0) * (fr/fr0)*(fr/fr0);
  bww   = fD * m_w * (gamma_w / rho0) * BW (m*m,m_w,gamma);
  bww   = bww * (fr/fr0);

  complex<double> cmplx_i(0,1);
  complex<double> expIPhase(cos(beta + alpha), sin(beta + alpha));
  complex<double> BWrho = sqrt (pstrD * pstr) * 
    (bwr + bww  * epsilon * expIPhase *
     delta * (m_r + m_w) / (m_w*m_w - m_r*m_r - cmplx_i *
			    (m_w * gamma_w - m_r * gamma_r)));
  
  
  if(dbThis){
    cout << "returning BWrho = " << BWrho 
	 << " for spipi = " << spipi << endl;
  }
  return BWrho;

}

std::complex<double> CrystalBarrelFOCUS::getVal(IDalitzEvent& evt){
  resetInternals();
  setEventPtr(evt);
  std::complex<double> returnVal = AlbertosFunction(mumsRecoMass2());
  return returnVal;
}

void CrystalBarrelFOCUS::print(std::ostream& out) const{
  out << name();
}

void CrystalBarrelFOCUS::print(IDalitzEvent& evt, std::ostream& out){
  out << name();
  BW_BW::print(evt, out);
}

std::ostream& operator<<(std::ostream& out, const CrystalBarrelFOCUS& amp){

  amp.print(out);
  return out;
}

CrystalBarrelFOCUS::~CrystalBarrelFOCUS(){

}
//

