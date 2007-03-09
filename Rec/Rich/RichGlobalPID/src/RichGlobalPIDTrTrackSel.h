
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSel.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSel
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDTrTrackSel.h,v 1.18 2007-03-09 22:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H
#define RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H 1

// Base class
#include "RichGlobalPIDAlgBase.h"

// base definitions
#include "RichRecBase/RichRecProcCode.h"

// interfaces
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichTrackSelector.h"

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

        // Private methods
      private:

        /// Clean up after event abortion
        void deleteEvent();

        /// Determine the global PID Status of a track.
        /// Determines how the track will be used in the global likelihood
        Rich::Rec::GlobalPID::TkQuality trackStatus( LHCb::RichRecTrack * track );

        // Private data members
      private:

        const IExpectedTrackSignal * m_tkSignal; ///< Pointer to RichExpectedTrackSignal

        /// Track selector
        const ITrackSelector * m_trSelector;

        // Selection cuts
        double m_minPhysPtot; ///< Minimum momentum for physics quality tracks
        double m_minLLPtot;   ///< Minimum momentum for use in LL calculation
        bool m_resetToPion;   ///< Reset all track hypotheses to pion, ignoring initial types

        /// Maximum number of Tracks with RICH information
        int m_maxUsedTracks;

        /// Maximum total number of input TrStoredTracks
        int m_maxInputTracks;

      };

      inline void TrackSel::deleteEvent()
      {
        // Tidy up incase of event abort
        if ( m_GPIDtracks && !m_GPIDtracks->empty() ) m_GPIDtracks->clear();
        if ( m_GPIDs      && !m_GPIDs->empty()      ) m_GPIDs->clear();
      }

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDTRTRACKSEL_H
