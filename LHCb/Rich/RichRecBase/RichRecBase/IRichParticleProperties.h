// $Id: IRichParticleProperties.h,v 1.2 2004-02-02 14:23:02 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H
#define RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H 1

// event model
class RichRecSegment;

// RichKernel
#include "RichKernel/RichParticleIDType.h"
#include "RichKernel/RichRadiatorType.h"

/** @class IRichParticleProperties IRichParticleProperties.h
 *
 *  Interface for tool to calculate various physical properties 
 *  for the different mass hypotheses.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichParticleProperties( "IRichParticleProperties", 1, 0 );

class IRichParticleProperties : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichParticleProperties;}

  /// Returns 'beta' for given particle hypothesis
  virtual double beta( RichRecSegment * segment,
                       const Rich::ParticleIDType id ) const = 0; 

  /// Returns the nominal mass for a given particle type
  virtual double mass( const Rich::ParticleIDType id ) const = 0;

  /// Returns the nominal mass squared for a given particle hypothesis
  virtual double massSq( const Rich::ParticleIDType id ) const = 0;

  /// Returns the threshold momentum for a given hypothesis in a given radiator
  virtual double thresholdMomentum( const Rich::ParticleIDType id,
                                    const Rich::RadiatorType rad ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H
