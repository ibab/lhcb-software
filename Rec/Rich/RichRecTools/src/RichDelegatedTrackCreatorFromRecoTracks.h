
//--------------------------------------------------------------------------------------------------
/** @file RichDelegatedTrackCreatorFromRecoTracks.h
 *
 *  Header file for tool : RichDelegatedTrackCreatorFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichDelegatedTrackCreatorFromRecoTracks.h,v 1.3 2006-12-01 17:05:08 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//--------------------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichDelegatedTrackCreatorFromRecoTracks_H
#define RICHRECTOOLS_RichDelegatedTrackCreatorFromRecoTracks_H 1

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// Event
#include "Event/Track.h"

//--------------------------------------------------------------------------------------------------
/** @class RichDelegatedTrackCreatorFromRecoTracks RichDelegatedTrackCreatorFromRecoTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Delegates the real work to other tools based on selection criteria.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//--------------------------------------------------------------------------------------------------

class RichDelegatedTrackCreatorFromRecoTracks : public RichTrackCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichDelegatedTrackCreatorFromRecoTracks( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );

  /// Destructor
  virtual ~RichDelegatedTrackCreatorFromRecoTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a Track.
  virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input Tracks
  const StatusCode newTracks() const;

  // Returns the number of tracks in the input Track container.
  const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

private: // methods

  /// Returns a pointer to the Tracks
  const LHCb::Tracks * recoTracks() const;

  /// Returns the RichTrackCreator tool appropriate for a given track type
  const IRichTrackCreator * tkTool( const Rich::Track::Type tkType ) const;

private: // data

  /// Pointer to Tracks
  mutable LHCb::Tracks * m_trTracks;

  /// Input location of Tracks in TES
  std::string m_trTracksLocation;

  /// Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  /// mapping type between track type and  RichTrackCreator tool pointer
  typedef std::vector< const IRichTrackCreator* > TrackToPnt;
  TrackToPnt m_tkToPtn;

};

inline const IRichTrackCreator *
RichDelegatedTrackCreatorFromRecoTracks::tkTool( const Rich::Track::Type tkType ) const
{
  if ( !m_tkToPtn[tkType] )
  {
    Exception("No creator tool configured for track type '"+Rich::text(tkType)+"'");
  }
  return m_tkToPtn[tkType];
}

#endif // RICHRECTOOLS_RichDelegatedTrackCreatorFromRecoTracks_H
