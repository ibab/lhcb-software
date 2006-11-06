
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromMCRichTracks.h
 *
 *  Header file for tool : RichTrackCreatorFromMCRichTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromMCRichTracks.h,v 1.5 2006-11-06 18:18:46 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
#define RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event
#include "Event/MCRichTrack.h"
#include "Event/Track.h"

// kernel
#include "RichKernel/RichMap.h"

// Linker
#include "Linker/LinkerWithKey.h"

//-------------------------------------------------------------------------------------
/** @class RichTrackCreatorFromMCRichTracks RichTrackCreatorFromMCRichTracks.h
 *
 *  Tool for the creation and book-keeping of RichRecTrack objects.
 *  Uses MCRichTracks as the parent objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Fix track selector
 */
//-------------------------------------------------------------------------------------

class RichTrackCreatorFromMCRichTracks : public RichTrackCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTrackCreatorFromMCRichTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

  /// Destructor
  virtual ~RichTrackCreatorFromMCRichTracks() { }

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecTrack object pointer for given ContainedObject.
  // In this implementation the ContainedObject must be a reconstruction 'Track'.
  virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

  // Form all possible RichRecTracks from input Tracks
  virtual const StatusCode newTracks() const;

  /// Returns the number of tracks in the input Track container.
  virtual const long nInputTracks() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

  /// Finalise current event
  virtual void FinishEvent();

private: // defintions

  /// typedef of the Linker object for Tracks to MCParticle
  typedef LinkerWithKey<LHCb::MCParticle,LHCb::Track> TrackToMCP;

private: // methods

  /// Returns a pointer to the MCRichTracks
  const LHCb::MCRichTracks * trTracks() const;

  /// Returns a pointer to the faked Tracks
  LHCb::Tracks * fakedTracks() const;

  /// Returns track type for given MCRichTrack
  Rich::Track::Type getTrType( const LHCb::MCRichTrack * track ) const;

private: // data

  /// Pointer to MCRichTracks
  mutable LHCb::MCRichTracks * m_mcrTracks;

  /// Pointer to ring creator
  const IRichMassHypothesisRingCreator * m_massHypoRings;

  /// Pointer to track segment maker
  const IRichTrSegMaker * m_segMaker;

  /// Pointer to RichExpectedTrackSignal interface
  const IRichExpectedTrackSignal * m_signal;

  /// Input location of MCRichTracks in TES
  std::string m_mcrTracksLocation;

  /// Job Option "nickname" of the TrackSegment tool to use
  std::string m_trSegToolNickName;

  // Flag to signify all tracks have been formed for current event
  mutable bool m_allDone;

  /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
  bool m_buildHypoRings;

  /// Create fake reconstructed tracks
  bool m_fakeRecoTracks;

  /// Fake Reco track location
  std::string m_fakeTrLoc;

  /// Pointer to faked Tracks
  mutable LHCb::Tracks * m_fakeTracks;

  /// Map linking MCRichTracks to fake reconstructed tracks
  mutable Rich::Map<const LHCb::MCRichTrack*,const LHCb::Track*> m_mcToFakeMap;

};

inline Rich::Track::Type 
RichTrackCreatorFromMCRichTracks::getTrType( const LHCb::MCRichTrack * track ) const
{
  Rich::Track::Type trType = Rich::Track::Unknown;
  try { trType = Rich::Track::type(track); }
  // Catch exceptions ( track type unknown )
  catch ( const GaudiException & expt )
  {
    Error( expt.message() );
  }
  return trType;
}


#endif // RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
