
/** @file RichTrackCreatorFromTrStoredTracks.h
 *
 *  Header file for tool : RichTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromTrStoredTracks.h,v 1.19 2005-01-13 14:34:27 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.18  2004/11/11 16:51:32  jonrob
 *  Add separate unique and non-unique counts for debug
 *
 *  Revision 1.17  2004/10/13 09:52:41  jonrob
 *  Speed improvements + various minor changes
 *
 *  Revision 1.16  2004/07/27 20:15:33  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

#ifndef RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base package
#include "RichRecBase/RichRecToolBase.h"
#include "RichRecBase/RichTrackSelector.h"

// interfaces
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichTrSegMaker.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/RichRecTrack.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichTrackCreatorFromTrStoredTracks RichTrackCreatorFromTrStoredTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses TrStoredTracks as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTrackCreatorFromTrStoredTracks : public RichRecToolBase,
                                           virtual public IRichTrackCreator,
                                           virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromTrStoredTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  // Implement the handle method for the Incident service.
  // This is used by the tool at the beginning of events to initialise
  // a new container for the RichRecTracks
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a TrStoredTrack.
  RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input TrStoredTracks
  const StatusCode newTracks() const;

  // Return a pointer to the container of RichRecTracks
  RichRecTracks * richTracks() const;

  /// Returns the number of tracks in the input TrStoredTrack container.
  const long nInputTracks() const;

private: // methods

  /// Returns a pointer to the TrStoredTracks
  const TrStoredTracks * trStoredTracks() const;

  /// Initialise for a new event
  void InitNewEvent();

private: // data

  /// Pointer to RichRecTracks
  mutable RichRecTracks * m_tracks;

  /// Pointer to TrStoredTracks
  mutable TrStoredTracks * m_trTracks;

  /// Pointer to the detector ray tracing tool
  IRichRayTracing * m_rayTrace;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// Pointer to ring creator
  IRichMassHypothesisRingCreator * m_massHypoRings;

  /// Pointer to track segment maker
  IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Job Option "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  // Working object to keep track of formed objects
  mutable RichHashMap<unsigned long, bool> m_trackDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

  /// Track Selector
  RichTrackSelector m_trSelector;

  // Track count
  typedef RichMap< Rich::Track::Type, std::pair< unsigned int, unsigned int > > TrackTypeCount;
  mutable TrackTypeCount m_nTracksUnique;
  mutable TrackTypeCount m_nTracksNonUnique;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Ray-tracing configuration object
  RichTraceMode m_traceMode;

};

inline void RichTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  if ( m_bookKeep ) m_trackDone.clear();
  m_allDone  = false;
  m_trTracks = 0;
  m_tracks   = 0;
  if ( msgLevel(MSG::DEBUG) ) {
    m_nTracksUnique.clear();
    m_nTracksNonUnique.clear();
  }
}

#endif // RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
