// $Id: RichRecMCTruthTool.h,v 1.2 2004-02-02 14:24:41 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_RICHRECMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
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

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichRecMCTruthTool : public RichRecToolBase,
                           virtual public IRichRecMCTruthTool,
                           virtual public IIncidentListener {

public:

  /// Standard constructor
  RichRecMCTruthTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Find best MCParticle association for a given RichRecTrack
  const MCParticle * mcParticle( const RichRecTrack * richTrack ) const;

  /// Find best MCParticle association for a given RichRecSegment
  const MCParticle * mcParticle( const RichRecSegment * richSegment ) const;

  /// Truth particle type for given RichRecTrack
  Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack );

  /// Truth particle type for given RichRecSegment
  Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment );

  /// Truth particle type for given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart );

  /// Find parent MCParticles associated to a given RichRecPixel
  bool mcParticle( const RichRecPixel * richPixel,
                   std::vector<const MCParticle*> & mcParts );

  /// Find parent MCRichDigit association for a given RichRecPixel
  const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel );

  /// Find parent MCRichDigit association for a given RichDigit
  const MCRichDigit * mcRichDigit( const RichDigit * digit );

  /// Find parent MCRichHits for a given RichRecPixel
  const SmartRefVector<MCRichHit> & mcRichHits( const RichRecPixel * richPixel );

  /// Finds parent MCRichHits for given MCRichDigit
  const SmartRefVector<MCRichHit> & mcRichHits( const MCRichDigit * mcDigit );

  /// Find parent MCRichOpticalPhotons associated to a given RichRecPixel
  bool mcRichOpticalPhoton( const RichRecPixel * richPixel,
                            SmartRefVector<MCRichOpticalPhoton> & phots );
  
  /// Is this a true photon candidate ?
  /// Do the associated segment and pixel have the same MC parent
  const MCParticle * trueRecPhoton( const RichRecPhoton * photon );

  /// Is this a true photon candidate ?
  /// Do the segment and pixel have the same MC parent
  const MCParticle * trueRecPhoton( const RichRecSegment * segment,
                                    const RichRecPixel * pixel );

  /// Returns the associated MCRichHit if given RichRecPhoton is true (null otherwise)
  const MCRichHit * trueCherenkovHit( const RichRecPhoton * photon );

  /// Returns the associated MCRichOpticalPhoton if given RichRecPhoton is true (null otherwise)
  const MCRichOpticalPhoton * trueOpticalPhoton( const RichRecPhoton * photon );

  /// Is this a true Cherenkov photon candidate ?
  /// Do the associated segment and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon );

  /// Is this a true Cherenkov photon candidate ?
  /// Do the segment and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  const MCParticle * trueCherenkovPhoton( const RichRecSegment * segment,
                                          const RichRecPixel * pixel );

  /// Is the hit due to Cherenkov radiation for given radiator medium ?
  const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel,
                                             Rich::RadiatorType rad );

  /// Returns the MCRichSegment associated to a given RichRecSegment
  const MCRichSegment * mcRichSegment( const RichRecSegment * segment );

  /// Returns the MCRichTrack associated to a given RichRecTrack
  const MCRichTrack * mcRichTrack( const RichRecTrack * track );

  /// Returns the MCRichTrack associated to a given RichRecSegment
  const MCRichTrack * mcRichTrack( const RichRecSegment * segment );

  /// Returns pointer to vector of MCRichSegments associated to a given RichRecTrack
  const SmartRefVector<MCRichSegment> * mcRichSegments( const RichRecTrack * track );

  /// Is this RichRecPixel background ?
  bool isBackground( const RichRecPixel * pixel );

private: // private methods

  const MCRichDigits *         mcRichDigits();
  const MCRichOpticalPhotons * mcRichOpticalPhotons();
  const MCRichTracks *         mcRichTracks();
  const MCRichSegments *       mcRichSegments();

private: // private data

  // tracking MC truth
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

  bool m_mcRichDigitsDone;
  bool m_mcRichOptPhotsDone;
  bool m_mcRichSegmentsDone;
  bool m_mcRichTracksDone;

  /// Pointer to MCRichDigits
  MCRichDigits * m_mcRichDigits;

  /// Pointer to MCRichOpticalPhotons
  MCRichOpticalPhotons * m_mcRichOpticalPhotons;

  /// Pointer to MCRichSegments
  MCRichSegments * m_mcRichSegments;

  /// Pointer to MCRichTracks
  MCRichTracks * m_mcRichTracks;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

  /// Location of MCRichOpticalPhotons in EDS
  std::string m_mcRichPhotonsLocation;

  /// Location of MCRichTracks in EDS
  std::string m_mcRichTracksLocation;

  /// Location of MCRichSegments in EDS
  std::string m_mcRichSegmentsLocation;

  /// PID information
  std::map<int,Rich::ParticleIDType> m_localID;

  /// MC Tracking truth
  std::string m_trAsctName;
  std::string m_trAsctType;
  TrackAsct* m_trackToMCP;

  // Empty container for missing links
  SmartRefVector<MCRichHit> m_emptyContainer;

};

#endif // RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
