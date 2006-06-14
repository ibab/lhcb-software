
//-----------------------------------------------------------------------------
/** @file RichRecMCTruthTool.h
 *
 *  Header file for RICH reconstruction tool : RichRecMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichRecMCTruthTool.h,v 1.15 2006-06-14 22:08:32 jonrob Exp $
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
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/Track.h"
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

// Linkers
#include "Linker/LinkedTo.h"
#include "Linker/LinkerTool.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// constants
#include "Kernel/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichRecMCTruthTool RichRecMCTruthTool.h
 *
 *  Tool performing MC truth associations for RICH reconstruction
 *  data objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Fin
 */
//-----------------------------------------------------------------------------

class RichRecMCTruthTool : public RichRecToolBase,
                           virtual public IRichRecMCTruthTool,
                           virtual public IIncidentListener 
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichRecMCTruthTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~RichRecMCTruthTool();

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public: // Public interface methods

  // Find best MCParticle association for a given reconstructed Track
  const LHCb::MCParticle * mcParticle( const LHCb::Track * track ) const;

  // Find best MCParticle association for a given RichRecTrack
  const LHCb::MCParticle * mcParticle( const LHCb::RichRecTrack * richTrack ) const;

  // Find best MCParticle association for a given RichRecSegment
  const LHCb::MCParticle * mcParticle( const LHCb::RichRecSegment * richSegment ) const;

  // Determines the particle mass hypothesis for a given reconstructed Track
  Rich::ParticleIDType mcParticleType( const LHCb::Track * track ) const;

  // Truth particle type for given RichRecTrack
  Rich::ParticleIDType mcParticleType( const LHCb::RichRecTrack * richTrack ) const;

  // Truth particle type for given RichRecSegment
  Rich::ParticleIDType mcParticleType( const LHCb::RichRecSegment * richSegment ) const;

  // Find parent MCParticles associated to a given RichRecPixel
  bool mcParticle( const LHCb::RichRecPixel * richPixel,
                   std::vector<const LHCb::MCParticle*> & mcParts ) const;

  // Find parent MCRichDigit association for a given RichRecPixel
  const LHCb::MCRichDigit * mcRichDigit( const LHCb::RichRecPixel * richPixel ) const;

  // Find parent MCRichHits for a given RichRecPixel
  const SmartRefVector<LHCb::MCRichHit> & mcRichHits( const LHCb::RichRecPixel * richPixel ) const;

  // Find parent MCRichOpticalPhotons associated to a given RichRecPixel
  bool mcRichOpticalPhoton( const LHCb::RichRecPixel * richPixel,
                            SmartRefVector<LHCb::MCRichOpticalPhoton> & phots ) const;

  // Is this a true photon candidate ?
  // Do the associated segment and pixel have the same MC parent
  const LHCb::MCParticle * trueRecPhoton( const LHCb::RichRecPhoton * photon ) const;

  // Is this a true photon candidate ?
  // Do the segment and pixel have the same MC parent
  const LHCb::MCParticle * trueRecPhoton( const LHCb::RichRecSegment * segment,
                                          const LHCb::RichRecPixel * pixel ) const;

  // Is this a true photon candidate ?
  //  Does the RichSmartID result from a hit from the given MCParticle
  const LHCb::MCParticle * trueRecPhoton( const LHCb::MCParticle * mcPart,
                                          const LHCb::RichSmartID id ) const;

  // Returns the associated MCRichHit if given RichRecPhoton is true (null otherwise)
  const LHCb::MCRichHit * trueCherenkovHit( const LHCb::RichRecPhoton * photon ) const;

  // Returns the associated MCRichOpticalPhoton if given RichRecPhoton is true (null otherwise)
  const LHCb::MCRichOpticalPhoton * trueOpticalPhoton( const LHCb::RichRecPhoton * photon ) const;

  // Returns the associated MCRichOpticalPhoton if given available for given segment and pixel
  const LHCb::MCRichOpticalPhoton * trueOpticalPhoton( const LHCb::RichRecSegment * segment,
                                                       const LHCb::RichRecPixel * pixel ) const;

  // Is this a true Cherenkov photon candidate ?
  // Do the associated segment and pixel have the same MC parent AND was the pixel
  // the result of Cherenkov radiation from the relevant radiator
  const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::RichRecPhoton * photon ) const;

  // Is this a true Cherenkov photon candidate ?
  // Do the segment and pixel have the same MC parent AND was the pixel
  // the result of Cherenkov radiation from the relevant radiator
  const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::RichRecSegment * segment,
                                                const LHCb::RichRecPixel * pixel ) const;

  // Is this a true Cherenkov photon candidate ?
  //  Does the RichSmartID result from a hit from the given MCParticle AND was the hit
  //  the result of Cherenkov radiation from the relevant radiator
  const LHCb::MCParticle * trueCherenkovPhoton( const LHCb::MCParticle * mcPart,
                                                const LHCb::RichSmartID id,
                                                const Rich::RadiatorType rad ) const;

  // Is the hit due to Cherenkov radiation for given radiator medium ?
  const LHCb::MCParticle * trueCherenkovRadiation( const LHCb::RichRecPixel * pixel,
                                                   const Rich::RadiatorType rad ) const;

  // Is the RichSmartID due to true MC Cherenkov radiation from given radiator medium ?
  const LHCb::MCParticle * trueCherenkovRadiation( const LHCb::RichSmartID id,
                                                   const Rich::RadiatorType rad ) const;

  // Returns the MCRichSegment associated to a given RichRecSegment
  const LHCb::MCRichSegment * mcRichSegment( const LHCb::RichRecSegment * segment ) const;

  // Returns the MCRichTrack associated to a given RichRecTrack
  const LHCb::MCRichTrack * mcRichTrack( const LHCb::RichRecTrack * track ) const;

  // Returns the MCRichTrack associated to a given RichRecSegment
  const LHCb::MCRichTrack * mcRichTrack( const LHCb::RichRecSegment * segment ) const;

  // Returns pointer to vector of MCRichSegments associated to a given RichRecTrack
  const SmartRefVector<LHCb::MCRichSegment> * mcRichSegments( const LHCb::RichRecTrack * track ) const;

  // Is this RichRecPixel background ?
  bool isBackground( const LHCb::RichRecPixel * pixel ) const;

private: // definitions

  /// typedef of the Linker object for Tracks to MCParticles
  //typedef LinkedTo<LHCb::MCParticle,LHCb::Track> TrackToMCP;

  typedef LinkerTool<LHCb::Track,LHCb::MCParticle> TrackToMCP;
  typedef TrackToMCP::DirectType                   Table;
  typedef Table::Range                             Range;
  typedef Table::iterator                          iterator;

private: // methods

  /// Returns the linker object for Tracks to MCParticles
  TrackToMCP * trackToMCPLinks() const;

  /// Clean up current linker objects
  void cleanUpLinkers();

  /// Initialise for a new event
  void InitNewEvent();

private: // private data

  /// pointer to RichMCTruth tool
  const IRichMCTruthTool * m_truth;

  /// Linker for Tracks to MCParticles
  mutable TrackToMCP * m_trToMCPLinks;

  /// Location of Tracks in TES
  std::string m_trLoc;

};

inline void RichRecMCTruthTool::cleanUpLinkers()
{
  if ( m_trToMCPLinks ) { delete m_trToMCPLinks; m_trToMCPLinks = 0; }
}

inline void RichRecMCTruthTool::InitNewEvent()
{
  cleanUpLinkers();
}

#endif // RICHRECMCTOOLS_RICHRECMCTRUTHTOOL_H
