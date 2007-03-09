
/** @file RichMCMassHypoRingsAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::MCMassHypoRingsAlg
 *
 *  CVS Log :-
 *  $Id: RichMCMassHypoRingsAlg.h,v 1.8 2007-03-09 22:59:34 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */

#ifndef RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H
#define RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/MCRichSegment.h"
#include "Event/RichRecRing.h"

// RichKernel
#include "RichKernel/RichHashMap.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      /** @class MCMassHypoRingsAlg RichMCMassHypoRingsAlg.h
       *
       *  Builds RichRecRing objects representing the true Cherenkov rings, as
       *  determined from the Monte Carlo MCRichSegment objects.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   10/01/2003
       */

      class MCMassHypoRingsAlg : public RichRecAlgBase 
      {

      public:

        /// Standard constructor
        MCMassHypoRingsAlg( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~MCMassHypoRingsAlg();   ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // methods

        /// Build the MC rings for a given event location
        StatusCode buildRings( const std::string & evtLoc ) const;

        /// Returns the Ring creator for a given location
        const IMassHypothesisRingCreator * ringCreator( const std::string & loc ) const;

        /// Returns the cherenkov angle for a given MCRichSegment
        double ckTheta( const LHCb::MCRichSegment * segment ) const;

      private: // Private data members

        /// typedef to a list of event locations to process
        typedef std::vector<std::string> EventList;

        /// List of event locations to process
        EventList m_evtLocs;

        /** typedef to a map translating an event location into a particular
         *  Mass Hypothesis ring creator */
        typedef Rich::HashMap<std::string,const IMassHypothesisRingCreator*> RingCreators;

        /// The Mass hypothesis ring creators
        mutable RingCreators m_ringCrs;

        const Rich::MC::IMCTruthTool * m_truth;              ///< MC truth tool
        const Rich::MC::IMCTrackInfoTool * m_mcTkInfo;       ///< MC Track information
        const IRayTraceCherenkovCone * m_rayTrace; ///< Ray tracing

        /// Max Cherenkov theta angle
        std::vector<double> m_maxCKtheta;

        /// Min Cherenkov theta angle
        std::vector<double> m_minCKtheta;

        /// used radiators
        Rich::HashMap<Rich::RadiatorType,bool> m_usedRads;

        /// Ray-tracing configuration object
        LHCb::RichTraceMode m_traceMode;

      };

      inline const IMassHypothesisRingCreator *
      MCMassHypoRingsAlg::ringCreator( const std::string & loc ) const
      {
        const IMassHypothesisRingCreator *& tool = m_ringCrs[loc];
        if (!tool)
        {
          const int slash = loc.find_first_of( "/" );
          const std::string toolName =
            "MCCherenkovRings" + ( slash > 0 ? loc.substr(0,slash) : loc );
          acquireTool( toolName, tool );
        }
        return tool;
      }

    }
  }
}

#endif // RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H
