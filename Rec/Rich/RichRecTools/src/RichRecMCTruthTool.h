// $Id: RichRecMCTruthTool.h,v 1.1 2003-04-01 14:33:20 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_RICHRECMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

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
#include "Event/MCRichDigit.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichRecTrackTool.h"
#include "RichRecBase/IRichRecPixelTool.h"
#include "RichRecBase/IRichRecSegmentTool.h"

// Forward declarations
class IDataProviderSvc;

/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// tracking MC truth
typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

class RichRecMCTruthTool : public AlgTool,
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
  MCParticle * mcParticle( const RichRecTrack * richTrack );

  /// Find best MCParticle association for a given RichRecSegment
  MCParticle * mcParticle( const RichRecSegment * richSegment );

  /// Truth particle type for given RichRecTrack
  Rich::ParticleIDType mcParticleType( const RichRecTrack * richTrack );

  /// Truth particle type for given RichRecSegment
  Rich::ParticleIDType mcParticleType( const RichRecSegment * richSegment );

  /// Truth particle type for given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart );

  /// Find parent MCParticle association for a given RichRecPixel
  MCParticle * mcParticle( const RichRecPixel * richPixel );

  /// Find parent MCRichDigit association for a given RichRecPixel
  MCRichDigit * mcRichDigit( const RichRecPixel * richPixel );

  /// Find parent MCRichOpticalPhoton association for a given RichRecPixel
  MCRichOpticalPhoton * mcRichOpticalPhoton( const RichRecPixel * richPixel );

  /// If pixel and segment have the same MParticle returns pointer to parent,
  /// otherwise return NULL
  MCParticle * trueRecPhoton( const RichRecPhoton * photon );

  /// Returns a vector of pointers to true Cherenkov pixels for this segment
  RichRecPixelVector* trueCkPixels( const RichRecSegment * segment );

private:

  MCRichDigits * mcRichDigits();
  MCRichOpticalPhotons * mcRichOpticalPhotons();

  bool m_mcRichDigitsDone;
  bool m_mcRichOptPhotsDone;
  bool m_mcTruePixelsDone;
  std::map<const RichRecSegment*, RichRecPixelVector > m_mcPixelMap;

  /// Pointer to event data service
  IDataProviderSvc * m_evtDataSvc;

  /// Pointer to Particle property service
  IParticlePropertySvc* m_ppSvc;

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

  /// Pointer to RichRecTrackTool interface
  IRichRecTrackTool* m_richRecTrackTool;

  /// Pointer to RichRecPixelTool interface
  IRichRecPixelTool* m_richRecPixelTool;

  /// Pointer to RichRecSegmentTool interface
  IRichRecSegmentTool* m_richRecSegmentTool;

};

#endif // RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
