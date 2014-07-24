#ifndef NONRES_LINESHAPE_HH
#define NONRES_LINESHAPE_HH
// author: Nunya Business (ya.mum@bristol.ac.uk)
// status:  Mon 22 Jul 2014 19:17:59 GMT

#include <complex>

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/NamedParameter.h"

class NonRes : public BW_BW, virtual public ILineshape{
 public:
  /**
     Constructor
   */
  NonRes( const AssociatedDecayTree& tree, 
	  IDalitzEventAccess* events,
	  const std::string& type="" );

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
    return "NonRes("+_theDecay.oneLiner() +")";
  }

  /**
     Destructor
   */
  virtual ~NonRes(){}

 protected:
  /**
     Return shape parameter alpha
     Not possible to vary this in the fit in the current implementation
   */
  double GetAlpha() const;

  const std::string _type;
  MINT::NamedParameter<double> _alpha;
};

std::ostream& operator<<( std::ostream& out, const NonRes& amp );

#endif
//
