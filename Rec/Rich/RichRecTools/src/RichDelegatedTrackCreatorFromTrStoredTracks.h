
//--------------------------------------------------------------------------------------------------
/** @file RichDelegatedTrackCreatorFromTrStoredTracks.h
 *
 *  Header file for tool : RichDelegatedTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichDelegatedTrackCreatorFromTrStoredTracks.h,v 1.6 2005-05-28 13:10:53 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//--------------------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H
#define RICHRECTOOLS_RichDelegatedTrackCreatorFromTrStoredTracks_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// RichKernel
#include "RichKernel/StringHashFuncs.h"

// Event
#include "Event/TrStoredTrack.h"

//--------------------------------------------------------------------------------------------------
/** @class RichDelegatedTrackCreatorFromTrStoredTracks RichDelegatedTrackCreatorFromTrStoredTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Delegates the real work to other tools based on selection criteria.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//--------------------------------------------------------------------------------------------------

class RichDelegatedTrackCreatorFromTrStoredTracks : public RichTrackCreatorBase
{

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

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a TrStoredTrack.
  RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input TrStoredTracks
  const StatusCode newTracks() const;

  // Returns the number of tracks in the input TrStoredTrack container.
  const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

private: // methods

  /// Returns a pointer to the TrStoredTracks
  const TrStoredTracks * trStoredTracks() const;

  /// Returns the RichTrackCreator tool appropriate for a given track type
  IRichTrackCreator * tkTool( const Rich::Track::Type tkType ) const;

private: // data

  /// Pointer to TrStoredTracks
  mutable TrStoredTracks * m_trTracks;

  /// Input location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  typedef std::vector<std::string> ToolList;
  /// Tool data from job options
  ToolList m_names;

  /// mapping type between track type and  RichTrackCreator tool pointer
  typedef std::vector< IRichTrackCreator* > TrackToPnt;
  TrackToPnt m_tkToPtn;

};

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
