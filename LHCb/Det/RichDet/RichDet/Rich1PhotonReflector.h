
#ifndef Rich1PhotonReflector_h
#define Rich1PhotonReflector_h 1

// PhotonReflector
#include "RichDet/PhotonReflector.h"

class Rich;
class PhotonSpectrum;
class PhotonDetector;
class RichParameters;
class Photon;

class Rich1PhotonReflector : public PhotonReflector {

public:
  Rich1PhotonReflector (const Rich &rich);

  virtual ~Rich1PhotonReflector();


  virtual void update (const RichParameters &parameter);

  PhotonSpectrum * newPhotonEfficiency () const;

  // Additional Public Declarations

protected:
  // Additional Protected Declarations

private:
  Rich1PhotonReflector();

  // Additional Private Declarations

private:
  // Additional Implementation Declarations

};

#endif
