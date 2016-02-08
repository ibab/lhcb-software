
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PixelQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichRecPixelQC_H
#define RICHRECMONITOR_RichRecPixelQC_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/MCRichDigitSummary.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "MCInterfaces/IRichMCTruthTool.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class PixelQC RichRecPixelQC.h
       *
       *  Monitor class for the pixel reconstruction
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //-----------------------------------------------------------------------------

      class PixelQC : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        PixelQC( const std::string& name,
                 ISvcLocator* pSvcLocator );

        virtual ~PixelQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      protected:

        /// Pre-Book all (non-MC) histograms
        virtual StatusCode prebookHistograms();

      private: // helper classes

        /// Simple class containing MC flags for a given pixel (reconstructed hit)
        class MCFlags
        {
        public:
          /// Default constructor
          MCFlags() = default;
          // data members
          bool isBkg{false};           ///< Hit is background, of any sort
          bool isHPDQCK{false};        ///< Hit is from CK radiation from the HPD quartz windows
          bool isGasCK{false};         ///< Hit is from CK radiation from the gas volume quartz windows
          bool isN2CK{false};          ///< Hit is from CK radiation from N2
          bool isChargedTk{false};     ///< Hit is from a charged track hitting the HPD
          bool isChargeShare{false};   ///< Hit is from charge sharing in the HPD silicon detector
          bool isAeroFiltCK{false};    ///< Hit is from CK radiator in the aerogel Filter
          bool isSiBackScatter{false}; ///< Hit is from back-scattering from the silicon chip
          bool isHPDIntReflect{false}; ///< Hit is from internal reflections inside the HPD
          bool isSignal{false};        ///< Hit is from (unscattered) CK radiation from one of the main radiators
          bool isAerogelCK{false};     ///< Hit is from (unscattered) CK radiation from the aerogel radiator
          bool isRich1GasCK{false};    ///< Hit is from (unscattered) CK radiation from the Rich1 gas radiator
          bool isRich2GasCK{false};    ///< Hit is from (unscattered) CK radiation from the Rich2 gas radiator
          bool isRadScint{false};      ///< Hit is from Radiator Scintillation
        };

        /// Stores tallys opf various types
        class Tallys
        {
        public:
          /// Constructor
          Tallys()
            : pixels            ( Rich::NRiches,        0 ),
              bkgs              ( Rich::NRiches,        0 ),
              npdqcks           ( Rich::NRiches,        0 ),
              ngasck            ( Rich::NRiches,        0 ),
              n2ck              ( Rich::NRiches,        0 ),
              ntrack            ( Rich::NRiches,        0 ),
              nchargeshare      ( Rich::NRiches,        0 ),
              naerofilter       ( Rich::NRiches,        0 ),
              nbackscatter      ( Rich::NRiches,        0 ),
              nhpdintreflect    ( Rich::NRiches,        0 ),
              signal            ( Rich::NRiches,        0 ),
              radHits           ( Rich::NRadiatorTypes, 0 ),
              radScint          ( Rich::NRiches,        0 ) { }
          // count type
          using Tally = std::vector<unsigned long long>;
          // data members
          Tally pixels;   ///< Total number of pixels hit
          Tally bkgs;     ///< Total number of background hits
          Tally npdqcks;  ///< Total number of HPD quartz window CK hits
          Tally ngasck;   ///< Total number of gas volume quartz window CK hits
          Tally n2ck;     ///< Total number of N2 CK hits
          Tally ntrack;   ///< Total number of charged track on HPD hits
          Tally nchargeshare; ///< Total number of silicon charge share hits
          Tally naerofilter; ///< Total number of aerogel filter CK hits
          Tally nbackscatter; ///< Total number of back-scattered hits
          Tally nhpdintreflect; ///< Total number of HPD internal reflection hits
          Tally signal;   ///< Total number of signal hits
          Tally radHits;  ///< Total number of signal hits for each radiator
          Tally radScint; ///< Total number of hits from Scintillation
        };

      private: // methods

        /// Get various MC history flags for given RichSmartID
        MCFlags getHistories( const LHCb::RichSmartID id ) const;

        /// Print summary for given RICH
        void printRICH( const Rich::DetectorType rich ) const;

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr;

        /// Pointer to RichMCTruthTool
        const Rich::MC::IMCTruthTool * m_truth = nullptr;

        /// Raw Buffer Decoding tool
        const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder = nullptr;

        /// Pointer to RICH system detector element
        const DeRichSystem * m_richSys = nullptr;

        // tallies

        Tallys m_recoTally; ///< tally for reconstructed pixels
        Tallys m_rawTally;  ///< tally for all raw pixels

        unsigned long long m_nEvts{0}; ///< Total number of events processed

        unsigned int m_maxPixels; ///< Maximum pixels

      };

    }
  }
}

#endif // RICHRECMONITOR_RichRecPixelQC_H
