// $Id: IRichRecSegmentTool.h,v 1.2 2002-11-14 13:54:21 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_IRICHRECSEGMENTTOOL_H 1

// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// RichKernel
#include "RichKernel/RichDefinitions.h"

// Event
#include "Event/RichRecSegment.h"

/** @class RichRecSegmentTool RichRecSegmentTool.h
 *
 *  Interface for tool which performs the association between RichRecTracks
 *  and RichRecPixels to form RichRecSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecSegmentTool( "IRichRecSegmentTool", 1, 0 );

class IRichRecSegmentTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() {return IID_IRichRecSegmentTool;}

  /// Save a new RichRecSegment in the container
  virtual void saveSegment( RichRecSegment * segment ) = 0;

  /// Return Pointer to RichRecSegments
  virtual RichRecSegments * richSegments() = 0;

  /// Obtain geometrical efficiency for this track and hypothesis
  virtual double geomEfficiency ( RichRecSegment * segment,
                                  const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal photons for given segment and hypothesis
  virtual double signalPhotons ( RichRecSegment * segment,
                                 const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable scattered photons for given segment and hypothesis
  virtual double scatteredPhotons ( RichRecSegment * segment,
                                    const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  virtual double allPhotons ( RichRecSegment * segment, 
                              const Rich::ParticleIDType& id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis
  virtual double emittedPhotons ( RichRecSegment * segment,
                                  const Rich::ParticleIDType& id ) = 0;

  /// Returns average Cherenkov angle for given particle hypothesis
  virtual double avgCherenkovTheta( RichRecSegment * segment,
                                    const Rich::ParticleIDType& id ) = 0;

  /// Returns 'beta' for given particle hypothesis
  virtual double beta( RichRecSegment * segment,
                       const Rich::ParticleIDType& id ) = 0;

  /// Is it geometrically possible for this segment to give Rich information
  virtual bool hasRichInfo( RichRecSegment * segment ) = 0;

  /// Is this segment above threshold for a given particle hypothesis
  virtual bool aboveThreshold( RichRecSegment * segment,
                               Rich::ParticleIDType & type ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
