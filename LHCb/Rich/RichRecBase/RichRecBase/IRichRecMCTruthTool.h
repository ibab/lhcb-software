// $Id: IRichRecMCTruthTool.h,v 1.3 2003-07-02 15:37:27 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H 1

// Event Model
class MCRichOpticalPhoton;
class MCRichDigit;
class MCParticle;

/** @class IRichRecMCTruthTool IRichRecMCTruthTool.h
 *
 *  Interface for tool performing MC truth associations between Rich event objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *  @todo Review whether this utility should be reimplemented as associators.
 */

static const InterfaceID IID_IRichRecMCTruthTool( "IRichRecMCTruthTool", 1, 0 );

class IRichRecMCTruthTool : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichRecMCTruthTool; }

  /// Find best MCParticle association for a given RichRecTrack
  virtual const MCParticle * mcParticle( const RichRecTrack * richTrack ) = 0;

  /// Find best MCParticle association for a given RichRecSegment
  virtual const MCParticle * mcParticle( const RichRecSegment * richSegment ) = 0;

  /// Truth particle type for given RichRecTrack
  virtual Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack ) = 0;

  /// Truth particle type for given RichRecSegment
  virtual Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment ) = 0;

  /// Truth particle type for given MCParticle
  virtual Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) = 0;

  /// Find parent MCParticle association for a given RichRecPixel
  virtual const MCParticle * mcParticle( const RichRecPixel * richPixel ) = 0;

  /// Find parent MCRichDigit association for a given RichRecPixel
  virtual const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel ) =0;

  /// Find parent MCRichOpticalPhoton association for a given RichRecPixel
  virtual const MCRichOpticalPhoton * mcRichOpticalPhoton( const RichRecPixel * richPixel ) = 0;

  /// Is this a true photon candidate ?
  /// Do the associated track and pixel have the same MC parent
  virtual const MCParticle * trueRecPhoton( const RichRecPhoton * photon ) = 0;

  /// Is this a true Cherenkov photon candidate ?
  /// Do the associated track and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  virtual const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon ) = 0;

  /// Is the hit due to Cherenkov radiation for given radiator medium ?
  virtual const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel, 
                                                     Rich::RadiatorType rad  ) = 0;

  /// Returns a vector of true Cherenkov pixels for this segment
  virtual RichRecPixelVector* trueCkPixels( const RichRecSegment * segment ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
