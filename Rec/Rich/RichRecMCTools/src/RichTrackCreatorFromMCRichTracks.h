
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromMCRichTracks.h
 *
 *  Header file for tool : Rich::Rec::TrackCreatorFromMCRichTracks
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
#define RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H 1

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

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-------------------------------------------------------------------------------------
      /** @class TrackCreatorFromMCRichTracks RichTrackCreatorFromMCRichTracks.h
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

      class TrackCreatorFromMCRichTracks : public TrackCreatorBase
      {

      public: // Methods for Gaudi Framework

        /// Standard constructor
        TrackCreatorFromMCRichTracks( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

        /// Destructor
        virtual ~TrackCreatorFromMCRichTracks() { }

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

        /// Finalise current event
        virtual void FinishEvent();

      private: // definitions

        /// typedef of the Linker object for Tracks to MCParticle
        typedef LinkerWithKey<LHCb::MCParticle,LHCb::Track> TrackToMCP;

      private: // methods

        /// Returns a pointer to the MCRichTracks
        const LHCb::MCRichTracks * trTracks() const;

        /// Returns a pointer to the faked Tracks
        LHCb::Tracks * fakedTracks() const;

        /// Returns track type for given MCRichTrack
        Rich::Rec::Track::Type getTrType( const LHCb::MCRichTrack * track ) const;

      private: // data

        /// Pointer to MCRichTracks
        mutable LHCb::MCRichTracks * m_mcrTracks = nullptr;

        /// Pointer to ring creator
        const IMassHypothesisRingCreator * m_massHypoRings = nullptr;

        /// Pointer to track segment maker
        const ITrSegMaker * m_segMaker = nullptr;

        /// Pointer to RichExpectedTrackSignal interface
        const IExpectedTrackSignal * m_signal = nullptr;

        /// Input location of MCRichTracks in TES
        std::string m_mcrTracksLocation;

        /// Job Option "nickname" of the TrackSegment tool to use
        std::string m_trSegToolNickName;

        // Flag to signify all tracks have been formed for current event
        mutable bool m_allDone = false;

        /// Flag to turn on the creation of the RichRecRings for the segment mass hypotheses
        bool m_buildHypoRings;

        /// Create fake reconstructed tracks
        bool m_fakeRecoTracks;

        /// Fake Reco track location
        std::string m_fakeTrLoc;

        /// Pointer to faked Tracks
        mutable LHCb::Tracks * m_fakeTracks = nullptr;

        /// Map linking MCRichTracks to fake reconstructed tracks
        mutable Rich::Map<const LHCb::MCRichTrack*,const LHCb::Track*> m_mcToFakeMap;

      };

      inline Rich::Rec::Track::Type
      TrackCreatorFromMCRichTracks::getTrType( const LHCb::MCRichTrack * track ) const
      {
        Rich::Rec::Track::Type trType = Rich::Rec::Track::Unknown;
        try { trType = Rich::Rec::Track::type(track); }
        // Catch exceptions ( track type unknown )
        catch ( const GaudiException & expt )
        {
          Error( expt.message() );
        }
        return trType;
      }

    }
  }
}

#endif // RICHRECMCTOOLS_RichTrackCreatorFromMCRichTracks_H
