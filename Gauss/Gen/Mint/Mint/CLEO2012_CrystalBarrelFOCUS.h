#ifndef CLEO2012_CRYSTAL_BARREL_FORTRAN
#define CLEO2012_CRYSTAL_BARREL_FORTRAN
// this is the FOCUS implementation of the Crystal Barrel Lineshape, 
// literally translated from FORTRAN
// (with added features to make it function in the framework)

#include "Mint/CLEO2012_BW_BW.h"

#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"
#include "Mint/DalitzEventAccess.h"
#include "Mint/IDalitzEventAccess.h"

#include "Mint/IGenFct.h"
#include "TRandom.h"

#include "Mint/counted_ptr.h"
#include <complex>

class CLEO2012_CrystalBarrelFOCUS : public CLEO2012_BW_BW, virtual public ILineshape{
 protected:
  mutable double _mRho, _GRho, _mOmega, _GOmega;
  std::complex<double> AlbertosFunction(double sij);

  double mRho() const;
  double GammaRhoFixed() const;
  double mOmega() const;
  double GammaOmegaFixed() const;

  std::complex<double> BW (double s, double m_w, double gamma) const;

 public:
  CLEO2012_CrystalBarrelFOCUS( const AssociatedDecayTree& decay
		  , IDalitzEventAccess* events);
  CLEO2012_CrystalBarrelFOCUS(const CLEO2012_CrystalBarrelFOCUS& other);
  virtual ~CLEO2012_CrystalBarrelFOCUS();

  virtual std::complex<double> getVal();
  virtual std::complex<double> getValAtResonance();
  virtual void print(std::ostream& out = std::cout) const;
  virtual void print(std::ostream& out = std::cout);

  virtual std::string name() const{
    return "CLEO2012_CrystalBarrel_ala_FOCUS("+_theDecay.oneLiner() +")";
  }

};

std::ostream& operator<<(std::ostream& out, const CLEO2012_CrystalBarrelFOCUS& amp);

#endif
//
