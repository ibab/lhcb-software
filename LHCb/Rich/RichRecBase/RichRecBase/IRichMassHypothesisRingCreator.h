
//-----------------------------------------------------------------------------
/** @file IRichMassHypothesisRingCreator.h
 *
 *  Header file for RICH reconstruction tool interface : IRichMassHypothesisRingCreator
 *
 *  CVS Log :-
 *  $Id: IRichMassHypothesisRingCreator.h,v 1.5 2006-03-12 16:13:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H
#define RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event model
#include "Event/RichRecRing.h"
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichMassHypothesisRingCreator( "IRichMassHypothesisRingCreator", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichMassHypothesisRingCreator IRichMassHypothesisRingCreator.h
 *
 *  Interface for tool to create RichRecRings that represent the rings a given
 *  segment would produce under a given mass hypothesis
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2004
 */
//-----------------------------------------------------------------------------

class IRichMassHypothesisRingCreator : public virtual IAlgTool 
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichMassHypothesisRingCreator; }

  /** Creates the Cherenkov ring for a given segment and mass hypothesis
   *
   *  @param segment Pointer to a RichRecSegment
   *  @param id      Mass hypothesis
   *
   *  @return Pointer to the mass hypothesis ring
   */
  virtual LHCb::RichRecRing * newMassHypoRing( LHCb::RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const = 0;

  /** Creates and returns a new default RichRecRing object.
   *
   *  @attention It is the reponsibility of the user to save or delete the ring,
   *             otherwise there will be a memory leak.
   *
   *  @return Pointer to the new RichrecRing
   */
  virtual LHCb::RichRecRing * newMassHypoRing() const = 0;

  /** Save the RichRecRing in the tool internal container
   *
   *  @attention Once this method is called on a ring, the user should
   *             NOT delete the ring
   *
   *  @param ring Pointer to the RichRecRing to save
   */
  virtual void saveMassHypoRing( LHCb::RichRecRing * ring ) const = 0;

  /** Builds the mass hypothesis rings for all mass hypotheses for given RichRecSegment
   *
   *  @param segment Pointer to a RichRecSegment
   */
  virtual void newMassHypoRings( LHCb::RichRecSegment * segment ) const = 0;

  /** Access to all mass hypothesis rings currently created
   *
   *  @return Pointer to the contianer of all RichRecRings
   */
  virtual LHCb::RichRecRings * massHypoRings() const = 0;

};

#endif // RICHRECBASE_IRICHMASSHYPOTHESISRINGCREATOR_H
