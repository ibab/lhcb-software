
//--------------------------------------------------------------------------
/** @file RichGlobalPIDTrTrackSelUsingPIDs.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::TrackSelUsingPIDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/12/2007
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDTrTrackSelUsingPIDs_H
#define RICHGLOBALPID_RichGlobalPIDTrTrackSelUsingPIDs_H 1

// Base class
#include "RichGlobalPIDTrTrackSel.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class TrackSelUsingPIDs RichGlobalPIDTrTrackSelUsingPIDs.h
       *
       *  Track selection algorithm for Rich Global PID. Extends TrackSel adding
       *  additional selection criteria using previous RichPID results
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   10/12/2007
       */
      //--------------------------------------------------------------------------

      class TrackSelUsingPIDs : public TrackSel
      {

      public:

        /// Standard constructor
        TrackSelUsingPIDs( const std::string& name,
                           ISvcLocator* pSvcLocator );

        virtual ~TrackSelUsingPIDs(); ///< Destructor

        virtual StatusCode initialize(); // Algorithm initialization

      protected: // methods

        // General event initialisation
        virtual StatusCode eventInit();

        /// Set tracks for use
        virtual bool trackSelection( LHCb::RichRecTrack * track ) const;

      private: // methods

        /// Get the RichPID container
        inline const LHCb::RichPIDs * richPIDs() const
        {
          if ( !m_pids ) { m_pids = get<LHCb::RichPIDs>(m_richPIDLocation); }
          return m_pids;
        }

      private: // data

        /// Location in TES of RichPIDs to use for selection
        std::string m_richPIDLocation;

        /// Point to RichPID results
        mutable LHCb::RichPIDs * m_pids = nullptr;

        /// Dll selection cuts (El, Mu, Pi, Ka, Pr)
        std::vector<double> m_dllCuts;

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDTrTrackSelUsingPIDs_H
