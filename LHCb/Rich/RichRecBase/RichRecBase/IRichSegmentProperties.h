// $Id: IRichSegmentProperties.h,v 1.1 2003-06-30 15:11:56 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHSEGMENTPROPERTIES_H
#define RICHRECTOOLS_IRICHSEGMENTPROPERTIES_H 1

// Event model
class RichRecSegment;
class RichPID;

/** @class IRichSegmentProperties IRichSegmentProperties.h
 *
 *  Interface for tool performing useful calculations and manipulations evolving
 *  RichRecSegments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *  @todo Review this class and move methods to more logical and sef-contained tools
 */

static const InterfaceID IID_IRichSegmentProperties( "IRichSegmentProperties", 1, 0 );

class IRichSegmentProperties : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() {return IID_IRichSegmentProperties;}

  /// Expected number of signal photons for given segment and hypothesis
  virtual double nSignalPhotons ( RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal photons for given segment and hypothesis
  virtual double nObservableSignalPhotons ( RichRecSegment * segment,
                                            const Rich::ParticleIDType id ) = 0;

  /// Expected number of scattered photons for given segment and hypothesis
  virtual double nScatteredPhotons ( RichRecSegment * segment,
                                     const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable scattered photons for given segment and hypothesis
  virtual double nObservableScatteredPhotons ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) = 0;

  /// Expected number of observable signal+scattered photons for given segment and hypothesis
  virtual double nTotalObservablePhotons ( RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis
  virtual double nEmittedPhotons ( RichRecSegment * segment,
                                   const Rich::ParticleIDType id ) = 0;

  /// Expected number of emitted photons for given segment and hypothesis, scaled by the HPD quantum efficiency
  virtual double nDetectablePhotons ( RichRecSegment * segment,
                                      const Rich::ParticleIDType id ) = 0;

  /// Returns average Cherenkov angle for given particle hypothesis
  virtual double avgCherenkovTheta( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) = 0;

  /// Returns 'beta' for given particle hypothesis
  virtual double beta( RichRecSegment * segment,
                       const Rich::ParticleIDType id ) = 0;

  /// Is it geometrically possible for this segment to give Rich information
  virtual bool hasRichInfo( RichRecSegment * segment ) = 0;

  /// Is this segment above threshold for a given particle hypothesis
  virtual bool aboveThreshold( RichRecSegment * segment,
                               const Rich::ParticleIDType type ) = 0;

  /// Average refractive index for this segments radiator
  virtual double refractiveIndex( RichRecSegment * segment ) = 0;

  /// Photon resolution
  virtual double ckThetaResolution( RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) = 0;

  /// Set the threshold information in a RichPID object for given segment
  virtual void setThresholdInfo( RichRecSegment * segment,
                                 RichPID * pid ) = 0;

};

#endif // RICHRECTOOLS_IRICHSEGMENTPROPERTIES_H
