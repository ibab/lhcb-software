// $Id: PhotonRadiator.cpp,v 1.2 2001-10-26 13:02:13 rihill Exp $
#include <cmath>
#include <cassert>
#include "CLHEP/Random/RandFlat.h"

// Rich
#include "RichDet/Rich.h"
// PhotonSpectrum
#include "RichDet/PhotonSpectrum.h"
// PhotonRadiator
#include "RichDet/PhotonRadiator.h"
// PhotonReflector
#include "RichDet/PhotonReflector.h"
// PhotonDetector
#include "RichDet/PhotonDetector.h"
// RichParameters
#include "RichDet/RichParameters.h"
// ActivePixel
#include "RichRec/ActivePixel.h"
// Photon
#include "RichRec/Photon.h"
// Trajectory
#include "RichRec/Trajectory.h"
// TrackSegment
#include "RichRec/TrackSegment.h"

#include <typeinfo>

PhotonRadiator::PhotonRadiator (const RadiatorID id, const Rich &rich)
  : m_photonEfficiency(NULL), m_id(id), m_rich(&rich)
{
}

PhotonRadiator::~PhotonRadiator() 
{
  delete m_photonEfficiency;
}

double PhotonRadiator::momentumThreshold (const ParticleCode particle) const
{
  double refIdx = this->refractiveIndex();
  return Rich::particleMass(particle) / ( refIdx * refIdx - 1. );
}

double PhotonRadiator::maxThetaCherenkov () const
{
  return acos( 1. / this->refractiveIndex() );
}

PhotonSpectrum
*PhotonRadiator::newObservedSpectrum(const double mass,
                                         const TrackSegment &segment) const
{

  double beta   = segment.beta(mass);
  double length = segment.leaves() - segment.enters();

  PhotonSpectrum * spectrum = new PhotonSpectrum(this->photonEfficiency());

  double delta  = m_photonEfficiency->deltaEnergy();
  double energy = m_photonEfficiency->lowerEnergy() + 0.5 * delta;
  int    bins   = m_photonEfficiency->energyBins();

  for( int i = 0; i<bins; ++i ) {

    // skip empty bins

    if ( (*spectrum)[i] <= 0. ) {
      continue;
    }

    // momentum threshold

    double refIdx = this->refractiveIndex(energy);
    if ( beta * refIdx <= 1. ) {
      (*spectrum)[i] = 0.;
      continue;
    };

    // calculate emitted photons for an energy interval

    double a = 1.0 / ( beta * refIdx );
    (*spectrum)[i] *= 370. * length * delta * ( 1.0 - a * a );

    energy += delta;
  }

  return spectrum;

}

bool PhotonRadiator::enters (const Trajectory &trajectory,
                                 double &distance) const
{

  // is it really entering ?
  // cout << typeid(trajectory).name() << endl;


  if ( !trajectory.intersect(m_plane0,distance) ) {
    return false;
  }

  // inside sensitive area ?

  HepPoint3D pos = trajectory.position(distance);
  if ( ( abs(pos.x()) < m_x0Min && abs(pos.y()) < m_y0Min ) ||
       abs(pos.x()) > m_x0Max ||
       abs(pos.y()) > m_y0Max ) {
    return false;
  }


  // right flight direction ?

  HepVector3D dir = trajectory.direction(distance);
  if ( dir.z() < 0. ) {
    return false;
  }

  return true;

}

Photon PhotonRadiator::radiatePhoton (const ParticleCode code,
                                          TrackSegment &segment) const
{

  double energy   = segment.observedSpectrum(code).random();
  double beta     = segment.beta(code);
  double thetaC   = acos( 1. / ( beta * refractiveIndex(energy) ) );
  double phiC     = RandFlat::shoot(0.,M_2PI);
  double distC    = ( m_emissionError ?
                      RandFlat::shoot(segment.enters(),segment.leaves()) :
                      0.5 * ( segment.enters() + segment.leaves() ) );

  return Photon::emitted(energy,thetaC,phiC,distC,segment);

}

const PhotonSpectrum & PhotonRadiator::photonEfficiency () const
{

  if ( m_photonEfficiency == NULL ) {
    m_photonEfficiency = rich().reflector().newPhotonEfficiency();

    double delta  = m_photonEfficiency->deltaEnergy();
    double energy = m_photonEfficiency->lowerEnergy() + 0.5 * delta;
    int    bins   = m_photonEfficiency->energyBins();

    for( int i = 0; i<bins; ++i ) {
      //
      // Energy Cutoff
      //
      if ( energy > m_energyCutoff ) {
        (*m_photonEfficiency)[i] = 0.;
      }
      energy += delta;
    }

  }
  assert( m_photonEfficiency != NULL );
  return *m_photonEfficiency;

}

void PhotonRadiator::efficiency (const ParticleCode particle,
                                     const TrackSegment &segment,
                                     double &signalEfficiency,
                                     double &scatterEfficiency) const
{

  // calculated by MC integration. Photons are simulated and the number of
  // observed signal or scatter photons is counted.

  int signal  = 0;
  int scatter = 0;

  for(int i = 0; i < int(1/m_efficiencyPrecision) ; ++i ) {
    Photon photon =
      this->generatePhoton(particle,const_cast<TrackSegment &>(segment));
    switch( photon.status() ) {
    case Photon::Observed :
      ++signal;
      break;
    case Photon::ScatteredObserved :
      ++scatter;
      break;
    default:
      break;
    }
  }

  signalEfficiency  = signal  * m_efficiencyPrecision;
  scatterEfficiency = scatter * m_efficiencyPrecision;

}

double PhotonRadiator::signalFraction (const ParticleCode particle,
                                           const TrackSegment &segment,
                                           const double theta,
                                           const double area) const
{

  double thtExp = segment.avgThetaCherenkov(particle);
  assert( thtExp > 0. );
  assert( theta >= 0 );
  double sig =  photonResolution(thtExp);
  assert( sig > 0. );
  double del  = theta - thtExp;
  if ( abs(del) > 30. * sig ) {
    return 0;
  }
  double fsig = exp( -0.5*del*del / ( sig*sig ) ) / ( sqrt(2.*M_PI)*M_PI*sig );
  double fare = 4. * area /
    ( m_mirrorRadius * m_mirrorRadius * (theta > 0.00001 ? theta : 0.00001 ) );
  return fsig * fare;

}

double PhotonRadiator::photonResolution (const double theta) const
{

  assert( theta > 0. );
  double tanTheta = tan(theta);
  return sqrt( m_resPar1 + m_resPar2 / (tanTheta*tanTheta) );

}

void PhotonRadiator::initPhotonResolution ()
{

  // (refIdex * refIdx - 1) is tan(thetaMax)**2

  double refIdx = this->refractiveIndex();
  m_resPar1 = m_sigmaPhotonMin*m_sigmaPhotonMin;
  m_resPar2  = ( m_sigmaPhotonMax*m_sigmaPhotonMax - m_resPar1 ) *
    ( refIdx*refIdx - 1);

}

