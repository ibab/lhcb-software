
//---------------------------------------------------------------------------------
/** @file RichRecSummaryQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::SummaryQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RichRecSummaryQC_H
#define RICHRECQC_RichRecSummaryQC_H 1

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

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class SummaryQC RichRecSummaryQC.h
       *
       *  Quality control monitor for Rich Reconstruction Summary data objects
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-02
       */
      //---------------------------------------------------------------------------------

      class SummaryQC : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        SummaryQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~SummaryQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // data

        const IParticleProperties * m_richPartProp = nullptr; ///< Rich Particle properties

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr;

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr;

        /// Location of the summary tracks
        std::string m_summaryLoc;

        // job options
        double m_minBeta;        ///< minimum beta value for 'saturated' tracks

        unsigned long long m_nEvts{0};                   ///< Number of processed events
        unsigned long long m_nTracks{0};                 ///< Number of summary tracks produced
        std::vector<unsigned long long> m_nSegments;  ///< Number of summary segments produced per radiator
        std::vector<unsigned long long> m_nSegmentsMC;  ///< Number of summary segments produced per radiator, with at least one true CK hit
        std::vector<unsigned long long> m_nPhotons;   ///< Number of summary photons produced per radiator
        std::vector<unsigned long long> m_nTruePhotons; ///< Number of true summary photons produced per radiator

      };

    }
  }
}

#endif // RICHRECQC_RichRecSummaryQC_H
