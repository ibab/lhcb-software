// $Id: IRichRecMCTruthTool.h,v 1.7 2004-03-16 13:39:57 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H 1

// Event Model
class MCRichOpticalPhoton;
class MCRichSegment;
class MCRichTrack;
class MCRichDigit;
class MCParticle;
class MCRichHit;
class RichDigit;

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
  virtual const MCParticle * mcParticle( const RichRecTrack * richTrack ) const = 0;

  /// Find best MCParticle association for a given TrStoredTrack
  virtual const MCParticle * mcParticle( const TrStoredTrack * track ) const = 0;

  /// Find best MCParticle association for a given RichRecSegment
  virtual const MCParticle * mcParticle( const RichRecSegment * richSegment ) const = 0;

  /// Truth particle type for given RichRecTrack
  virtual Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack ) = 0;

  /// Truth particle type for given RichRecSegment
  virtual Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment ) = 0;

  /// Truth particle type for given MCParticle
  virtual Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) = 0;

  /// Find parent MCParticles associated to a given RichRecPixel
  virtual bool mcParticle( const RichRecPixel * richPixel,
                           std::vector<const MCParticle*> & mcParts ) = 0;
  
  /// Find parent MCRichDigit association for a given RichRecPixel
  virtual const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel ) = 0;

  /// Find parent MCRichDigit association for a given RichDigit
  virtual const MCRichDigit * mcRichDigit( const RichDigit * digit ) = 0;

  /// Find parent MCRichHits for a given RichRecPixel
  virtual const SmartRefVector<MCRichHit> & mcRichHits( const RichRecPixel * richPixel ) = 0;

  /// Finds parent MCRichHits for given MCRichDigit
  virtual const SmartRefVector<MCRichHit> & mcRichHits( const MCRichDigit * mcDigit ) = 0;

  /// Find parent MCRichOpticalPhotons associated to a given RichRecPixel
  virtual bool mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                    SmartRefVector<MCRichOpticalPhoton> & phots ) = 0;

  /// Is this a true photon candidate ?
  /// Do the associated track and pixel have the same MC parent
  virtual const MCParticle * trueRecPhoton( const RichRecPhoton * photon ) = 0;

  /// Returns the associated MCRichHit if given RichRecPhoton is true (null otherwise)
  virtual const MCRichHit * trueCherenkovHit( const RichRecPhoton * photon ) = 0;

  /// Returns the associated MCRichOpticalPhoton if given RichRecPhoton is true (null otherwise)
  virtual const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecPhoton * photon ) = 0;

  /// Is this a true photon candidate ?
  /// Do the segment and pixel have the same MC parent
  virtual const MCParticle * trueRecPhoton( const RichRecSegment * segment,
                                            const RichRecPixel * pixel ) = 0;

  /// Is this a true Cherenkov photon candidate ?
  /// Do the associated segment and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  virtual const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon ) = 0;

  /// Is this a true Cherenkov photon candidate ?
  /// Do the segment and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  virtual const MCParticle * trueCherenkovPhoton( const RichRecSegment * segment,
                                                  const RichRecPixel * pixel ) = 0;

  /// Is the hit due to Cherenkov radiation for given radiator medium ?
  virtual const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel,
                                                     Rich::RadiatorType rad  ) = 0;

  /// Returns the MCRichSegment associated to a given RichRecSegment
  virtual const MCRichSegment * mcRichSegment( const RichRecSegment * segment ) = 0;

  /// Returns the MCRichTrack associated to a given RichRecTrack
  virtual const MCRichTrack * mcRichTrack( const RichRecTrack * track ) = 0;

  /// Returns the MCRichTrack associated to a given TrStoredTrack
  virtual const MCRichTrack * mcRichTrack( const TrStoredTrack * track ) = 0;

  /// Returns the MCRichTrack associated to a given RichRecSegment
  virtual const MCRichTrack * mcRichTrack( const RichRecSegment * segment ) = 0;

  /// Returns the MCRichTrack associated to a given MCParticle
  virtual const MCRichTrack * mcRichTrack( const MCParticle * mcPart ) = 0;

  /// Returns pointer to vector of MCRichSegments associated to a given RichRecTrack
  virtual const SmartRefVector<MCRichSegment> * mcRichSegments( const RichRecTrack * track ) = 0;

  /// Is this RichRecPixel background ?
  virtual bool isBackground( const RichRecPixel * pixel ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
