// Include files
#include <cmath>

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichRec/ActivePixel.h"
#include "RichRec/Photon.h"
#include "RichRec/TrackSegment.h"
#include "RichDet/RichXGasRadiator.h"

RichXGasRadiator::RichXGasRadiator (const RadiatorID id, const Rich &rich)
  : PhotonRadiator(id,rich), m_parGas(2), m_avgRefIndex(0.)
{
}

RichXGasRadiator::~RichXGasRadiator() {}

void RichXGasRadiator::update (const RichParameters &parameter) {

  switch(this->rich().id()) {
  case Rich::Rich1 :

    // m_z0Pos          = parameter.getValue("Rich1.GRad.Z0Pos",    110.0    );
    // m_x0Max          = parameter.getValue("Rich1.GRad.X0Max",     36.0    );
    // m_y0Max          = parameter.getValue("Rich1.GRad.Y0Max",     36.0    );
    // m_mirrorRadius   = parameter.getValue("Rich1.Refl.Radius",190.);

    m_refIndex       = parameter.getValue("Rich1.GRad.RefIndex",   1.0014 );
    m_z0Pos          = parameter.getValue("CDF.RIC1.ZSGAS1",    110.0    );
    m_x0Min          = parameter.getValue("Rich1.GRad.X0Min",      2.6    );
    m_x0Max          = parameter.getValue("CDF.RIC1.XSGAS1",     36.0    );
    m_y0Min          = parameter.getValue("Rich1.GRad.Y0Min",      2.6    );
    m_y0Max          = parameter.getValue("CDF.RIC1.YSGAS1",     36.0    );

    m_parGas[0]      = parameter.getValue("Rich1.GRad.ParGas0",   338.5   );
    m_parGas[1]      = parameter.getValue("Rich1.GRad.ParGas1",   358.7   );

    m_energyCutoff   = parameter.getValue("Rich1.GRad.Cutoff",    5.5   );

    m_emissionError  = parameter.getValue("Rich1.GRad.EmissionError", true );
    m_chromaticError = parameter.getValue("Rich1.GRad.ChromaticError", true );

    m_efficiencyPrecision =
      parameter.getValue("Rich1.GRad.EfficiencyPrecision",0.01);

    m_sigmaPhotonMin =
      parameter.getValue("Rich1.GRad.SigmaPhotonMin",0.00054);
    m_sigmaPhotonMax =
      parameter.getValue("Rich1.GRad.SigmaPhotonMax",0.00076);

    m_mirrorRadius   = parameter.getValue("CDF.RIC1.RCMIR1",190.);

    break;
  case Rich::Rich2 :

    // m_z0Pos           = parameter.getValue("Rich2.GRad.Z0Pos",    952.0    );
    // m_x0Max           = parameter.getValue("Rich2.GRad.X0Max",    114.8    );
    // m_y0Max           = parameter.getValue("Rich2.GRad.Y0Max",     95.5    );
    // m_mirrorRadius        = parameter.getValue("Rich2.Refl.Radius",820.);

    m_refIndex        = parameter.getValue("Rich2.GRad.RefIndex",   1.0005 );
    m_z0Pos           = parameter.getValue("CDF.RIC2.ZSGAS2",    952.0    );
    m_x0Min           = parameter.getValue("Rich2.GRad.X0Min",      9.5    );
    m_x0Max           = parameter.getValue("CDF.RIC2.XSGAS2",    114.8    );
    m_y0Min           = parameter.getValue("Rich2.GRad.Y0Min",      9.5    );
    m_y0Max           = parameter.getValue("CDF.RIC2.YSGAS2",     95.5    );
 
    m_parGas[0]       = parameter.getValue("Rich2.GRad.ParGas0",   382.94   );
    m_parGas[1]       = parameter.getValue("Rich2.GRad.ParGas1",   401.55   );

    m_energyCutoff    = parameter.getValue("Rich2.GRad.Cutoff",    5.0   );

    m_emissionError   = parameter.getValue("Rich2.GRad.EmissionError", true );
    m_chromaticError  = parameter.getValue("Rich2.GRad.ChromaticError", true );

    m_efficiencyPrecision =
      parameter.getValue("Rich2.GRad.EfficiencyPrecision",0.01);

    m_sigmaPhotonMin      =
      parameter.getValue("Rich2.GRad.SigmaPhotonMin",0.00021);
    m_sigmaPhotonMax      =
      parameter.getValue("Rich2.GRad.SigmaPhotonMax",0.00030);

    m_mirrorRadius        = parameter.getValue("CDF.RIC2.RCMIR2",820.);
    break;

  default:
    assert( false );
  };

  m_plane0 = HepPlane3D(0.,0.,1.,-m_z0Pos);

  // cached results that could be invalid after an update

  delete m_photonEfficiency;
  m_photonEfficiency = NULL;
  m_avgRefIndex = 0.;

  this->initPhotonResolution();

}

double RichXGasRadiator::refractiveIndex (const double energy) const
{

  if ( m_chromaticError ) {
    return 1. + ( m_refIndex - 1. ) * m_parGas[0] / ( m_parGas[1] - energy );
  } else {
    return m_refIndex;
  }

}

double RichXGasRadiator::refractiveIndex () const
{
  if ( m_avgRefIndex <= 0. ) {
    double avgEnergy = this->photonEfficiency().averageEnergy();
    m_avgRefIndex = this->refractiveIndex(avgEnergy);
  }
  return m_avgRefIndex;
}

bool RichXGasRadiator::leaves (const Trajectory &trajectory,
                               double &distance) const
{

  // If a track would not hit the mirror, one would have to check how
  // else it leaves the system ... For the future.

  if ( ! rich().reflector().intersectMirror( trajectory, distance ) ) {
    return  false;
  }
  return trajectory.direction(distance).z() > 0.;

}

Photon RichXGasRadiator::generatePhoton (const ParticleCode code,
                                         TrackSegment &segment) const
{
  Photon photon = this->radiatePhoton(code,segment);

  if ( photon.status() == Photon::Emitted ) {
    rich().reflector().reflect(photon);
  }

  if ( photon.status() == Photon::Reflected ) {
    rich().detector().detect(photon);
  }

  return photon;
}

Photon RichXGasRadiator::generatePhoton (const double thetaCherenkov,
                                         const double phiCherenkov,
                                         const double distCherenkov,
                                         TrackSegment &segment) const
{
  Photon photon = Photon::emitted(0.,thetaCherenkov,phiCherenkov,
                                  distCherenkov,segment);

  if ( photon.status() == Photon::Emitted ) {
    rich().reflector().reflect(photon);
  }

  if ( photon.status() == Photon::Reflected ) {
    rich().detector().detect(photon);
  }

  return photon;
}

Photon RichXGasRadiator::reconstructPhoton (TrackSegment &segment,
                                            const ActivePixel &pixel) const
{

  if ( ! segment.active() ) {
    return Photon::notReconstructed(pixel.pixel(),segment);
  }

  // No intermediate wall

  HepPoint3D detection = pixel.globalPosition();
  double distance = 0.5 * ( segment.enters() + segment.leaves() );
  double fraction = 1.;

  if ( Rich::Rich1 == this->rich().id() ) {
    double enter = segment.enters();
    double delta = segment.leaves() - enter;
    HepPoint3D emission1  = segment.position(enter+0.01*delta);
    HepPoint3D emission2  = segment.position(enter+0.99*delta);
    HepPoint3D reflection1;
    HepPoint3D reflection2;
    bool ok1 =
      rich().reflector().reflectionPoint(emission1,detection,reflection1);
    bool ok2 =
      rich().reflector().reflectionPoint(emission2,detection,reflection2);
    //      cout << "Emission1" << emission1 << " :" << enter + 0.01*delta << endl;
    //      cout << "Emission2" << emission2 << " :" << enter + 0.99*delta << endl;
    //      cout << "Reflection1" << reflection1 << " : " << ok1 << endl;
    //      cout << "Reflection2" << reflection2 << " : " << ok2 << endl;
    double fracx = 999.;
    double fracy = 999.;
    if ( ok1 && ok2 ) {
      // Nop
    } else if ( ok1 ) {

      if ( reflection1.x() * reflection2.x() < 0. ) {
        fracx = abs( reflection1.x() / (reflection1.x() - reflection2.x()));
      }
      if ( reflection1.y() * reflection2.y() < 0. ) {
        fracy = abs( reflection1.y() / (reflection1.y() - reflection2.y()));
      }
      fraction = ( fracx < fracy ? fracx : fracy );
      distance = enter + 0.5 * fraction * delta;

    } else if ( ok2 ) {

      if ( reflection1.x() * reflection2.x() < 0. ) {
        fracx = abs( reflection2.x() / (reflection1.x() - reflection2.x()));
      }
      if ( reflection1.y() * reflection2.y() < 0. ) {
        fracy = abs( reflection2.y() / (reflection1.y() - reflection2.y()));
      }
      fraction = ( fracx < fracy ? fracx : fracy );
      distance = enter + ( 1. - 0.5 * fraction ) * delta;

    } else {
      return Photon::notReconstructed(pixel.pixel(),segment);
    }

    // intermediate wall

  } else if ( Rich::Rich2 == this->rich().id() ) {

    HepPoint3D entry = segment.position(segment.enters());
    double dist;
    bool ok = segment.intersect(HepPlane3D(0.,1.,0.,0.),dist);
    bool entryOk = entry.x() * detection.x() > 0.;

    // Track has intersection inside sensitive volume

    if ( ok && dist > segment.enters() && dist < segment.leaves() ) {

      if ( entryOk ) {
        dist = 0.5 * ( segment.enters() + dist );
        fraction = dist / distance;
        distance = dist;
      } else {
        dist = 0.5 * ( dist + segment.leaves() );
        fraction = dist / distance;
        distance = dist;
      }

    } else {

      if ( !entryOk ) {
        return Photon::notReconstructed(pixel.pixel(),segment);
      }

    }

  }

  HepPoint3D emission = segment.position(distance);
  // cout << "Final Emission" << emission << " : " << distance << endl;
  HepPoint3D reflection;
  bool ok = rich().reflector().reflectionPoint(emission,detection,reflection);
  // cout << "Final " << reflection << " : " << ok << endl;
  if ( ! ok ) {
    return Photon::notReconstructed(pixel.pixel(),segment);
  }

  HepVector3D emissionDirection = reflection - emission;
  double theta, phi;
  segment.angleToDirection(distance,emissionDirection,
                           HepVector3D(1.,0.,0.),theta,phi);

  //    if ( theta > this->maxThetaCherenkov() + 0.02 ) {
  //    return Photon::notReconstructed(pixel.pixel(),segment);
  //   }
  return Photon::reconstructed(distance,theta,phi,fraction,
                               pixel.pixel(),segment);

}

double RichXGasRadiator::scatterFraction (const ParticleCode particle,
                                          const TrackSegment &segment,
                                          const double theta,
                                          const double area) const
{
  return 0.;
}
