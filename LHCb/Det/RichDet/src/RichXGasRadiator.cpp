#include <cmath>

#include "RichDet/RichXGasRadiator.h"

#include "RichDet/Trajectory.h"

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
  return trajectory.direction(/*distance*/).z() > 0.;

}

double RichXGasRadiator::scatterFraction (const double theta,
                                          const double area) const
{
  return 0.;
}
