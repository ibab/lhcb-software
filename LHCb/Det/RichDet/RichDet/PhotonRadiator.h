#ifndef RICHDET_PHOTONRADIATOR_H
#define RICHDET_PHOTONRADIATOR_H 1

#include <cassert>
#include <stddef.h>
#include "CLHEP/Geometry/Plane3D.h"

//#include "RichGlobal/Definitions.h"
#include "ParticleCode.h"

#include "Rich.h"
class PhotonSpectrum;
class RichParameters;
class ActivePixel;
class Photon;
class Trajectory;
class TrackSegment;

class PhotonRadiator {

public:
  typedef int RadiatorID;        // in principle, would be nicer to have
  enum {CF4, C4F10, Aerogel=0};  // "enum RadiatorID {C4F10, CF4, Aerogel=0}"
                                 // but then looping is ugly

  PhotonRadiator (const RadiatorID id, const Rich &rich);

  virtual ~PhotonRadiator();

  virtual void update (const RichParameters &parameter) = 0;

  const RadiatorID id () const;

  const Rich & rich () const;

  virtual PhotonSpectrum * 
  newObservedSpectrum (const double mass, 
                       const TrackSegment& segment) const;

  virtual bool enters (const Trajectory &trajectory, 
                       double &distance) const;

  virtual bool leaves (const Trajectory &trajectory, 
                       double &distance) const = 0;

  virtual double refractiveIndex (const double energy) const = 0;

  virtual double refractiveIndex () const = 0;

  virtual Photon generatePhoton (const ParticleCode code, 
                                 TrackSegment &segment) const = 0;

  virtual Photon generatePhoton (const double thetaCherenkov, 
                                 const double phiCherenkov, 
                                 const double distCherenkov, 
                                 TrackSegment &segment) const = 0;

  virtual Photon reconstructPhoton (TrackSegment &segment, 
                                    const ActivePixel &pixel) const = 0;

  virtual double momentumThreshold (const ParticleCode particle) const;

  virtual double maxThetaCherenkov () const;

  virtual void efficiency (const ParticleCode particle, 
                           const TrackSegment &segment, 
                           double &signalEfficiency, 
                           double &scatterEfficiency) const;

  virtual double signalFraction (const ParticleCode particle, 
                                 const TrackSegment &segment,
                                 const double theta, 
                                 const double area) const;

  virtual double scatterFraction (const ParticleCode particle, 
                                  const TrackSegment &segment, 
                                  const double theta, 
                                  const double area) const = 0;

  virtual double photonResolution (const double theta) const;

protected:
  Photon radiatePhoton (const ParticleCode code, 
                        TrackSegment &segment) const;

  virtual const PhotonSpectrum & photonEfficiency () const;

  virtual void initPhotonResolution ();

  // Data Members for Class Attributes

  double m_refIndex;
  double m_z0Pos;
  double m_x0Min;
  double m_x0Max;
  double m_y0Min;
  double m_y0Max;

  HepPlane3D m_plane0;

  double m_energyCutoff;

  mutable PhotonSpectrum *m_photonEfficiency;

  bool m_emissionError;
  bool m_chromaticError;
  double m_efficiencyPrecision;
  double m_sigmaPhotonMin;
  double m_sigmaPhotonMax;
  double m_resPar1;
  double m_resPar2;
  double m_mirrorRadius;


private:
  PhotonRadiator();

  PhotonRadiator(const PhotonRadiator &right);

  PhotonRadiator & operator=(const PhotonRadiator &right);

  // Data Members for Class Attributes

  const RadiatorID m_id;

private:
  // Data Members for Associations

  const Rich *m_rich;
};

inline const PhotonRadiator::RadiatorID PhotonRadiator::id () const 
{
  return m_id;
}

inline const Rich & PhotonRadiator::rich () const
{
  assert( m_rich != NULL );
  return *m_rich;
}

#endif // RICHDET_PHOTONRADIATOR_H
