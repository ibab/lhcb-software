// $Id: RichGeomEffPhotonTracing.h,v 1.2 2003-12-11 16:33:36 cattanem Exp $
#ifndef RICHRECTOOLS_RICHGEOMEFFPHOTONTRACING_H
#define RICHRECTOOLS_RICHGEOMEFFPHOTONTRACING_H 1

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichDetTools/IRichDetInterface.h"

/** @class RichGeomEffPhotonTracing RichGeomEffPhotonTracing.h
 *
 *  Tool to perform a full detailed calculation of the geometrical
 *  efficiency for a given RichRecSegment and mass hypothesis.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichGeomEffPhotonTracing : public RichRecToolBase,
                                 virtual public IRichGeomEff {

public:

  /// Standard constructor
  RichGeomEffPhotonTracing( const std::string& type,
                            const std::string& name,
                            const IInterface* parent );

  /// Destructor
  virtual ~RichGeomEffPhotonTracing() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Obtain geometrical efficiency for this track and hypothesis
  double geomEfficiency ( RichRecSegment * segment,
                          const Rich::ParticleIDType id );

  /// Obtain scattered geometrical efficiency for this track and hypothesis
  double geomEfficiencyScat ( RichRecSegment * segment,
                              const Rich::ParticleIDType id );

private: // Private data

  // Pointers to tool instances
  IRichDetInterface * m_richDetInt;
  IRichCherenkovAngle * m_ckAngle;

  /// Number of photons to use in geometrical efficiency calculation
  int m_nGeomEff;

  /// Number of photons to quit after in geometrical efficiency calculation
  /// if all so far have failed
  int m_nGeomEffBailout;

  /// Increment parameter for PD efficiencies
  double m_pdInc;

  /// Cherenkov phi incrementation
  double m_incPhi;

  /// Cache Sin(angle) for geometrical efficiency calculation
  std::vector<double> m_sinCkPhi;
  /// Cache Cos(angle) for geometrical efficiency calculation
  std::vector<double> m_cosCkPhi;

  /// Flat Randomn distribution between 0 and 1
  Rndm::Numbers m_uniDist;

};

#endif // RICHRECTOOLS_RICHGEOMEFFPHOTONTRACING_H
