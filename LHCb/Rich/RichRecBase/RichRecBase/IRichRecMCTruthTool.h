// $Id: IRichRecMCTruthTool.h,v 1.1.1.1 2003-04-01 13:21:55 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H 1

// GaudiKernel
//#include "GaudiKernel/IAlgTool.h"
//#include "GaudiKernel/SmartRef.h"
//#include "GaudiKernel/SmartRefVector.h"

// Event Model
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichDigit.h"

/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Interface for tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRecMCTruthTool( "IRichRecMCTruthTool", 1, 0 );

class IRichRecMCTruthTool : public virtual IAlgTool {

public:

  /// Destructor
  virtual ~IRichRecMCTruthTool() {}

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRecMCTruthTool; }

  /// Find best MCParticle association for a given RichRecTrack
  virtual MCParticle * mcParticle( const RichRecTrack * richTrack ) = 0;

  /// Find best MCParticle association for a given RichRecSegment
  virtual MCParticle * mcParticle( const RichRecSegment * richSegment ) = 0;

  /// Truth particle type for given RichRecTrack
  virtual Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack ) = 0;

  /// Truth particle type for given RichRecSegment
  virtual Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment ) = 0;

  /// Truth particle type for given MCParticle
  virtual Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) = 0;

  /// Find parent MCParticle association for a given RichRecPixel
  virtual MCParticle * mcParticle( const RichRecPixel * richPixel ) = 0;

  /// Find parent MCRichDigit association for a given RichRecPixel
  virtual MCRichDigit * mcRichDigit( const RichRecPixel * richPixel ) =0;

  /// Find parent MCRichOpticalPhoton association for a given RichRecPixel
  virtual MCRichOpticalPhoton * mcRichOpticalPhoton( const RichRecPixel * richPixel ) = 0;

  /// If pixel and segment have the same MParticle returns pointer to parent,
  /// otherwise return NULL
  virtual MCParticle * trueRecPhoton( const RichRecPhoton * photon ) = 0;

  /// Returns a vector of true Cherenkov pixels for this segment
  virtual RichRecPixelVector* trueCkPixels( const RichRecSegment * segment ) = 0;

};

#endif // RICHRECTOOLS_IRICHRECMCTRUTHTOOL_H
