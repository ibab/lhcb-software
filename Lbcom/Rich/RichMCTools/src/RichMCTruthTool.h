
//-----------------------------------------------------------------------------
/** @file RichMCTruthTool.h
 *
 *  Header file for tool : RichMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichMCTruthTool.h,v 1.16 2005-03-03 15:46:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCTOOLS_RICHMCTRUTHTOOL_H
#define RICHMCTOOLS_RICHMCTRUTHTOOL_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichKernel/RichToolBase.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichParticleIDType.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/TrgTrack.h"
#include "Event/MCParticle.h"
#include "Event/MCTruth.h"
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCRichHit.h"

// Linkers
#include "Linker/LinkedTo.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichMCTruthTool RichMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class RichMCTruthTool : public RichToolBase,
                        virtual public IRichMCTruthTool,
                        virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMCTruthTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from interface

  // Find best MCParticle association for a given TrStoredTrack
  const MCParticle * mcParticle( const TrStoredTrack * track ) const;

  // Find best MCParticle association for a given TrgTrack
  const MCParticle * mcParticle( const TrgTrack * track ) const;

  // Determines the particle mass hypothesis for a given TrStoredTrack
  Rich::ParticleIDType mcParticleType( const TrStoredTrack * track ) const;

  // Determines the particle mass hypothesis for a given TrgTrack
  Rich::ParticleIDType mcParticleType( const TrgTrack * track ) const;

  // Determines the particle mass hypothesis for a given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) const;

  // Finds the MCRichDigit association for a given RichDigit
  const MCRichDigit * mcRichDigit( const RichDigit * digit ) const;

  // Finds the MCRichDigit association for a RichSmartID channel identifier
  const MCRichDigit * mcRichDigit( const RichSmartID id ) const;

  // Finds the MCRichTrack associated to a given TrStoredTrack
  const MCRichTrack * mcRichTrack( const TrStoredTrack * track ) const;

  // Finds the MCRichTrack associated to a given TrgTrack
  const MCRichTrack * mcRichTrack( const TrgTrack * track ) const;

  // Finds the MCRichTrack associated to a given MCParticle
  const MCRichTrack * mcRichTrack( const MCParticle * mcPart ) const;

  // Finds the MCRichOpticalPhoton associated to a given MCRichHit
  const MCRichOpticalPhoton * mcOpticalPhoton( const MCRichHit * mcHit ) const;

  // Checks if the given MCRichDigit is the result of a background
  bool isBackground ( const MCRichDigit * digit ) const;

  // Checks if the given MCRichHit is the result of a background
  bool isBackground( const MCRichHit * hit ) const;

private: // definitions

  /// (offline) tracking MC truth typedef
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

  /// typedef of the Linker object for MCParticles to MCRichTracks
  typedef LinkedTo<MCRichTrack,MCParticle> MCPartToRichTracks;

  /// typedef of the Linker object for MCRichHits to MCRichOpticalPhotons
  typedef LinkedTo<MCRichOpticalPhoton,MCRichHit> MCRichHitToPhoton;

  /// typedef of the Linker object for TrgTracks to MCParticles
  typedef LinkedTo<MCParticle,TrgTrack> TrgTrackToMCP;

private: // private methods

  /// Returns the linker object for MCParticles to MCRichTracks
  MCPartToRichTracks * mcTrackLinks() const;

  /// Returns the linker object for MCRichHits to MCRichOpticalPhotons
  MCRichHitToPhoton * mcPhotonLinks() const;

  ///  Returns the linker object for TrgTracks to MCParticles
  TrgTrackToMCP * trgTrackToMCPLinks() const;

  /// clean up current linker objects
  void cleanUpLinkers();

  /** Loads the MCRichDigits from the TES
   *
   * @return Pointer to the MCRichDigits
   * @retval NULL means no MC information is available
   */
  const MCRichDigits * mcRichDigits() const;

  /** Loads the MCRichHits from the TES
   *
   * @return Pointer to the MCRichHits
   * @retval NULL means no MC information is available
   */
  const MCRichHits * mcRichHits() const;

  /// Initialise for a new event
  void InitNewEvent();

  /// Load the (offline) track associator on demand
  inline TrackAsct * trackAsct() const
  {
    if ( !m_trackToMCP ) { m_trackToMCP = tool<TrackAsct>( m_trAsctType, m_trAsctName ); }
    return m_trackToMCP;
  }

private: // private data

  /// Flag to say MCRichDigits have been loaded for this event
  mutable bool m_mcRichDigitsDone;

  /// Flag to say MCRichHits have been loaded for this event
  mutable bool m_mcRichHitsDone;

  /// Pointer to MCRichDigits
  mutable MCRichDigits * m_mcRichDigits;

  /// Pointer to MCRichDigits
  mutable MCRichHits * m_mcRichHits;

  /// Linker for MCParticles to MCRichTracks
  mutable MCPartToRichTracks * m_mcTrackLinks;

  /// Linker for MCRichHits to MCRichOpticalPhotons
  mutable MCRichHitToPhoton * m_mcPhotonLinks;

  /// Linker for TrgTracks to MCParticles
  mutable TrgTrackToMCP * m_trgTrToMCPLinks;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

  /// Location of MCRichHits in EDS
  std::string m_mcRichHitsLocation;

  /// PID information
  mutable RichMap<int,Rich::ParticleIDType> m_localID;

  // MC Tracking truth
  std::string m_trAsctName; ///< Track associator name
  std::string m_trAsctType; ///< Track associator type
  mutable TrackAsct * m_trackToMCP; ///< Pointer to track associator

};

inline void RichMCTruthTool::cleanUpLinkers()
{
  if ( m_mcTrackLinks    ) { delete m_mcTrackLinks;    m_mcTrackLinks    = 0; }
  if ( m_mcPhotonLinks   ) { delete m_mcPhotonLinks;   m_mcPhotonLinks   = 0; }
  if ( m_trgTrToMCPLinks ) { delete m_trgTrToMCPLinks; m_trgTrToMCPLinks = 0; }
}

inline void RichMCTruthTool::InitNewEvent()
{
  m_mcRichDigitsDone = false;
  cleanUpLinkers();
}

#endif // RICHMCTOOLS_RICHMCTRUTHTOOL_H
