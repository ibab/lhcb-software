
//-----------------------------------------------------------------------------
/** @file IRichParticleProperties.h
 *
 *  Header file for RICH reconstruction tool interface : IRichParticleProperties
 *
 *  CVS Log :-
 *  $Id: IRichParticleProperties.h,v 1.5 2004-10-13 09:29:43 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/07/26 18:00:57  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H
#define RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H 1

// event model
class RichRecSegment;

// Kernel
#include "RichKernel/RichParticleIDType.h"
#include "Kernel/RichRadiatorType.h"

/// Static Interface Identification
static const InterfaceID IID_IRichParticleProperties( "IRichParticleProperties", 1, 0 );

/** @class IRichParticleProperties IRichParticleProperties.h
 *
 *  Interface for tool to calculate various physical properties
 *  for the different mass hypotheses.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichParticleProperties : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichParticleProperties;}

  /** Calculates 'beta' for given particle hypothesis
   *
   *  @param segment Pointer to the RichRecSegment for which to calculate beta
   *  @para  id      The mass hypothesis to assume whilst calculating beta
   *
   *  @return The value of beta
   */
  virtual double beta( RichRecSegment * segment,
                       const Rich::ParticleIDType id ) const = 0;

  /** Calculates the nominal mass for a given particle type
   *
   *  @para  id  The mass hypothesis to assume whilst calculating beta
   *
   *  @return the nominal mass for the given particle hypothesis
   */
  virtual double mass( const Rich::ParticleIDType id ) const = 0;

  /** calculates the nominal mass squared for a given particle hypothesis
   *
   *  @para  id  The mass hypothesis to assume whilst calculating beta
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
   *          cal be emitted
   */
  virtual double thresholdMomentum( const Rich::ParticleIDType id,
                                    const Rich::RadiatorType rad ) const = 0;

};

#endif // RICHRECTOOLS_IRICHPARTICLEPROPERTIES_H
