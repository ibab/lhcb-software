
//-----------------------------------------------------------------------------
/** @file IRichParticleProperties.h
 *
 *  Header file for RICH reconstruction tool interface : Rich::IParticleProperties
 *
 *  $Id: IRichParticleProperties.h,v 1.5 2007-03-09 17:58:13 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHPARTICLEPROPERTIES_H
#define RICHKERNEL_IRICHPARTICLEPROPERTIES_H 1

// Kernel
#include "Kernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichParticleProperties( "Rich::IParticleProperties", 1, 0 );

namespace Rich
{

  /** @class IParticleProperties IRichParticleProperties.h
   *
   *  Interface for tool to calculate various physical properties
   *  for the different mass hypotheses.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   15/03/2002
   */

  class IParticleProperties : public virtual IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() {return IID_IRichParticleProperties;}

    /** Calculates 'beta' for given particle hypothesis and track momentum
     *
     *  @param ptot    Track segment momentum
     *  @param id      The mass hypothesis to assume whilst calculating beta
     *
     *  @return The value of beta
     */
    virtual double beta( const double ptot,
                         const Rich::ParticleIDType id ) const = 0;

    /** Calculates the nominal mass for a given particle type
     *
     *  @param id  The mass hypothesis to assume whilst calculating beta
     *
     *  @return the nominal mass for the given particle hypothesis
     */
    virtual double mass( const Rich::ParticleIDType id ) const = 0;

    /** calculates the nominal mass squared for a given particle hypothesis
     *
     *  @param id  The mass hypothesis to assume whilst calculating beta
     *
     *  @return the nominal mass squared for the given particle hypothesis
     */
    virtual double massSq( const Rich::ParticleIDType id ) const = 0;

    /** Calculates the threshold momentum for a given mass hypothesis
     *  in a given radiator
     *
     *  @param id The mass hypothesis
     *  @param rad The radiator medium
     *
     *  @return The momentum threshold value, below which no Cherenkov radiation
     *          can be emitted
     */
    virtual double thresholdMomentum( const Rich::ParticleIDType id,
                                      const Rich::RadiatorType rad ) const = 0;

    /** Calculates the square of the threshold momentum for a given mass hypothesis
     *  in a given radiator
     *
     *  @param id The mass hypothesis
     *  @param rad The radiator medium
     *
     *  @return The momentum threshold squared, below which no Cherenkov radiation
     *          can be emitted
     */
    virtual double thresholdMomentumSq( const Rich::ParticleIDType id,
                                        const Rich::RadiatorType rad ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHPARTICLEPROPERTIES_H
