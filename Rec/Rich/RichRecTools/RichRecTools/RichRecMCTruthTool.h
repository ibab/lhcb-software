// $Id: RichRecMCTruthTool.h,v 1.2 2002-11-20 09:04:08 jonrob Exp $
#ifndef RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
#define RICHRECTOOLS_RICHRECMCTRUTHTOOL_H 1

#include <string>
#include <cmath>
#include <map>

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

// interfaces
#include "RichRecInterfaces/IRichRecMCTruthTool.h"
#include "RichRecInterfaces/IRichRecTrackTool.h"
#include "RichRecInterfaces/IRichRecPixelTool.h"

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

  /// Find parent MCParticle association for a given RichRecPixel
  MCParticle * mcParticle( const RichRecPixel * richPixel );

  /// Find parent MCRichDigit association for a given RichRecPixel
  MCRichDigit * mcRichDigit( const RichRecPixel * richPixel );

  /// If pixel and segment have the same MParticle returns pointer to parent,
  /// otherwise return NULL
  MCParticle * trueRecPhoton( const RichRecPhoton * photon );

private:

  MCRichDigits * mcRichDigits();

  bool mcRichDigitsDone;

  /// Pointer to event data service
  IDataProviderSvc * m_evtDataSvc;

  /// Pointer to Particle property service
  IParticlePropertySvc* m_ppSvc;

  /// Pointer to MCRichDigits
  MCRichDigits * m_mcRichDigits;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

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

};

#endif // RICHRECTOOLS_RICHRECMCTRUTHTOOL_H
