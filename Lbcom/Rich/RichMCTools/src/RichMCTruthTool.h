
/** @file RichMCTruthTool.h
 *
 *  Header file for tool : RichMCTruthTool
 *
 *  CVS Log :-
 *  $Id: RichMCTruthTool.h,v 1.9 2004-08-19 14:00:29 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.8  2004/07/26 17:56:09  jonrob
 *  Various improvements to the doxygen comments
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

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
#include "RichKernel/RichParticleIDType.h"

// Event model
#include "Event/TrStoredTrack.h"
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

/** @class RichMCTruthTool RichMCTruthTool.h
 *
 *  Tool performing MC truth associations
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

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

  // Determines the particle mass hypothesis for a given TrStoredTrack
  Rich::ParticleIDType mcParticleType( const TrStoredTrack * track ) const;

  // Determines the particle mass hypothesis for a given MCParticle
  Rich::ParticleIDType mcParticleType( const MCParticle * mcPart ) const;

  // Finds the MCRichDigit association for a given RichDigit
  const MCRichDigit * mcRichDigit( const RichDigit * digit ) const;

  // Finds the MCRichTrack associated to a given TrStoredTrack
  const MCRichTrack * mcRichTrack( const TrStoredTrack * track ) const;

  // Finds the MCRichTrack associated to a given MCParticle
  const MCRichTrack * mcRichTrack( const MCParticle * mcPart ) const;

  // Finds the MCRichOpticalPhoton associated to a given MCRichHit
  const MCRichOpticalPhoton * mcOpticalPhoton( const MCRichHit * mcHit ) const;

  // Checks if the given MCRichDigit is the result of a background
  bool isBackground ( const MCRichDigit * digit ) const;

  // Checks if the given MCRichHit is the result of a background
  bool isBackground( const MCRichHit * hit ) const;

private: // private methods

  /// clean up current linker objects
  void  cleanUpLinkers();

  /** Loads the MCRichDigits into the TES
   *
   * @return Pointer to the MCRichDigits
   */
  const MCRichDigits * mcRichDigits() const;

  /// typedef of the Linker object for MCParticles to MCRichTracks
  typedef LinkedTo<MCRichTrack,MCParticle> MCPartToRichTracks;
  /// Returns the linker object for MCParticles to MCRichTracks
  MCPartToRichTracks * mcTrackLinks() const;

  /// typedef of the Linker object for MCRichHits to MCRichOpticalPhotons
  typedef LinkedTo<MCRichOpticalPhoton,MCRichHit> MCRichHitToPhoton;
  /// Returns the linker object for MCRichHits to MCRichOpticalPhotons
  MCRichHitToPhoton * mcPhotonLinks() const;

  /// Initialise for a new event
  void InitNewEvent();

private: // private data

  // tracking MC truth
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

  mutable bool m_mcRichDigitsDone;

  /// Pointer to MCRichDigits
  mutable MCRichDigits * m_mcRichDigits;

  /// Linker for MCRichTracks
  mutable MCPartToRichTracks * m_mcTrackLinks;

  /// Linker for MCRichOpticalPhotons
  mutable MCRichHitToPhoton * m_mcPhotonLinks;

  /// Location of MCRichDigits in EDS
  std::string m_mcRichDigitsLocation;

  /// PID information
  mutable std::map<int,Rich::ParticleIDType> m_localID;

  /// MC Tracking truth
  std::string m_trAsctName;
  std::string m_trAsctType;
  TrackAsct* m_trackToMCP;

};

inline RichMCTruthTool::MCRichHitToPhoton * RichMCTruthTool::mcPhotonLinks() const
{
  if ( !m_mcPhotonLinks ) {
    m_mcPhotonLinks =
      new MCRichHitToPhoton( evtSvc(), msgSvc(),
                             MCRichOpticalPhotonLocation::LinksFromMCRichHits );
  }
  return m_mcPhotonLinks;
}

inline RichMCTruthTool::MCPartToRichTracks * RichMCTruthTool::mcTrackLinks() const
{
  if ( !m_mcTrackLinks ) {
    m_mcTrackLinks =
      new MCPartToRichTracks( evtSvc(), msgSvc(),
                              MCRichTrackLocation::LinksFromMCParticles );
  }
  return m_mcTrackLinks;
}

inline void RichMCTruthTool::cleanUpLinkers()
{
  if ( m_mcTrackLinks  ) { delete m_mcTrackLinks;  m_mcTrackLinks  = 0; }
  if ( m_mcPhotonLinks ) { delete m_mcPhotonLinks; m_mcPhotonLinks = 0; }
}

inline void RichMCTruthTool::InitNewEvent()
{
  m_mcRichDigitsDone = false;
  cleanUpLinkers();
}

#endif // RICHMCTOOLS_RICHMCTRUTHTOOL_H
