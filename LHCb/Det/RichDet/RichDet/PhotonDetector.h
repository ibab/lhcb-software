// $Id: PhotonDetector.h,v 1.3 2001-10-31 16:50:12 rihill Exp $
#ifndef RICHDET_PHOTONDETECTOR_H
#define RICHDET_PHOTONDETECTOR_H 1

#include <cassert>
#include <stddef.h>

class Rich;
class PhotonSpectrum;
class RichParameters;
class LocalPosition;
class Pixel;

class Trajectory;

class HepPoint3D;

// The general representation of a Photon detector. Only
// general features are implemented. The concrete
// implementation
// has to be provided in the derived classes, that
// represent concrete Detectors - as for example a Pixel
// HPD in RICH1.

class PhotonDetector {

public:
  PhotonDetector (const Rich &rich);

  virtual ~PhotonDetector();

  virtual void update (const RichParameters &parameter) = 0;

  const Rich & rich () const;

  virtual PhotonSpectrum * photonEfficiency () const = 0;

  virtual Pixel* detect (Trajectory &photon) const = 0;

  virtual Pixel * newPixel (const HepPoint3D &position) const = 0;

  virtual Pixel * newPixel (const LocalPosition &local) const = 0;

  virtual LocalPosition * newLocalPosition (const Pixel &pixel) const = 0;

  virtual LocalPosition * newLocalPosition (const HepPoint3D &position) const=0;

  virtual HepPoint3D globalPosition (const LocalPosition &local) const = 0;

  virtual HepPoint3D globalPosition (const Pixel &pixel) const = 0;

  virtual double area (const Pixel &pixel) const = 0;

  virtual int nrNoisePixelToGenerate () const = 0;

  virtual Pixel * newNoisePixel () const = 0;

  virtual double expSignalBackground (const Pixel &pixel) const = 0;

protected:

  void updateTrajectory(Trajectory& trajectory, const HepPoint3D& pos) const;

private:
  PhotonDetector();

  PhotonDetector(const PhotonDetector &right);

  PhotonDetector & operator=(const PhotonDetector &right);

  const Rich *m_rich;

};

// ==============================================


inline const Rich & PhotonDetector::rich () const
{
  assert( m_rich != NULL );
  return *m_rich;
}

#endif // RICHDET_PHOTONDETECTOR_H
