// $Id: IRichRecRingCreator.h,v 1.1 2004-05-30 16:50:34 jonrob Exp $
#ifndef RICHRECBASE_IRICHRECRINGCREATOR_H
#define RICHRECBASE_IRICHRECRINGCREATOR_H 1

// Event model
class RichRecRing;

/** @class IRichRecRingCreator IRichRecRingCreator.h
 *
 *  Interface for tool to create and book keep RichRecRing objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecRingCreator( "IRichRecRingCreator", 1, 0 );

class IRichRecRingCreator : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichRecRingCreator; }

  /// Returns a RichRecRing object pointer for given RichRecSegment and mass hypothesis
  virtual RichRecRing * newRing( RichRecSegment * segment,
                                 const Rich::ParticleIDType id,
                                 const RichRecRing::RingType type = RichRecRing::RayTracedPhotons ) const = 0;

  /// Builds Richrecrings for all mass hypotheses for given RichRecSegment
  virtual void newRings( RichRecSegment * segment,
                         const RichRecRing::RingType type = RichRecRing::RayTracedPhotons ) const = 0;

  /// Returns a pointer to the RichRecRings
  virtual RichRecRings * richRings() const = 0;

};

#endif // RICHRECBASE_IRICHRECRINGCREATOR_H
