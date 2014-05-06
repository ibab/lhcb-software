#ifndef CLEO2012_LASS_LINESHAPE_HH
#define CLEO2012_LASS_LINESHAPE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "ILineshape.h"
#include "CLEO2012_BW_BW.h"
#include "AssociatedDecayTree.h"
#include "IDalitzEventAccess.h"
#include "CLHEPSystemOfUnits.h"
#include "CLHEPPhysicalConstants.h"
#include "NamedParameter.h"

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

class CLEO2012_Lass : public CLEO2012_BW_BW, virtual public ILineshape{
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
  
  CLEO2012_Lass( const AssociatedDecayTree& decay
	, IDalitzEventAccess* events);
  CLEO2012_Lass(const CLEO2012_Lass& other);

  virtual std::complex<double> getVal();

  virtual std::string name() const{
    return "LASS("+_theDecay.oneLiner() +")";
  }
  virtual ~CLEO2012_Lass(){}
};

#endif
//
