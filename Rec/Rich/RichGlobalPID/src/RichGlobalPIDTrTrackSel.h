
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSel.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSel
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDTrTrackSel_H
#define RICHGLOBALPID_RichGlobalPIDTrTrackSel_H 1

// STL
#include <sstream>

// Base class
#include "RichGlobalPIDAlgBase.h"

// base definitions
#include "RichRecBase/RichRecProcCode.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "IRichGlobalPIDTrackCreator.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class TrackSel RichGlobalPIDTrTrackSel.h
       *
       *  Track selection algorithm for Rich Global PID
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   12/12/2002
       *
       *  @todo Find a way to avoid the dynamic_casts when setting Track in RichPID
       */
      //--------------------------------------------------------------------------

      class TrackSel : public AlgBase
      {

      public:

        /// Standard constructor
        TrackSel( const std::string& name,
                  ISvcLocator* pSvcLocator );

        virtual ~TrackSel(); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      protected: // methods

        // General event initialisation
        virtual StatusCode eventInit();
        
        /// Determine the global PID Status of a track.
        /// Determines how the track will be used in the global likelihood
        Rich::Rec::GlobalPID::TkQuality trackStatus( LHCb::RichRecTrack * track );

        /// Set tracks for use
        virtual bool trackSelection( LHCb::RichRecTrack * track ) const;

      private: // data members

        /// Primary Track selector
        const ITrackSelector * m_trSelector = nullptr;

        /// 'Frozen' Track selector
        const ITrackSelector * m_frozenTrSel = nullptr;

        /// RichGlobalPIDTrack creator
        const ITrackCreator * m_gtkCreator = nullptr;

        // Selection cuts
        double m_minPhysPtot; ///< Minimum momentum for physics quality tracks
        double m_minLLPtot;   ///< Minimum momentum for use in LL calculation
        bool m_resetToPion;   ///< Reset all track hypotheses to pion, ignoring initial types

        /// Maximum number of Tracks with RICH information
        unsigned int m_maxUsedTracks{0};

        /// Maximum total number of input TrStoredTracks
        unsigned int m_maxInputTracks{0};

        /// Turn on the use of 'frozen' tracks
        bool m_freezeTracks;

        /// The mass hypothesis for frozen tracks
        int m_frozenType;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDTrTrackSel_H
