
// Include files
#include <vector>
#include <cmath>
#include "CLHEP/Random/RandExponential.h"
#include "CLHEP/Random/RandFlat.h"

#include "RichDet/Rich.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"
#include "RichRec/ActivePixel.h"
#include "RichRec/Photon.h"
#include "RichRec/Trajectory.h"
#include "RichRec/TrackSegment.h"
#include "RichDet/Rich1AerogelRadiator.h"

Rich1AerogelRadiator::Rich1AerogelRadiator (const Rich &rich)
  : PhotonRadiator(Aerogel,rich), m_parGel(5), m_avgRefIndex(0.)
{
}

Rich1AerogelRadiator::~Rich1AerogelRadiator()
{
}

void Rich1AerogelRadiator::update (const RichParameters &parameter)
{
  //m_z0Pos        = parameter.getValue("Rich1.ARad.Z0Pos",    105.0    );
  //m_width       = parameter.getValue("Rich1.ARad.Width",      5.0    );
  // m_x0Max        = parameter.getValue("Rich1.ARad.X0Max",     36.0    );
  //m_y0Max        = parameter.getValue("Rich1.ARad.Y0Max",     36.0    );
  //m_x0Min        = parameter.getValue("Rich1.ARad.X0Min",      5.2    );
  //m_y0Min        = parameter.getValue("Rich1.ARad.Y0Min",      5.2    );

  m_refIndex     = parameter.getValue("Rich1.ARad.RefIndex",   1.03   );

  m_width        = parameter.getValue("CDF.RIC1.TAERO1", 5.0);
  m_z0Pos        = parameter.getValue("CDF.RIC1.ZSGAS1", 105.0) - m_width;
  m_x0Min        = parameter.getValue("CDF.RIC1.DRAERO1",      5.2   );
  m_x0Max        = parameter.getValue("CDF.RIC1.XAERO1",     36.0    );
  m_y0Min        = parameter.getValue("CDF.RIC1.DRAERO1",      5.2    );
  m_y0Max        = parameter.getValue("CDF.RIC1.YAERO1",     36.0    );
  m_clarity     = parameter.getValue("Rich1.ARad.Clarity",    0.008  );

  m_parGel[0]   = parameter.getValue("Rich1.ARad.ParGel0",    2.1467 );
  m_parGel[1]   = parameter.getValue("Rich1.ARad.ParGel1",   46.41   );
  m_parGel[2]   = parameter.getValue("Rich1.ARad.ParGel2",  113.8    );
  m_parGel[3]   = parameter.getValue("Rich1.ARad.ParGel3",  228.7    );
  m_parGel[4]   = parameter.getValue("Rich1.ARad.ParGel4",  328.5    );

  m_energyCutoff = parameter.getValue("Rich1.ARad.Cutoff",   3.5    );
  m_plane0 = HepPlane3D(0.,0.,1.,-m_z0Pos);
  m_plane1 = HepPlane3D(0.,0.,1.,-m_z0Pos - m_width);

  m_effRefIndex   = parameter.getValue("Rich1.ARad.EffRefIndex",1.03);

  m_emissionError       = parameter.getValue("Rich1.ARad.EmissionError", true);
  m_chromaticError      = parameter.getValue("Rich1.ARad.ChromaticError",
                                             true);
  m_rayleighScattering = parameter.getValue("Rich1.ARad.RayleighScattering",
                                            true );
  m_refraction         = parameter.getValue("Rich1.ARad.Refraction",true);
  m_efficiencyPrecision = parameter.getValue("Rich1.ARad.EfficiencyPrecision",
                                             0.01);

  m_sigmaPhotonMin      = parameter.getValue("Rich1.ARad.SigmaPhotonMin",
                                             0.00036);
  m_sigmaPhotonMax      = parameter.getValue("Rich1.ARad.SigmaPhotonMin",
                                             0.00126);

  m_mirrorRadius        = parameter.getValue("CDF.RIC1.RCMIR1",190.);

  // Delete cached results that could be invalid after an update
  delete m_photonEfficiency;
  m_photonEfficiency = NULL;
  m_avgRefIndex = 0.;

  // Re-initialise
  this->initPhotonResolution();

}

double Rich1AerogelRadiator::refractiveIndex (const double energy) const
{

  if ( m_chromaticError ) {
    return  1. + ( m_refIndex -  1. ) * m_parGel[0] *
      ( sqrt( m_parGel[1] / ( m_parGel[2] - energy*energy ) +
              m_parGel[3] / ( m_parGel[4] - energy*energy ) + 1. ) - 1. );
  } else {
    return m_refIndex;
  };

}

double Rich1AerogelRadiator::refractiveIndex () const
{
  return m_effRefIndex;
}

bool Rich1AerogelRadiator::leaves (const Trajectory &trajectory,
                                   double &distance) const
{

  if ( !trajectory.intersect(m_plane1,distance) ) {
    return false;
  }
  return trajectory.direction(distance).z() > 0;

}

Photon Rich1AerogelRadiator::generatePhoton (const ParticleCode code,
                                             TrackSegment &segment) const
{
  Photon photon = this->radiatePhoton(code,segment);

  if ( photon.status() == Photon::Emitted ) {
    scatter(photon);
  };

  if ( photon.status() == Photon::Emitted ||
       photon.status() == Photon::Scattered ) {
    rich().reflector().reflect(photon);
  };

  if ( photon.status() == Photon::Reflected ||
       photon.status() == Photon::ScatteredReflected ) {
    rich().detector().detect(photon);
  };

  return photon;
}

Photon Rich1AerogelRadiator::generatePhoton (const double thetaCherenkov,
                                             const double phiCherenkov,
                                             const double distCherenkov,
                                             TrackSegment &segment) const
{
  Photon photon = Photon::emitted(0.,thetaCherenkov,phiCherenkov,
                                  distCherenkov,segment);

  if ( photon.status() == Photon::Emitted ) {
    scatter(photon);
  };
  if ( photon.status() == Photon::Emitted ||
       photon.status() == Photon::Scattered ) {
    rich().reflector().reflect(photon);
  }

  if ( photon.status() == Photon::Reflected ||
       photon.status() == Photon::ScatteredReflected ) {
    rich().detector().detect(photon);
  }

  return photon;
}

void Rich1AerogelRadiator::scatter (Photon &photon) const
{

  //    cout << "Photon Emitted :" << photon.thetaCherenkov() << endl;


  double dist;

  if ( m_rayleighScattering ) {

    double scatterLength = pow(1.24/photon.energy(),4) / m_clarity;

    int scatter = 0;
    for(;;) {

      if ( photon.direction().z() > 0. ) {
        if ( ! photon.intersect(m_plane1,dist) || dist < 0.  ) {
          //     cout << "Photon absorbed 1" << endl;
          photon.absorbed(true);
          return;
        }
      } else {
        if ( ! photon.intersect(m_plane0,dist) || dist < 0.) {
          //     cout << "Photon absorbed 2" << endl;
          photon.absorbed(true);
          return;
        }
      }
      HepPoint3D  pos = photon.position(dist);
      if ( abs(pos.x()) > m_x0Max || abs(pos.y()) > m_y0Max ) {
        //   cout << "Photon absorbed 3 : z = "
        //        << photon.position().z() << endl;
        photon.absorbed(true);
        return;
      }


      double length =  RandExponential::shoot(scatterLength);

      if ( length > dist ) break;

      double theta = randCos2();
      double phi   = RandFlat::shoot(2.*M_PI);

      // HepVector3D dir = photon.direction(length);
      // if ( abs(dir.x()) < abs(dir.y()) ) {
      HepVector3D  dir =
        photon.rotateDirection(length,theta,phi,HepVector3D(1.,0.,0.));
      // } else {
      //  dir = photon.rotateDirection(length,theta,phi,HepVector3D(0.,1.,0.));
      // }
      photon.scattered(pos,dir);

      // Emergency Exit
      if ( ++scatter < 10 ) {
        photon.absorbed(true);
        return;
      }

    }
  }

  // photon has to go upstream

  if ( ! leaves(photon,dist) ) {
    //      cout << "Photon absorbed 4 : z = " << photon.position().z() << endl;
    photon.absorbed(true);
    return;
  }

  //    cout << "Photon After :" <<
  //    photon.direction().angle(photon.segment().direction()) << endl;

  // finally refraction on exiting aerogel

  if ( m_refraction ) {

    HepPoint3D  pos = photon.position(dist);
    HepVector3D dir = photon.direction(dist);

    // snells law, it is assumed that the refraction plane is ortogonal
    // to the z axis

    double sinTheta =
      this->refractiveIndex(photon.energy()) * sin(dir.theta());

    if ( sinTheta > 1. ) {
      // cout << "Total reflection : z = " << photon.position().z() << endl;
      photon.absorbed(true);
      return;
    }

    dir.setTheta( asin(sinTheta) );

    //      cout << " Refracted : z = " << photon.position().z() << endl;
    photon.refracted(pos,dir);

  }

}

double Rich1AerogelRadiator::randCos2 ()
{
  double theta, cosTheta;
  do {
    theta = RandFlat::shoot(M_PI);
    cosTheta = cos(theta);
  } while ( RandFlat::shoot(1.) > cosTheta * cosTheta );

  return theta;
}

double Rich1AerogelRadiator::scatterFraction (const ParticleCode particle,
                                              const TrackSegment &segment,
                                              const double theta,
                                              const double area) const
{
  assert( segment.avgThetaCherenkov(particle) > 0. );

  double par[] = { 31.2301, -0.000175438, 0.132727, 0.570114, -2.34341,
                   2.18153, 1.4248 };

  double fbkg;
  if ( theta < 0.25 ) {
    fbkg = par[0] * theta * theta + par[1]*theta + par[2];
  } else if ( theta < par[6] ) {
    fbkg = par[3] * theta * theta + par[4]*theta + par[5];
  } else {
    fbkg = 0.;
  }
  fbkg = fbkg / (2. * M_PI);
  return fbkg * 4. * area / ( m_mirrorRadius * m_mirrorRadius * theta);

}
