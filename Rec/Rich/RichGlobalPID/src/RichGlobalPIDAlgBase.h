
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.h,v 1.10 2007-12-14 14:21:18 jonrob Exp $
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

        virtual StatusCode initialize(); ///< Algorithm initialization

        virtual StatusCode sysExecute(); ///< system execute

      protected: // methods

        LHCb::RichGlobalPIDTracks  * gpidTracks();  ///< Access RichGlobalPIDTracks
        LHCb::RichGlobalPIDs       * gpidPIDs();    ///< Access RichGlobalPIDPIDs
        LHCb::RichGlobalPIDSummary * gpidSummary(); ///< Access RichGlobalPIDSummary
        LHCb::ProcStatus           * procStatus();  ///< Access the ProcStatus object

        /// The GPID sequence name
        inline const std::string& gpidName() const { return m_richGPIDName; }

        /// The PID types to consider
        inline const Rich::Particles& pidTypes() const { return m_pidTypes; }

        /// Create new GPID event TES objects
        void createGPIDEvent();

        /// Clean up GPID event TES objects
        void deleteGPIDEvent();

      private: // data

        /// Pointer to parent RichGlobalPIDTrack container
        LHCb::RichGlobalPIDTracks * m_GPIDtracks;

        /// Pointer to parent RichGlobalPIDSummary object
        LHCb::RichGlobalPIDSummary * m_GPIDSummary;

        /// Pointer to parent RichGlobalPID container
        LHCb::RichGlobalPIDs * m_GPIDs;

        /// Pointer to the ProcStatus object
        LHCb::ProcStatus * m_procStat;

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

      inline LHCb::RichGlobalPIDTracks * AlgBase::gpidTracks()
      {
        if ( !m_GPIDtracks ) m_GPIDtracks = get<LHCb::RichGlobalPIDTracks>( m_richGPIDTrackLocation );
        return m_GPIDtracks;
      }

      inline LHCb::RichGlobalPIDs * AlgBase::gpidPIDs()
      {
        if ( !m_GPIDs ) m_GPIDs = get<LHCb::RichGlobalPIDs>( m_richGPIDLocation );
        return m_GPIDs;
      }

      inline LHCb::RichGlobalPIDSummary * AlgBase::gpidSummary()
      {
        if ( !m_GPIDSummary ) m_GPIDSummary = get<LHCb::RichGlobalPIDSummary>( m_richGPIDSummaryLocation );
        return m_GPIDSummary;
      }

      inline LHCb::ProcStatus * AlgBase::procStatus()
      {
        if ( !m_procStat ) m_procStat = get<LHCb::ProcStatus>( m_procStatLocation );
        return m_procStat;
      }

    }
  }
}

#endif // RICHRECALGS_RICHGLOBALPIDALGBASE_H
