// $Id: Rich1AerogelRadiator.h,v 1.2 2001-10-26 13:02:12 rihill Exp $
#ifndef RICHDET_RICH1AEROGELRADIATOR_H
#define RICHDET_RICH1AEROGELRADIATOR_H 1

// Include files
#include <vector>

// PhotonRadiator
#include "RichDet/PhotonRadiator.h"

class Rich;
class PhotonSpectrum;
class PhotonReflector;
class PhotonDetector;
class RichParameters;
class ActivePixel;
class Photon;
class Trajectory;
class TrackSegment;

class Rich1AerogelRadiator : public PhotonRadiator {

public:
  Rich1AerogelRadiator (const Rich &rich);

  virtual ~Rich1AerogelRadiator();

  void update (const RichParameters &parameter);

  double refractiveIndex (const double energy) const;

  double refractiveIndex () const;

  bool leaves (const Trajectory &trajectory,
               double &distance) const;

  Photon generatePhoton (const ParticleCode code,
                         TrackSegment &segment) const;

  Photon generatePhoton (const double thetaCherenkov,
                         const double phiCherenkov,
                         const double distCherenkov,
                         TrackSegment &segment) const;

  Photon reconstructPhoton (TrackSegment &segment,
                            const ActivePixel &pixel) const;

  double scatterFraction (const ParticleCode particle,
                          const TrackSegment &segment,
                          const double theta,
                          const double area) const;
  // Additional Public Declarations

protected:
  // Additional Protected Declarations

private:
  Rich1AerogelRadiator();

  void scatter (Photon &photon) const;

  static double randCos2 ();

  // Data Members for Class Attributes

  vector<double> m_parGel;

  double m_width;

  double m_clarity;

  HepPlane3D m_plane1;

  bool m_rayleighScattering;

  bool m_refraction;

  mutable double m_avgRefIndex;

  double m_effRefIndex;

};

#endif // RICHDET_RICH1AEROGELRADIATOR_H
