#ifndef LASS_LINESHAPE_HH
#define LASS_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/ILineshape.h"
#include "Mint/BW_BW.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/NamedParameter.h"

#include <complex>
/*
  Lass parameterised a la BaBar, Phys.Rev.D78:012004,2008 also
  available as: arXiv:0803.4451[hep-ex], SLAC-PUB-13189,
  BABAR-PUB-08/005 (and of course originally developed by Lass,
  D. Aston et al. (LASS), Nucl. Phys. B296, 493 (1988)).
  
  There is a normalisation difference in this implementation compared
  to BaBar: The whole thing is multiplied by q0()/(M_0*M_0*Gamma0); to
  be consistent with the usual BW normalisation (or lack thereof)
*/

class Lass : public BW_BW, virtual public ILineshape{
 protected:
  MINT::NamedParameter<double> _a, _r, _phi, _bgCutOff;

  std::complex<double> BG();
  double paraA()const;
  double paraR()const;
  double paraPhi() const;
  double bgCutOff() const;
  double q();
  double q0() const;
  double cotDeltaBg();
  double cotDeltaBg(double& y, double& x);
  double deltaBg();

 public:
  
  Lass( const AssociatedDecayTree& decay
	, IDalitzEventAccess* events);
  Lass(const Lass& other);

  virtual std::complex<double> getVal();

  virtual std::string name() const{
    return "LASS("+_theDecay.oneLiner() +")";
  }
  virtual ~Lass(){}
};

#endif
//
