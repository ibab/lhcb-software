// $Id: IRichMCTruthTool.h,v 1.1 2004-06-02 21:37:20 jonesc Exp $
#ifndef RICHMCTOOLS_IRICHMCTRUTHTOOL_H
#define RICHMCTOOLS_IRICHMCTRUTHTOOL_H 1

// Event Model
class MCRichOpticalPhoton;
class MCRichSegment;
class MCRichTrack;
class MCRichDigit;
class MCParticle;
class MCRichHit;
class RichDigit;

/** @class IRichMCTruthTool IRichMCTruthTool.h
 *
 *  Interface for tool performing MC truth associations between Rich event objects
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichMCTruthTool( "IRichMCTruthTool", 1, 0 );

class IRichMCTruthTool : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichMCTruthTool; }

  /// Find best MCParticle association for a given TrStoredTrack
  virtual const MCParticle * mcParticle( const TrStoredTrack * track ) const = 0;

  /// Truth particle type for given MCParticle
  virtual Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) const = 0;

  /// Find parent MCRichDigit association for a given RichDigit
  virtual const MCRichDigit * mcRichDigit( const RichDigit * digit ) const = 0;

  /// Finds parent MCRichHits for given MCRichDigit
  virtual const SmartRefVector<MCRichHit> & mcRichHits( const MCRichDigit * mcDigit ) const = 0;

  /// Returns the MCRichTrack associated to a given TrStoredTrack
  virtual const MCRichTrack * mcRichTrack( const TrStoredTrack * track ) const = 0;

  /// Returns the MCRichTrack associated to a given MCParticle
  virtual const MCRichTrack * mcRichTrack( const MCParticle * mcPart ) const = 0;

  /// Returns the MCRichOptical photon associated to a given MCRichHit
  virtual const MCRichOpticalPhoton * mcOpticalPhoton( const MCRichHit * mcHit ) const = 0;

};

#endif // RICHMCTOOLS_IRICHMCTRUTHTOOL_H
