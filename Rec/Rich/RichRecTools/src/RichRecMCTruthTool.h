// $Id: RichRecMCTruthTool.h,v 1.4 2003-07-02 15:37:52 jonesc Exp $
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

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichSegmentCreator.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// tracking MC truth
typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

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
  const MCParticle * mcParticle( const RichRecTrack * richTrack );

  /// Find best MCParticle association for a given RichRecSegment
  const MCParticle * mcParticle( const RichRecSegment * richSegment );

  /// Truth particle type for given RichRecTrack
  Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack );

  /// Truth particle type for given RichRecSegment
  Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment );

  /// Truth particle type for given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart );

  /// Find parent MCParticle association for a given RichRecPixel
  const MCParticle * mcParticle( const RichRecPixel * richPixel );

  /// Find parent MCRichDigit association for a given RichRecPixel
  const MCRichDigit * mcRichDigit( const RichRecPixel * richPixel );

  /// Find parent MCRichOpticalPhoton association for a given RichRecPixel
  const MCRichOpticalPhoton * mcRichOpticalPhoton( const RichRecPixel * richPixel );

  /// Is this a true photon candidate ?
  /// Do the associated track and pixel have the same MC parent
  const MCParticle * trueRecPhoton( const RichRecPhoton * photon );

  /// Is this a true Cherenkov photon candidate ?
  /// Do the associated track and pixel have the same MC parent AND was the pixel
  /// the result of Cherenkov radiation from the relevant radiator
  const MCParticle * trueCherenkovPhoton( const RichRecPhoton * photon );

  /// Is the hit due to Cherenkov radiation for given radiator medium ?
  const MCParticle * trueCherenkovRadiation( const RichRecPixel * pixel, 
                                             Rich::RadiatorType rad );

  /// Returns a vector of pointers to true Cherenkov pixels for this segment
  RichRecPixelVector* trueCkPixels( const RichRecSegment * segment );

private: // private methods

  MCRichDigits * mcRichDigits();
  MCRichOpticalPhotons * mcRichOpticalPhotons();

private: // private data

  bool m_mcRichDigitsDone;
  bool m_mcRichOptPhotsDone;
  bool m_mcTruePixelsDone;
  std::map<const RichRecSegment*, RichRecPixelVector > m_mcPixelMap;

  /// Pointer to event data service
  IDataProviderSvc * m_evtDataSvc;

  /// Pointer to MCRichDigits
  MCRichDigits * m_mcRichDigits;

  /// Pointer to MCRichOpticalPhotons
  MCRichOpticalPhotons * m_mcRichOpticalPhotons;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

  /// Location of MCRichOpticalPhotons in EDS
  std::string m_mcRichPhotonsLocation;

  /// PID information
  std::map<int,Rich::ParticleIDType> m_localID;

  /// MC Tracking truth
  std::string m_trAsctName;
  std::string m_trAsctType;
  TrackAsct* m_trackToMCP;

  /// Pointer to RichTrackCreator interface
  IRichTrackCreator * m_trackCreator;

  /// Pointer to RichRecPixelTool interface
  IRichPixelCreator * m_pixelCreator;

  /// Pointer to RichSegmentCreator
  IRichSegmentCreator * m_segCreator;

};

#endif // RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
