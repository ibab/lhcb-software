#ifndef CLEO2012_BW_BW_DIFFERENT_MOTHER_HH
#define CLEO2012_BW_BW_DIFFERENT_MOTHER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include "Mint/CLEO2012_BW_BW.h"

// used for CrystalBarrelRhoOmega
class ParticlePropeties;

class CLEO2012_BW_BW_DifferentMother : public CLEO2012_BW_BW, virtual public ILineshape{
 protected:
  int _alternativeMumPDG;
  virtual const ParticleProperties* mumsProperties() const;
 public:
  CLEO2012_BW_BW_DifferentMother(const AssociatedDecayTree& decay
				 , IDalitzEventAccess* events
				 , int newMumID
				 );
};

#endif
//
