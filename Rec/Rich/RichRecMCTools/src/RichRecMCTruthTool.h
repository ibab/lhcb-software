
//-----------------------------------------------------------------------------
/** @file RichRecMCTruthTool.h
 *
 *  Header file for RICH reconstruction tool : RichRecMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichRecMCTruthTool.h,v 1.10 2005-06-23 15:14:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHRECMCTRUTHTOOL_H
#define RICHRECMCTOOLS_RICHRECMCTRUTHTOOL_H 1

// STL
#include <sstream>

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecTrack.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Tool performing MC truth associations for RICH reconstruction
 *  data objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichRecMCTruthTool : public RichRecToolBase,
                           virtual public IRichRecMCTruthTool {

public: // methods for Gaudi framework

  /// Standard constructor
  RichRecMCTruthTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // Public interface methods

  // Find best MCParticle association for a given RichRecTrack
  const MCParticle * mcParticle( const RichRecTrack * richTrack ) const;

  // Find best MCParticle association for a given RichRecSegment
  const MCParticle * mcParticle( const RichRecSegment * richSegment ) const;

  // Truth particle type for given RichRecTrack
  Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack ) const;

  // Truth particle type for given RichRecSegment
  Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment ) const;

  // Find parent MCParticles associated to a given RichRecPixel
  bool mcParticle( const RichRecPixel * richPixel,
                   std::vector<const MCParticle*> & mcParts ) const;

  // Find parent MCRichDigit association for a given RichRecPixel
  const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel ) const;

  // Find parent MCRichHits for a given RichRecPixel
  const SmartRefVector<MCRichHit> & mcRichHits( const RichRecPixel * richPixel ) const;

  // Find parent MCRichOpticalPhotons associated to a given RichRecPixel
  bool mcRichOpticalPhoton( const RichRecPixel * richPixel,
                            SmartRefVector<MCRichOpticalPhoton> & phots ) const;
  
  // Is this a true photon candidate ?
  // Do the associated segment and pixel have the same MC parent
  const MCParticle * trueRecPhoton( const RichRecPhoton * photon ) const;

  // Is this a true photon candidate ?
  // Do the segment and pixel have the same MC parent
  const MCParticle * trueRecPhoton( const RichRecSegment * segment,
                                    const RichRecPixel * pixel ) const;

  // Returns the associated MCRichHit if given RichRecPhoton is true (null otherwise)
  const MCRichHit * trueCherenkovHit( const RichRecPhoton * photon ) const;

  // Returns the associated MCRichOpticalPhoton if given RichRecPhoton is true (null otherwise)
  const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecPhoton * photon ) const;

  // Returns the associated MCRichOpticalPhoton if given available for given segment and pixel
  const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecSegment * segment,
                                                 const RichRecPixel * pixel ) const;

  // Is this a true Cherenkov photon candidate ?
  // Do the associated segment and pixel have the same MC parent AND was the pixel
  // the result of Cherenkov radiation from the relevant radiator
  const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon ) const;

  // Is this a true Cherenkov photon candidate ?
  // Do the segment and pixel have the same MC parent AND was the pixel
  // the result of Cherenkov radiation from the relevant radiator
  const MCParticle * trueCherenkovPhoton( const RichRecSegment * segment,
                                          const RichRecPixel * pixel ) const;

  // Is the hit due to Cherenkov radiation for given radiator medium ?
  const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel,
                                             const Rich::RadiatorType rad ) const;

  // Returns the MCRichSegment associated to a given RichRecSegment
  const MCRichSegment * mcRichSegment( const RichRecSegment * segment ) const;

  // Returns the MCRichTrack associated to a given RichRecTrack
  const MCRichTrack * mcRichTrack( const RichRecTrack * track ) const;

  // Returns the MCRichTrack associated to a given TrStoredTrack
  const MCRichTrack * mcRichTrack( const TrStoredTrack * track ) const;

  // Returns the MCRichTrack associated to a given RichRecSegment
  const MCRichTrack * mcRichTrack( const RichRecSegment * segment ) const;

  // Returns pointer to vector of MCRichSegments associated to a given RichRecTrack
  const SmartRefVector<MCRichSegment> * mcRichSegments( const RichRecTrack * track ) const;

  // Is this RichRecPixel background ?
  bool isBackground( const RichRecPixel * pixel ) const;

private: // private data

  /// pointer to RichMCTruth tool
  const IRichMCTruthTool * m_truth;

  /// Empty container for missing links
  SmartRefVector<MCRichHit> m_emptyContainer;

};

#endif // RICHRECMCTOOLS_RICHRECMCTRUTHTOOL_H
