
//---------------------------------------------------------------------------------------------
/** @file RichDelegatedTrackCreatorFromTrStoredTracks.h
 *
 *  Header file for tool : RichDelegatedTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichDelegatedTrackCreatorFromTrStoredTracks.h,v 1.5 2005-02-02 10:05:23 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//---------------------------------------------------------------------------------------------

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

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/StringHashFuncs.h"

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

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichDelegatedTrackCreatorFromTrStoredTracks( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent );

  /// Destructor
  virtual ~RichDelegatedTrackCreatorFromTrStoredTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used by the tool at the beginning of events to initialise
  /// a new container for the RichRecTracks
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a TrStoredTrack.
  RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input TrStoredTracks
  const StatusCode newTracks() const;

  // Return a pointer to the container of RichRecTracks
  RichRecTracks * richTracks() const;

  // Returns the number of tracks in the input TrStoredTrack container.
  const long nInputTracks() const;

private: // methods

  /// Returns a pointer to the TrStoredTracks
  const TrStoredTracks * trStoredTracks() const;

  /// Initialise for a new event
  void InitNewEvent();

  /// Returns the RichTrackCreator tool appropriate for a given track type
  IRichTrackCreator * tkTool( const Rich::Track::Type tkType ) const;

private: // data

  /// Pointer to RichRecTracks
  mutable RichRecTracks * m_tracks;

  /// Pointer to TrStoredTracks
  mutable TrStoredTracks * m_trTracks;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Output location for RichRecTracks in TES
  std::string m_richRecTrackLocation;

  /// Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Working object to keep track of formed objects
  mutable RichHashMap<long int, bool> m_trackDone;

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  /// mapping type between track type and  RichTrackCreator tool pointer
  typedef std::vector< IRichTrackCreator* > TrackToPnt;
  TrackToPnt m_tkToPtn;

};

inline void RichDelegatedTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  m_trackDone.clear();
  m_allDone  = false;
  m_trTracks = 0;
  m_tracks   = 0;
}

inline IRichTrackCreator *
RichDelegatedTrackCreatorFromTrStoredTracks::tkTool( const Rich::Track::Type tkType ) const
{
  if ( !m_tkToPtn[tkType] ) 
  {
    Exception("No creator tool configured for track type '"+Rich::text(tkType)+"'");
  }
  return m_tkToPtn[tkType];
}

#endif // RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H
