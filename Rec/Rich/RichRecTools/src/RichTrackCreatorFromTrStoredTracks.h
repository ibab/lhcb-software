// $Id: RichTrackCreatorFromTrStoredTracks.h,v 1.14 2004-06-29 19:53:39 jonesc Exp $
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

public:

  /// Standard constructor
  RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromTrStoredTracks() { }

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used by the tool at the beginning of events to initialise
  /// a new container for the RichRecTracks
  void handle( const Incident& incident );

  /// Returns a RichRecTrack object pointer for given ContainedObject.
  /// In this implementation the ContainedObject must be a TrStoredTrack.
  RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  /// Form all possible RichRecTracks from input TrStoredTracks
  const StatusCode newTracks() const;

  /// Return a pointer to the container of RichRecTracks
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

  /// Joboption "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  // Working object to keep track of formed objects
  mutable std::map<unsigned long, bool> m_trackDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

  /// Track Selector
  RichTrackSelector m_trSelector;

  // Track count
  typedef std::vector< std::pair<unsigned,unsigned> > TrackTypeCount;
  mutable TrackTypeCount m_nTracks;

};

inline void RichTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  m_trackDone.clear();
  m_allDone  = false;
  m_trTracks = 0;
  m_tracks   = 0;
  if ( msgLevel(MSG::DEBUG) ) {
    for ( TrackTypeCount::iterator i = m_nTracks.begin(); 
          i != m_nTracks.end(); ++i ) { (*i).first = (*i).second = 0; }
  }
}

#endif // RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
