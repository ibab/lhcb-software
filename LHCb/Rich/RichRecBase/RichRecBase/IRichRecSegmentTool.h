// $Id: IRichRecSegmentTool.h,v 1.1.1.1 2003-04-01 13:21:56 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
#define RICHRECTOOLS_IRICHRECSEGMENTTOOL_H 1

// GaudiKernel
//#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/SmartRef.h"
//#include "GaudiKernel/SmartRefVector.h"

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

  /// Obtain scattered geometrical efficiency for this track and hypothesis
  virtual double geomEfficiencyScat ( RichRecSegment * segment,
                                      const Rich::ParticleIDType& id ) = 0;

  /// Expected number of signal photons for given segment and hypothesis
  virtual double nSignalPhotons ( RichRecSegment * segment,
                                  const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable signal photons for given segment and hypothesis
  virtual double nObservableSignalPhotons ( RichRecSegment * segment,
                                            const Rich::ParticleIDType& id ) = 0;

  /// Expected number of scattered photons for given segment and hypothesis
  virtual double nScatteredPhotons ( RichRecSegment * segment,
                                     const Rich::ParticleIDType& id ) = 0;

  /// Expected number of observable scattered photons for given segment and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecSegment * segment,
                                               const Rich::ParticleIDType & id ) = 0;

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  virtual double nTotalObservablePhotons ( RichRecSegment * segment,
                                           const Rich::ParticleIDType& id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis
  virtual double nEmittedPhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType& id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis,
  /// scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecSegment * segment,
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
                               const Rich::ParticleIDType & type ) = 0;

  /// Average refractive index for this segments radiator
  virtual double refractiveIndex( RichRecSegment * segment ) = 0;

  /// Photon resolution
  virtual double ckThetaResolution( RichRecSegment * segment,
                                    const Rich::ParticleIDType& id ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECSEGMENTTOOL_H
