// $Id: IRichMassHypothesisRingCreator.h,v 1.1 2004-05-31 21:34:03 jonrob Exp $
#ifndef RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H
#define RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H 1

// Event model
#include "Event/RichRecRing.h"
class RichRecSegment;

/** @class IRichMassHypothesisRingCreator IRichMassHypothesisRingCreator.h
 *
 *  Interface for tool to create RichRecRings that represent the rings a given
 *  segment would produce under a given mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichMassHypothesisRingCreator( "IRichMassHypothesisRingCreator", 1, 0 );

class IRichMassHypothesisRingCreator : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichMassHypothesisRingCreator; }

  /// Returns the mas hypothesis ring for a given segment and mass hypothesis
  virtual RichRecRing * newMassHypoRing( RichRecSegment * segment,
                                         const Rich::ParticleIDType id ) const = 0;

  /// Builds the mass hypothesis rings for all mass hypotheses for given RichRecSegment
  virtual void newMassHypoRings( RichRecSegment * segment ) const = 0;

  /// Returns a pointer to all mass hypothesis rings
  virtual RichRecRings * massHypoRings() const = 0;

};

#endif // RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H
