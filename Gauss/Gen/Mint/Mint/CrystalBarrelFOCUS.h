#ifndef CRYSTAL_BARREL_FORTRAN
#define CRYSTAL_BARREL_FORTRAN
// this is the FOCUS implementation of the Crystal Barrel Lineshape, 
// literally translated from FORTRAN
// (with added features to make it function in the framework)

#include "Mint/BW_BW.h"

#include "Mint/ILineshape.h"
#include "Mint/ILineshape.h"
#include "Mint/AssociatedDecayTree.h"

#include "Mint/IGenFct.h"
#include "TRandom.h"

#include "Mint/counted_ptr.h"
#include <complex>

class CrystalBarrelFOCUS : public BW_BW, virtual public ILineshape{
 protected:
  mutable double _mRho, _GRho, _mOmega, _GOmega;
  std::complex<double> AlbertosFunction(double sij);

  double mRho() const;
  double GammaRhoFixed() const;
  double mOmega() const;
  double GammaOmegaFixed() const;

  std::complex<double> BW (double s, double m_w, double gamma) const;

 public:
  CrystalBarrelFOCUS( const AssociatedDecayTree& decay);
  CrystalBarrelFOCUS(const CrystalBarrelFOCUS& other);
  virtual ~CrystalBarrelFOCUS();

  virtual std::complex<double> getVal(IDalitzEvent& evt);

  virtual void print(std::ostream& out = std::cout) const;
  virtual void print(IDalitzEvent& evt, std::ostream& out = std::cout);

  virtual std::string name() const{
    return "CrystalBarrel_ala_FOCUS("+_theDecay.oneLiner() +")";
  }

};

std::ostream& operator<<(std::ostream& out, const CrystalBarrelFOCUS& amp);

#endif
//
