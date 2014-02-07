#ifndef GOUNARIS_SAKURAI_LINESHAPE_HH
#define GOUNARIS_SAKURAI_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"

class GounarisSakurai : public BW_BW, virtual public ILineshape{
 public:
  /**
     Constructor
     Gounaris-Sakurai lineshape describing the P-wave pipi scattering amplitude
     Phys. Rev. Lett. 21, 244 (1968)
   */ 
  GounarisSakurai( const AssociatedDecayTree& decay,
		   IDalitzEventAccess* events )
    : BW_BW(decay, events){}

  /**
     Evaluate Gounaris-Sakurai lineshape
   */
  virtual std::complex<double> getVal();

  /**
     Decay Name
   */
  virtual std::string name() const{
    return "GounarisSakurai("+_theDecay.oneLiner() +")";
  }

  /**
     Destructor
   */
  virtual ~GounarisSakurai(){}

 protected:
  /**
     Return pi+ breakup momentum in the rest frame of the pi+pi- system
   */
  double k( const double& mpipi ) const;

  /**
     Term from the generalised effective-range formula of the
     Chew-Mandelstam type
     Phys. Rev. 119, 467 (1960)
   */
  double h( const double& mpipi ) const;

  /**
     Derivative of h with respect to s
   */
  double hprime( const double& mpipi ) const;

  /**
     Constant depending only on the rho mass
   */
  double d() const;

  /**
     Gounaris-Sakurai numerator
   */
  double sJ() const;

  /**
     Real part of the Gounaris-Sakurai denominator
   */
  double ReGSDen() const;

  /**
     Imaginary part of the Gounaris-Sakurai denominator
     Outstanding Issue: No barrier factor in the width yet
   */
  double ImGSDen() const;

  /**
     Inverse of the Gounaris-Sakurai denominator
   */
  std::complex<double> InvGSDen() const;
};

#endif
//
