// $Id: RichTrackCreatorFromTrStoredTracks.h,v 1.2 2003-08-06 11:08:14 jonrob Exp $
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
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichDetTools/IRichDetInterface.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/RichRecTrack.h"

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
  RichRecTrack * newTrack ( ContainedObject * obj  );

  /// Form all possible RichRecTracks from input TrStoredTracks
  StatusCode newTracks();

  /// Return a pointer to the container of RichRecTracks
  RichRecTracks *& richTracks();

private:

  /// Pointer to RichRecTracks
  RichRecTracks * m_tracks;

  /// Pointer to event data service
  IDataProviderSvc* m_evtDataSvc;

  /// Pointer to the RichSegmentCreator tool
  IRichSegmentCreator * m_segCr;

  /// Pointer to the RichDetInterface tool
  IRichDetInterface * m_richDetInt;

  /// Pointer to RichExpectedTrackSignal interface
  IRichExpectedTrackSignal * m_signal;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Outpuit location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  // Flag to signify all tracks have been formed for current event
  bool m_allDone;

  // Working object to keep track of formed objects
  std::map<int, bool> m_trackDone;

};

#endif // RICHRECTOOLS_RichTrackCreatorFromTrStoredTracks_H
