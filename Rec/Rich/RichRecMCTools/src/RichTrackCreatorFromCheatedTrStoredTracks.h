// $Id: RichTrackCreatorFromCheatedTrStoredTracks.h,v 1.2 2003-11-25 14:01:51 jonesc Exp $
#ifndef RICHRECTOOLS_RichTrackCreatorFromCheatedTrStoredTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromCheatedTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichDetTools/IRichDetInterface.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
#include "Event/RichRecTrack.h"

/** @class RichTrackCreatorFromCheatedTrStoredTracks RichTrackCreatorFromCheatedTrStoredTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses TrStoredTracks as the parent objects but refers to the MCParticle
 *  for the true track
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */

class RichTrackCreatorFromCheatedTrStoredTracks : public RichRecToolBase,
                                                  virtual public IRichTrackCreator,
                                                  virtual public IIncidentListener {

public:

  /// Standard constructor
  RichTrackCreatorFromCheatedTrStoredTracks( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromCheatedTrStoredTracks() {}

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
  RichRecTrack * newTrack ( const ContainedObject * obj  );

  /// Form all possible RichRecTracks from input TrStoredTracks
  StatusCode newTracks();

  /// Return a pointer to the container of RichRecTracks
  RichRecTracks *& richTracks();

  /// Returns the number of tracks in the input TrStoredTrack container.
  long nInputTracks();

private: // methods

  /// Load the TrStoredTracks
  bool loadTrStoredTracks();

private: // data

  // tracking MC truth
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> TrackAsct ;

  /// Pointer to TrStoredTracks
  TrStoredTracks * m_trTracks;

  /// Pointer to RichRecTracks
  RichRecTracks * m_tracks;

  /// Pointer to the RichSegmentCreator tool
  IRichSegmentCreator * m_segCr;

  /// Pointer to the RichDetInterface tool
  IRichDetInterface * m_richDetInt;

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// MC Tracking truth
  std::string m_trAsctName;
  std::string m_trAsctType;
  TrackAsct* m_trackToMCP;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Flag to signify whether to ignore non-unique tracks
  bool m_skipNonUnique;

  // Flag to signify all tracks have been formed for current event
  bool m_allDone;

  // Working object to keep track of formed objects
  std::map<int, bool> m_trackDone;

};

#endif // RICHRECTOOLS_RichTrackCreatorFromCheatedTrStoredTracks_H
