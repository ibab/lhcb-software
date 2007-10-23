
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.h,v 1.9 2007-10-23 10:43:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */
//--------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHGLOBALPIDALGBASE_H
#define RICHRECALGS_RICHGLOBALPIDALGBASE_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichGlobalPIDTrack.h"
#include "Event/RichGlobalPIDSummary.h"
#include "Event/RichGlobalPID.h"
#include "Event/ProcStatus.h"

// interfaces
#include "RichKernel/IRichParticleProperties.h"

// Definitions
#include "RichGlobalPID/RichGlobalPIDTkQuality.h"
#include "RichRecBase/RichTrackID.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class AlgBase RichGlobalPIDAlgBase.h
       *
       *  Abstract base class for GlobalPID algorithms
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2002-11-30
       */
      //--------------------------------------------------------------------------

      class AlgBase : public Rich::Rec::AlgBase
      {

      public:

        ///< Standard constructor
        AlgBase( const std::string& name,
                 ISvcLocator* pSvcLocator );

        virtual ~AlgBase() = 0;  ///< Destructor

        virtual StatusCode initialize(); // Algorithm initialization

        // Protected methods
      protected:

        StatusCode gpidTracks();  ///< Update pointer to RichGlobalPIDTracks
        StatusCode gpidPIDs();    ///< Update pointer to RichGlobalPIDPIDs
        StatusCode gpidSummary(); ///< Update pointer to RichGlobalPIDSummary

        // Protected data
      protected:

        /// Pointer to parent RichGlobalPIDTrack container
        LHCb::RichGlobalPIDTracks * m_GPIDtracks;

        /// Pointer to parent RichGlobalPIDSummary object
        LHCb::RichGlobalPIDSummary * m_GPIDSummary;

        /// Pointer to parent RichGlobalPID container
        LHCb::RichGlobalPIDs * m_GPIDs;

        /// Location of working RICH Global PID tracks in TES
        std::string m_richGPIDTrackLocation;

        /// Location of Global PID Summary object in TES
        std::string m_richGPIDSummaryLocation;

        /// Location of Global PID results in TES
        std::string m_richGPIDLocation;

        /// Global algorithm name. Common to all sub-algorithms
        std::string m_richGPIDName;

        /// Location of processing status object in TES
        std::string m_procStatLocation;

        /// Pointer to RichParticleProperties interface
        const IParticleProperties * m_richPartProp;

        /// Particle ID types to consider
        Rich::Particles m_pidTypes;

      };

      inline StatusCode AlgBase::gpidTracks()
      {
        m_GPIDtracks = get<LHCb::RichGlobalPIDTracks>( m_richGPIDTrackLocation );
        return StatusCode::SUCCESS;
      }

      inline StatusCode AlgBase::gpidPIDs()
      {
        m_GPIDs = get<LHCb::RichGlobalPIDs>( m_richGPIDLocation );
        return StatusCode::SUCCESS;
      }

      inline StatusCode AlgBase::gpidSummary()
      {
        m_GPIDSummary = get<LHCb::RichGlobalPIDSummary>( m_richGPIDSummaryLocation );
        return StatusCode::SUCCESS;
      }

    }
  }
}

#endif // RICHRECALGS_RICHGLOBALPIDALGBASE_H
