#ifndef Rich2PhotonReflector_h
#define Rich2PhotonReflector_h 1

// PhotonReflector
#include "RichDet/PhotonReflector.h"

class Rich;
class PhotonSpectrum;
class PhotonDetector;
class RichParameters;
class Trajectory;

//class Photon;

class Rich2PhotonReflector : public PhotonReflector {

public:
  Rich2PhotonReflector (const Rich &rich);

  virtual ~Rich2PhotonReflector();

  virtual void update (const RichParameters &parameter);

  PhotonSpectrum * newPhotonEfficiency () const;

  bool reflect (Trajectory &photon) const;

  static HepPlane3D setPlane (const double xMin,
                              const double xMax,
                              const double zMin,
                              const double zMax);

  virtual bool reflectionPoint (const HepPoint3D &emission, 
                                const HepPoint3D &detection, 
                                HepPoint3D &reflection) const;

  // Additional Public Declarations

protected:
  // Additional Protected Declarations

private:
  Rich2PhotonReflector();

  // Data Members for Class Attributes

  std::vector<HepPlane3D> flatMirror_;

  // Additional Private Declarations

private:
  // Additional Implementation Declarations

};

#endif
