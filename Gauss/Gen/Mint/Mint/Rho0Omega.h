#ifndef RHO0OMEGA_LINESHAPE_HH
#define RHO0OMEGA_LINESHAPE_HH
// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 04 Feb 2014 19:17:59 GMT

#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"

class Rho0Omega : public BW_BW, virtual public ILineshape{
 public:
  /**
     Constructor
     Lineshape taking rho0-omega interference into account
     Default parameters taken from the Crystal Barrel Collaboration
     Phys. Lett. B 411, 354 (1997)
   */
  Rho0Omega( const AssociatedDecayTree& tree, 
	     IDalitzEventAccess* events );

  /**
     Evaluate rho0-omega lineshape from the Crystal Barrel measurement
   */
  virtual std::complex<double> getVal();

  /**
     Print decay
   */
  virtual void print( std::ostream& out = std::cout ) const;

  /**
     Print decay
   */
  virtual void print( std::ostream& out = std::cout );

  /**
     Decay Name
   */
  virtual std::string name() const{
    return "Rho0Omega("+_theDecay.oneLiner() +")";
  }

  /**
     Destructor
   */
  virtual ~Rho0Omega(){}

 protected:
  /**
     Production amplitude ratio of omega to rho0
     Amplitude ratio-squared determined by the Crystal Barrel measurement
     Default value is 3.9 +/- 0.4
   */
  double GetAOmegatoRho0() const;

  /**
     Magnitude of the off-diagonal element in the rho0-omega mixing matrix
     Free parameter in the Crystal Barrel measurement
     Default value is 1.14 +/- 0.15 +/- 0.13
   */
  double GetEpsCoh() const;

  /**
     Phase of the off-diagonal element in the rho0-omega mixing matrix
     Free parameter in the Crystal Barrel measurement
     Default value is -4.3 +/- 1.8 +/- 3.9 degrees
   */
  std::complex<double> GetExpiBeta() const;

  /**
     Magnitude of the off-diagonal element in the rho0-omega mixing matrix
     Derived from rho-omega interference
     Fixed parameter in the Crystal Barrel measurement
     Default value is 2.5 +/- 0.2 MeV
   */
  double GetDelta() const;

  /**
     Return rho0 mass
     Not possible to vary this in the fit in the current implementation
   */
  double GetRho0Mass() const;

  /**
     Return omega mass
     Not possible to vary this in the fit in the current implementation
   */
  double GetOmegaMass() const;

  /**
     Return rho0 width
     Not possible to vary this in the fit in the current implementation
   */
  double GetRho0Width() const;

  /**
     Return omega width
     Not possible to vary this in the fit in the current implementation
   */
  double GetOmegaWidth() const;

  /**
     Return pi+ mass
   */
  double GetPipMass() const;

  /**
     Return pi+pi- breakup momentum in the rest frame of the mother
     of the pi+pi-
   */
  double Getp( const double& mpipi ) const;

  /**
     Return pi+ breakup momentum in the rest frame of the pi+pi- system
   */
  double Getq( const double& mpipi ) const;

  /**
     Return barrier factor
     pi+pi- radius set by the Crystal Barrel measurement
     Default value is 197.3 MeV/c
   */
  double GetD1( const double& q ) const;

  /**
     Return normalised barrier factors
   */
  double GetB1( const double& q, const double& q0 ) const;

  /**
     Return pi+pi- phase space factor
   */
  double Getrhos( const double& mpipi ) const;

  /**
     Return mass-dependent width
     Not given in the Crystal Barrel reference
     Given in Phys. Lett. B 354, 14 (1995)
   */
  double GetGammas( const double& mpipi,
		    const double& m0, const double& Gamma0 ) const;

  /**
     Return Crystal Barrel measurement definition of the Breit-Wigner
     Note that the production barrier factor of Eq. 2, D1(p), has been
     removed as it is equivalent to the decay barrier factor of the
     rho0 mother which is already included higher up in the isobar chain
   */
  std::complex<double> BW( const double& mpipi,
			   const double& m0, const double& Gamma0,
			   const double& q, const double& q0 ) const;

  const MINT::NamedParameter<double> _pr, _aRatioSq, _eps_coh, _beta, _delta;

  mutable double _mRho0, _mOmega, _GRho0, _GOmega;
  const double _mPip;
};

std::ostream& operator<<( std::ostream& out, const Rho0Omega& amp );

#endif
//
