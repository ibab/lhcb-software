// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 04 Feb 2014 19:17:59 GMT
#include <iostream>

#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/ParticleProperties.h"

#include "Mint/Rho0Omega.h"

using namespace std;
using namespace MINT;

Rho0Omega::Rho0Omega( const AssociatedDecayTree& tree,
		      IDalitzEventAccess* events )
  : BW_BW(tree, events)
  , _pr("Rho0Omega::pr", 0.1973)
  , _aRatioSq("Rho0Omega::aRatioSq", 3.9)
  , _eps_coh("Rho0Omega::eps_coh", 1.14)
  , _beta("Rho0Omega::beta", -4.3*pi/180.0)
  , _delta("Rho0Omega::delta", 2.5/GeV)
  , _mRho0(-9999), _mOmega(-9999), _GRho0(-9999), _GOmega(-9999)
  , _mPip(ParticlePropertiesList::mass(211)/GeV)
{
}

std::complex<double> Rho0Omega::getVal(){
  resetInternals();

  const double mpipi = (BW_BW::daughterP4(0)+BW_BW::daughterP4(1)).M()/GeV;

  const double rho0_mass = GetRho0Mass();
  const double rho0_width = GetRho0Width();
  const double omega_mass = GetOmegaMass();
  const double omega_width = GetOmegaWidth();

  const double p = Getp(mpipi);
  const double q = Getq(mpipi);

  // Rho0 BW
  const double q0_rho0 = Getq(rho0_mass);
  const std::complex<double> bw_rho0 =
    BW( mpipi, rho0_mass, rho0_width, q, q0_rho0);

  // Omega BW
  const double q0_omega = Getq(omega_mass);
  const std::complex<double> bw_omega =
    BW( mpipi, omega_mass, omega_width, q, q0_omega);

  //Off-diagonal term
  const double tanphi =
    ((omega_mass*omega_width)-
     (rho0_mass*GetGammas(omega_mass,rho0_mass,rho0_width)))/
    ((omega_mass*omega_mass)-(rho0_mass*rho0_mass));
  const double phi = atan(tanphi);

  const double Gammas_rho0 = GetGammas(mpipi, rho0_mass, rho0_width);

  const std::complex<double> off_diag =
    std::polar(1.0, phi) * GetDelta() * (rho0_mass+omega_mass) /
    std::complex<double>((omega_mass*omega_mass)-(rho0_mass*rho0_mass),
			 -((omega_mass*omega_width) - (rho0_mass*Gammas_rho0)));

  //Rho0-Omega function
  const std::complex<double> rho0_omega =
    sqrt(p*q) *
    (bw_rho0 +
     (GetAOmegatoRho0()*bw_omega*GetEpsCoh()*GetExpiBeta()*off_diag));

  return rho0_omega;
}

void Rho0Omega::print( std::ostream& out ) const{
  out << name();
}

void Rho0Omega::print( std::ostream& out ){
  out << name();
}
    
double Rho0Omega::GetAOmegatoRho0() const{
  return sqrt(_aRatioSq);
}

double Rho0Omega::GetEpsCoh() const{
  return _eps_coh;
}

std::complex<double> Rho0Omega::GetExpiBeta() const{
  const double beta = _beta;

  return std::polar(1.0, beta);
}

double Rho0Omega::GetDelta() const{
  return _delta;
}

double Rho0Omega::GetRho0Mass() const{
  if(_mRho0 < 0) _mRho0 = ParticlePropertiesList::mass(113)/GeV;
  return _mRho0;
}

double Rho0Omega::GetOmegaMass() const{
  if(_mOmega < 0) _mOmega = ParticlePropertiesList::mass(223)/GeV;
  return _mOmega;
}

double Rho0Omega::GetRho0Width() const{
  if(_GRho0 < 0) _GRho0 = ParticlePropertiesList::width(113)/GeV;
  return _GRho0;
}

double Rho0Omega::GetOmegaWidth() const{
  if(_GOmega < 0) _GOmega = ParticlePropertiesList::width(223)/GeV;
  return _GOmega;
}

double Rho0Omega::GetPipMass() const{
  return _mPip;
}

double Rho0Omega::Getp( const double& mpipi ) const{
  const TLorentzVector p4pipi =
    (BW_BW::daughterP4(0)+BW_BW::daughterP4(1))*(1.0/GeV);
  const TLorentzVector p4init =
    _theDecay.getParent()->getVal().p(getEvent())*(1.0/GeV);
  const TLorentzVector p4rec = p4init-p4pipi;

  const double minit = p4init.M();
  const double mrec = p4rec.M();

  const double m2init = minit*minit;
  const double mpipi_p2_mrec = (mpipi + mrec)*(mpipi + mrec);
  const double mpipi_m2_mrec = (mpipi - mrec)*(mpipi - mrec);

  const double p =
    sqrt((m2init-mpipi_p2_mrec)*(m2init-mpipi_m2_mrec))/
    (2.0*minit);

  if( (m2init-mpipi_p2_mrec)*(m2init-mpipi_m2_mrec) < 0.0 ){
    std::cout << "p is negative. This should never happen." << std::endl;
    exit(1);
  }

  return p;
}

double Rho0Omega::Getq( const double& mpipi ) const{
  const double s = mpipi*mpipi;
  const double m2pi = GetPipMass()*GetPipMass();

  const double q = sqrt(s-(4.0*m2pi))/2.0;

  return q;
}

double Rho0Omega::GetD1( const double& q ) const{
  const double qoverpr = q/_pr;

  const double D1 = qoverpr * sqrt(2.0/((qoverpr*qoverpr)+1.0));

  return D1;
}

double Rho0Omega::GetB1( const double& q, const double& q0 ) const{
  return GetD1(q)/GetD1(q0);
}

double Rho0Omega::Getrhos( const double& mpipi ) const{
  const double s = mpipi*mpipi;
  const double m2pi = GetPipMass()*GetPipMass();

  const double rhos = 1.0/mpipi*sqrt(s - (4.0*m2pi));

  return rhos;
}

double Rho0Omega::GetGammas( const double& mpipi,
			     const double& m0, const double& Gamma0 ) const{
  const double s = mpipi*mpipi;
  const double s0 = m0*m0;
  const double m2pi = GetPipMass()*GetPipMass();

  const double Gamma =
    Gamma0*pow(((s-(4.0*m2pi))/(s0-(4.0*m2pi))), 1.5)*m0/mpipi;

  return Gamma;
}

std::complex<double> Rho0Omega::BW( const double& mpipi,
				    const double& m0, const double& Gamma0,
				    const double& q, const double& q0 ) const
{
  const double s = mpipi*mpipi;
  const double s0 = m0*m0;

  const double rhos = Getrhos(mpipi);
  const double rhos0 = Getrhos(m0);
  const double B1 = GetB1(q,q0);

  const double Gamma = Gamma0 * rhos / rhos0 * B1 * B1;

  const std::complex<double> bw =
    m0 * Gamma0 / rhos0 * B1 /
    std::complex<double>(s0 - s,-m0*Gamma);

  return bw;
}

std::ostream& operator<<( std::ostream& out, const Rho0Omega& amp ){
  amp.print(out);
  return out;
}
