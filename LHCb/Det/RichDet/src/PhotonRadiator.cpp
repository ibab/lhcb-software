// $Id: PhotonRadiator.cpp,v 1.4 2001-11-15 13:22:49 rihill Exp $
#include <cmath>
#include <cassert>
#include "CLHEP/Random/RandFlat.h"

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonRadiator.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichDet/Trajectory.h"

//#include "RichRec/Photon.h"

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

void PhotonRadiator::updateTrajectory(Trajectory& trajectory,
                                       const HepPoint3D& pos,
                                       const HepVector3D& dir) const
{ 
  trajectory.m_position = pos;
  trajectory.m_direction = dir;
  trajectory.m_direction.setMag(1.0);
}


int PhotonRadiator::scatter (Trajectory &photon, const double energy) const
{
  return 0; // no scattering by default
}

bool PhotonRadiator::refract (Trajectory &photon, const double energy) const
{
  return true; // ...means not absorbed
}

PhotonSpectrum
*PhotonRadiator::newObservedSpectrum(const double mass,
                                     const double beta,
                                     const double length) const
{
  PhotonSpectrum * spectrum = new PhotonSpectrum(this->photonEfficiency());

  double delta  = m_photonEfficiency->deltaEnergy();
  double energy = m_photonEfficiency->lowerEnergy() + 0.5 * delta;
  int    bins   = m_photonEfficiency->energyBins();

  double ref = this->refractiveIndex(energy);
  if (mass == 0.00051) {
    //  cout << "beta " << beta << " n " << ref << " nbeta " << beta * ref << endl;
  }
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

  HepVector3D dir = trajectory.direction(/*distance*/);
  if ( dir.z() < 0. ) {
    return false;
  }

  return true;

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

double PhotonRadiator::signalFraction (const double thtExp,
                                       const double theta,
                                       const double area) const
{
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

