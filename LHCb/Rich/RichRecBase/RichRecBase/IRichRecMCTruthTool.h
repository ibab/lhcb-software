
//-----------------------------------------------------------------------------
/** @file IRichRecMCTruthTool.h
 *
 *  Header file for RICH reconstruction tool interface : IRichRecMCTruthTool
 *
 *  CVS Log :-
 *  $Id: IRichRecMCTruthTool.h,v 1.10 2004-07-26 18:00:58 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

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

/// Static Interface Identification
static const InterfaceID IID_IRichRecMCTruthTool( "IRichRecMCTruthTool", 1, 0 );

/** @class IRichRecMCTruthTool IRichRecMCTruthTool.h
 *
 *  Interface for tool performing MC truth associations between Rich
 *  reconstruction event objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichRecMCTruthTool : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecMCTruthTool; }

  /** Find the best MCParticle association for a given RichRecTrack
   *
   *  @param richTrack Pointer to the RichRecTrack
   *
   *  @return Pointer to associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * mcParticle( const RichRecTrack * richTrack ) const = 0;

  /** Find the best MCParticle association for a given RichRecSegment
   *
   *  @param richTrack Pointer to the RichRecSegment
   *
   *  @return Pointer to associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * mcParticle( const RichRecSegment * richSegment ) const = 0;

  /** Truth particle type for given RichRecTrack
   *
   *  @param richTrack Pointer to the RichRecTrack
   *
   *  @return True Particle type
   *  @retval Rich::Unknown if no Monte Carlo association was possible
   */
  virtual Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack ) const = 0;

  /** Truth particle type for given RichRecSegment
   *
   *  @param richTrack Pointer to the RichRecSegment
   *
   *  @return True Particle type
   *  @retval Rich::Unknown if no Monte Carlo association was possible
   */
  virtual Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment ) const = 0;

  /** Find the parent MCParticles associated to a given RichRecPixel
   *
   *  @param richPixel Pointer to a RichRecPixel
   *  @param mcParts Vector of asscoiated MCparticle pointers to be filled
   *
   *  @return status of the Monte Carloassociation
   *  @retval true  Monte Carlo association was successful
   *  @retval false No Monte Carlo association was possible
   */
  virtual bool mcParticle( const RichRecPixel * richPixel,
                           std::vector<const MCParticle*> & mcParts ) const = 0;

  /** Find the parent MCRichDigit association for a given RichRecPixel
   *
   *  @param richPixel Pointer to RichRecPixel
   *
   *  @return Pointer to associated MCRichDigit
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel ) const = 0;

  /** Find the parent MCRichHits for a given RichRecPixel
   *
   *  @param richPixel Pointer to a RichRecPixel
   *
   *  @return Reference to the SmartRefVector of associated MCRichHits
   */
  virtual const SmartRefVector<MCRichHit> & mcRichHits( const RichRecPixel * richPixel ) const = 0;

  /** Find the parent MCRichOpticalPhotons associated to a given RichRecPixel
   *
   *  @param richPixel Pointer to a RichRecPixel
   *  @param phots Reference to the SmartRefVector of associated MCRichOpticalPhotons
   *
   *  @return status of the Monte Carlo association
   *  @retval true  Monte Carlo association was successful
   *  @retval false No Monte Carlo association was possible
   */
  virtual bool mcRichOpticalPhoton( const RichRecPixel * richPixel,
                                    SmartRefVector<MCRichOpticalPhoton> & phots ) const = 0;

  /** Is this a true photon candidate ? Do the associated RichRecSegment and RichRecPixels
   *  used to form the given RichRecPhoton candidate have the same MCParticle parent.
   *
   *  @param photon Pointer to a RichRecPhoton
   *
   *  @return Pointer to an MCParticle
   *  @retval NULL No Monte Carlo association was possible,
   *          or the segment and pixel do not share the same parent
   *  @retval !NULL Pointer to the shared MCParticle parent
   */
  virtual const MCParticle * trueRecPhoton( const RichRecPhoton * photon ) const = 0;

  /** Access the associated MCRichHit if given RichRecPhoton is a true Cherenkov photon
   *
   *  @param photon Pointer to a RichRecPhoton
   *
   *  @return Pointer to associated MCRichHit
   *  @retval NULL No Monte Carlo association was possible,
   *          or the photon is not a true Cherenkov photon
   *  @retval !NULL Pointer to the associated MCRichHit
   */
  virtual const MCRichHit * trueCherenkovHit( const RichRecPhoton * photon ) const = 0;

  /** Access the associated MCRichOpticalPhoton if given RichRecPhoton is a true Cherenkov photon
   *
   *  @param photon Pointer to a RichRecPhoton
   *
   *  @return Pointer to associated MCRichOpticalPhoton
   *  @retval NULL No Monte Carlo association was possible,
   *          or the photon is not a true Cherenkov photon
   *  @retval !NULL Pointer to the associated MCRichOpticalPhoton
   */
  virtual const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecPhoton * photon ) const = 0;

  /** Access the associated MCRichOpticalPhoton if given RichRecSegment and RichRexPixel
   *  are both associated a single Cherenkov poton
   *
   *  @param segment Pointer to a RichRecSegment
   *  @param pixel   Pointer to a RichRecPixel
   *
   *  @return Pointer to associated MCRichOpticalPhoton
   *  @retval NULL No Monte Carlo association was possible,
   *          or the given segment and pixel are not associated to a single Cherenkov photon
   *  @retval !NULL Pointer to the associated MCRichOpticalPhoton
   */
  virtual const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecSegment * segment,
                                                         const RichRecPixel * pixel ) const = 0;

  /** Is this a true photon candidate ?
   *  Do the RichRecSegment and RichRecPixel have the same MCParticle parent
   *
   *  @param segment Pointer to a RichRecSegment
   *  @param pixel   Pointer to a RichRecPixel
   *
   *  @return Pointer to associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * trueRecPhoton( const RichRecSegment * segment,
                                            const RichRecPixel * pixel ) const = 0;

  /** Is this a true Cherenkov photon candidate ?
   *  Do the associated segment and pixel have the same MC parent AND was the pixel
   *  the result of Cherenkov radiation from the relevant radiator
   *
   *  @param photon Pointer to a RichRecPhoton
   *
   *  @return Pointer to associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible or photon is not a true
   *                Cherenkov photon
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon ) const = 0;

  /** Is this a true Cherenkov photon candidate ?
   *  Do the segment and pixel have the same MC parent AND was the pixel
   *  the result of Cherenkov radiation from the relevant radiator
   *
   *  @param segment Pointer to a RichRecSegment
   *  @param pixel   Pointer to a RichRecPixel
   *
   *  @return Pointer to associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible or photon is not a true
   *                Cherenkov photon
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * trueCherenkovPhoton( const RichRecSegment * segment,
                                                  const RichRecPixel * pixel ) const = 0;

  /** Is the RichRecPixel due to true MC Cherenkov radiation from given radiator medium ?
   *
   *  @param pixel   Pointer to a RichRecPixel
   *  @param rad     The radiator type
   *
   *  @return Pointer to the associated MCParticle
   *  @retval NULL  No Monte Carlo association was possible or photon is not a true
   *                Cherenkov photon
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel,
                                                     const Rich::RadiatorType rad ) const = 0;

  /** Access the MCRichSegment associated to a given RichRecSegment
   *
   *  @param segment Pointer to a RichRecSegment
   *
   *  @return Point to the associated MCRichSegment
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCRichSegment * mcRichSegment( const RichRecSegment * segment ) const = 0;

  /** Access the MCRichTrack associated to a given RichRecTrack
   *
   *  @param track Pointer to a RichRecTrack
   *
   *  @return Pointer to the associated MCRichTrack
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCRichTrack * mcRichTrack( const RichRecTrack * track ) const = 0;

  /** Access the MCRichTrack associated to a given RichRecSegment
   *
   *  @param segment Pointer to a RichRecSegment
   *
   *  @return Pointer to the associated MCRichTrack
   *  @retval NULL  No Monte Carlo association was possible
   *  @retval !NULL Monte Carlo association was successful
   */
  virtual const MCRichTrack * mcRichTrack( const RichRecSegment * segment ) const = 0;

  /** Access the MCRichSegments associated to a given RichRecTrack
   *
   *  @param track Pointer to a RichRecTrack
   *  
   *  @return Pointer to the SmartRefVector of associated MCRichSegments
   */
  virtual const SmartRefVector<MCRichSegment> * mcRichSegments( const RichRecTrack * track ) const = 0;

  /** Is this RichRecPixel background ?
   *
   *  @param pixel   Pointer to a RichRecPixel
   *
   *  @retval true  Pixel is the result of a background hit in the detector
   *  @retval false Pixel is a Cherenkov Hit
   */
  virtual bool isBackground( const RichRecPixel * pixel ) const = 0;

};

#endif // RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
