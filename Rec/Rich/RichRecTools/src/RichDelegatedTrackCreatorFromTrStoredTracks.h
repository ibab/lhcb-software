// $Id: RichDelegatedTrackCreatorFromTrStoredTracks.h,v 1.2 2004-06-29 19:53:37 jonesc Exp $
#ifndef RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H
#define RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichTrackCreator.h"

// Event
#include "Event/TrStoredTrack.h"
#include "Event/RichRecTrack.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichDelegatedTrackCreatorFromTrStoredTracks RichDelegatedTrackCreatorFromTrStoredTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Delegates the real work to other tools based on selection criteria.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichDelegatedTrackCreatorFromTrStoredTracks : public RichRecToolBase,
                                                    virtual public IRichTrackCreator,
                                                    virtual public IIncidentListener {

public:

  /// Standard constructor
  RichDelegatedTrackCreatorFromTrStoredTracks( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent );

  /// Destructor
  virtual ~RichDelegatedTrackCreatorFromTrStoredTracks() { }

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

  /// Returns the name of the tool to use for a given track
  const std::string & toolName( const TrStoredTrack * track ) const;

  /// Returns the RichTrackCreator tool appropriate for a given track
  IRichTrackCreator * tkTool( const TrStoredTrack * track ) const;

private: // data

  /// Pointer to RichRecTracks
  mutable RichRecTracks * m_tracks;

  /// Pointer to TrStoredTracks
  mutable TrStoredTracks * m_trTracks;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  // Working object to keep track of formed objects
  mutable std::map<unsigned long, bool> m_trackDone;

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  /// map between track types and RichTrackCreator tools
  typedef std::map<std::string,std::string> TypeToCreator;
  mutable TypeToCreator m_trackToTool;

  // map between tool name and pointer to instance
  typedef std::map<std::string,IRichTrackCreator*> ToolNameToPnt;
  mutable ToolNameToPnt m_nameToPnt;

};

inline void RichDelegatedTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  m_trackDone.clear();
  m_allDone  = false;
  m_trTracks = 0;
  m_tracks   = 0;
}

inline const std::string & 
RichDelegatedTrackCreatorFromTrStoredTracks::toolName( const TrStoredTrack * track ) const
{
  return m_trackToTool[Rich::text(track)];
}

inline IRichTrackCreator *
RichDelegatedTrackCreatorFromTrStoredTracks::tkTool( const TrStoredTrack * track ) const
{
  const std::string & toolType = toolName(track);
  if ( 0 == m_nameToPnt[toolType] ) acquireTool( toolType, m_nameToPnt[toolType] );
  return m_nameToPnt[toolType];
}

#endif // RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H
