#ifndef BW_BW_DIFFERENT_MOTHER_HH
#define BW_BW_DIFFERENT_MOTHER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/BW_BW.h"

// used for CrystalBarrelRhoOmega
class ParticlePropeties;

class BW_BW_DifferentMother : public BW_BW, virtual public ILineshape{
 protected:
  int _alternativeMumPDG;
  virtual const ParticleProperties* mumsProperties() const;
 public:
  BW_BW_DifferentMother(const AssociatedDecayTree& decay
			, int newMumID
			);
};

#endif
//
