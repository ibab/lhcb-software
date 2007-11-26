
//---------------------------------------------------------------------------------
/** @file RichRecSummaryCKResMoni.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::SummaryCKResMoni
 *
 *  CVS Log :-
 *  $Id: RichRecSummaryCKResMoni.h,v 1.1 2007-11-26 17:33:38 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2007
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// Event
#include "Event/RichSummaryTrack.h"
#include "Event/MCParticle.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class SummaryCKResMoni RichRecSummaryCKResMoni.h
       *
       *  Quality control monitor for RICH summary information.
       *  Produces CK resolution plots, that can be compared to the 'standard' ones
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   23/11/2007
       */
      //---------------------------------------------------------------------------------

      class SummaryCKResMoni : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        SummaryCKResMoni( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~SummaryCKResMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private:

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
          return m_richRecMCTruth;
        }

      private: // data

        const IParticleProperties * m_richPartProp; ///< Rich Particle properties

        /// Pointer to RichRecMCTruthTool interface
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Location of the summary tracks
        std::string m_summaryLoc;

        // Job Options
        std::vector<double> m_minBeta;     ///< minimum beta value for 'saturated' tracks
        std::vector<double> m_ckResRange;  ///< Histogram ranges for CK resolution plots
        unsigned int m_nBins;              ///< Number of histogram bins

      };

    }
  }
}

#endif // RICHRECQC_RICHRECOQC_H
