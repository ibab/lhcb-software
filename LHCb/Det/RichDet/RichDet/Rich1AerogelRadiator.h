// $Id: Rich1AerogelRadiator.h,v 1.3 2001-10-31 16:50:12 rihill Exp $
#ifndef RICHDET_RICH1AEROGELRADIATOR_H
#define RICHDET_RICH1AEROGELRADIATOR_H 1

#include <vector>

#include "RichDet/PhotonRadiator.h"

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichDet/Trajectory.h"

class Rich1AerogelRadiator : public PhotonRadiator {

public:
  Rich1AerogelRadiator (const Rich &rich);

  virtual ~Rich1AerogelRadiator();

  void update (const RichParameters &parameter);

  double refractiveIndex (const double energy) const;

  double refractiveIndex () const;

  bool leaves (const Trajectory &trajectory,
               double &distance) const;

  double scatterFraction (const double theta,
                          const double area) const;

  virtual int scatter (Trajectory &photon, const double energy) const;

  virtual bool refract (Trajectory &photon, 
                        const double energy) const;

private:
  Rich1AerogelRadiator();

  static double randCos2 ();

  std::vector<double> m_parGel;

  double m_width;

  double m_clarity;

  HepPlane3D m_plane1;

  bool m_rayleighScattering;

  bool m_refraction;

  mutable double m_avgRefIndex;

  double m_effRefIndex;

};

#endif // RICHDET_RICH1AEROGELRADIATOR_H
