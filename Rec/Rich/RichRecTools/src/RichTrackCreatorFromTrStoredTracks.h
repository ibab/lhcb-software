// $Id: RichTrackCreatorFromTrStoredTracks.h,v 1.7 2004-03-16 13:45:07 jonesc Exp $
#ifndef RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichDetTools/IRichRayTracing.h"
#include "RichDetTools/IRichSmartIDTool.h"
#include "RichDetTools/IRichTrSegMaker.h"

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
  virtual ~RichTrackCreatorFromTrStoredTracks() {}

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

  /// Load the TrStoredTracks
  const bool loadTrStoredTracks() const;

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

  /// Pointer to track segment maker
  IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Flag to signify whether to ignore non-unique tracks
  bool m_skipNonUnique;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  // Working object to keep track of formed objects
  mutable std::map<unsigned long, bool> m_trackDone;

  // Momentum cut values for each track type
  std::vector<double> m_tkPcut;

  // Initialise track count
  typedef std::vector<unsigned> TrackTypeCount;
  mutable TrackTypeCount m_nTracks;

};

inline void RichTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  m_trackDone.clear();
  m_allDone  = false;
  m_trTracks = 0;
  m_tracks   = 0;
  for ( TrackTypeCount::iterator i = m_nTracks.begin(); 
        i != m_nTracks.end(); ++i ) { *i = 0; }
}

#endif // RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
