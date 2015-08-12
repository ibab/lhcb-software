//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::TrackCreatorFromRecoTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H
#define RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H 1

// base class
#include "RichRecBase/RichTrackCreatorBase.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event
#include "Event/Track.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

namespace Rich
{
  namespace Rec
  {

    //-------------------------------------------------------------------------------------
    /** @class TrackCreatorFromRecoTracks RichTrackCreatorFromRecoTracks.h
     *
     *  Tool for the creation and book-keeping of RichRecTrack objects.
     *  Uses reconstructed Tracks as the parent objects.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-------------------------------------------------------------------------------------

    class TrackCreatorFromRecoTracks : public TrackCreatorBase
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TrackCreatorFromRecoTracks( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent );

      /// Destructor
      virtual ~TrackCreatorFromRecoTracks() { }

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Returns a RichRecTrack object pointer for given ContainedObject.
      // In this implementation the ContainedObject must be a reconstruction 'Track'.
      virtual LHCb::RichRecTrack * newTrack ( const ContainedObject * obj ) const;

      // Form all possible RichRecTracks from input Tracks
      virtual StatusCode newTracks() const;

      /// Returns the number of tracks in the input Track container.
      virtual unsigned int nInputTracks() const;

    protected: // methods

      /// Initialise for a new event
      virtual void InitNewEvent();

    private: // methods

      /// Returns a pointer to the Tracks
      const LHCb::Tracks * trTracks() const;

      /// Remove RICH tracks by type
      unsigned int removeTracksByType( const Rich::Rec::Track::Type type ) const;

    private: // data

      /// Pointer to Tracks
      mutable LHCb::Tracks * m_trTracks;

      /// Pointer to ring creator
      const IMassHypothesisRingCreator * m_massHypoRings;

      /// Pointer to track segment maker
      const ITrSegMaker * m_segMaker;

      /// Pointer to RichExpectedTrackSignal interface
      const IExpectedTrackSignal * m_signal;

      /// Input location of Tracks in TES
      std::string m_trTracksLocation;

      /// Job Option "nickname" of the TrackSegment tool to use
      std::string m_trSegToolNickName;

      // Flag to signify all tracks have been formed for current event
      mutable bool m_allDone;

      /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
      bool m_buildHypoRings;

    };

  }
}

#endif // RICHRECTOOLS_RichTrackCreatorFromRecoTracks_H
