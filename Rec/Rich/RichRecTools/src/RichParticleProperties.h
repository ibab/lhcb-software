// $Id: RichParticleProperties.h,v 1.4 2003-12-11 16:33:36 cattanem Exp $
#ifndef RICHRECTOOLS_RICHPARTICLEPROPERTIES_H
#define RICHRECTOOLS_RICHPARTICLEPROPERTIES_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/BoostArray.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichParticleProperties.h"
#include "RichDetTools/IRichRefractiveIndex.h"

/** @class RichParticleProperties RichParticleProperties.h
 *
 *  Tool to calculate various physical properties 
 *  for the different mass hypotheses.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichParticleProperties : public RichRecToolBase,
                               virtual public IRichParticleProperties {

public:

  /// Standard constructor
  RichParticleProperties( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

  /// Destructor
  virtual ~RichParticleProperties() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Returns 'beta' for given particle hypothesis
  double beta( RichRecSegment * segment, const Rich::ParticleIDType id );

  /// Returns the nominal mass for a given particle type
  double mass( const Rich::ParticleIDType id );

  /// Returns the nominal mass squared for a given particle type
  double massSq( const Rich::ParticleIDType id );

  /// Returns the threshold momentum for a given hypothesis in a given radiator
  double thresholdMomentum( const Rich::ParticleIDType id,
                            const Rich::RadiatorType rad );
  
private:  // Private data

  /// Pointers to tool instances
  IRichRefractiveIndex * m_refIndex;

  /// Array containing particle masses
  boost::array<double,Rich::NParticleTypes> m_particleMass;

  /// Array containing square of particle masses
  boost::array<double,Rich::NParticleTypes> m_particleMassSq;

  /// Momentum thresholds
  double m_momThres[Rich::NRadiatorTypes][Rich::NParticleTypes];

};

#endif // RICHRECTOOLS_RICHPARTICLEPROPERTIES_H
