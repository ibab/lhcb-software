
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::RecoQC
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichRecBase/IRichStereoFitter.h"
#include "RichKernel/RichHPDIdentifier.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// RichDet
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "RichDet/DeRichAerogelRadiator.h"
#include "RichDet/DeRichSystem.h"

// boost
#include "boost/assign/list_of.hpp"

// Gaudi
#include "GaudiUtils/Aida2ROOT.h"

// ROOT
#include "TProfile.h"
#include "TH1D.h"
#include "TF1.h"

// FPE exception protection
#include "Kernel/FPEGuard.h"

// Redirect streams
#include "Kernel/STLOStreamRedirect.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class RecoQC RichRecoQC.h
       *
       *  Quality control monitor for Rich Reconstruction
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-02
       */
      //---------------------------------------------------------------------------------

      class RecoQC : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        RecoQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RecoQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      protected:

        /// Pre-Book all (non-MC) histograms
        virtual StatusCode prebookHistograms();

      private:

        /// Fit result
        class FitResult
        {
        public:
          /// Constructor
          FitResult() 
            : OK(false),
              resolution(0),reserror(0),
              mean(0),meanerror(0)
          { }
        public:
          bool   OK;         ///< Fit status
          double resolution; ///< CK resolution
          double reserror;   ///< CK resolution error
          double mean;       ///< Mean
          double meanerror;  ///< Mean error
        };

      private: // methods

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
          return m_richRecMCTruth;
        }

        /// Fit the resolution histograms
        void fitResolutionHistos();

        /// Get the per PD resolution histogram ID
        std::string pdResPlotID( const LHCb::RichSmartID hpd ) const;

        /// Fit the given histogram to extract the resolution
        FitResult fit( TH1D * hist,
                       const Rich::RadiatorType rad );

      private: // data

        // Pointers to tool instances
        const IParticleProperties * m_richPartProp = nullptr; ///< Rich Particle properties
        const ICherenkovAngle * m_ckAngle = nullptr;          ///< Pointer to RichCherenkovAngle tool
        const ICherenkovResolution * m_ckRes = nullptr;       ///< Cherenkov angle resolution tool
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr;  ///< Pointer to RichRecMCTruthTool interface
        const ITrackSelector * m_trSelector = nullptr;        ///< Track selector
        const IIsolatedTrack * m_isoTrack = nullptr;          ///< Isolated track tool
        const IStereoFitter * m_fitter = nullptr;             ///< Stereographic refitting tool

        /// Pointer to RICH system detector element
        const DeRichSystem * m_RichSys = nullptr;

        // job options selection cuts
        std::vector<double> m_minBeta; ///< minimum beta value for tracks
        std::vector<double> m_maxBeta; ///< maximum beta value for tracks

        std::vector<unsigned int> m_truePhotCount; ///< Total number of true cherenkov photons per radiator
        std::vector<unsigned int> m_nSegs;         ///< Total number of track segments per radiator

        std::vector<double> m_ckThetaMax; ///< Max theta limit for histos for each rad
        std::vector<double> m_ckThetaMin; ///< Min theta limit for histos for each rad

        std::vector<bool> m_rads; ///< Which radiators to monitor

        std::vector<unsigned int> m_minRadSegs; ///< Minimum segments per radiator
        std::vector<unsigned int> m_maxRadSegs; ///< Maximum segments per radiator

        /// Histogram fitting event frequency
        long long m_histFitFreq;

        /// Histogram ranges for CK resolution plots
        std::vector<double> m_ckResRange;

        /// Enable aerogel tile plots
        bool m_aeroTilePlots;

        /// Enable per PD resolution plots
        bool m_pdResPlots;

        /// Enable per PD column resolution plots
        bool m_pdColResPlots;

        /// Enable Fitted CK theta resolution plots per PD
        bool m_fittedPDResPlots;

        /// Reject ambiguous photons
        bool m_rejectAmbigPhots;

        /// Event count
        unsigned long long m_nEvts;

        typedef std::pair<Rich::RadiatorType,LHCb::RichSmartID> PDPlotKey;
        typedef Rich::HashMap<PDPlotKey,TH1D*> PDPlotMap;
        PDPlotMap m_pdPlots; ///< PD resolution plots

      };

      //-------------------------------------------------------------------------------

      inline std::string RecoQC::pdResPlotID( const LHCb::RichSmartID hpd ) const
      {
        const Rich::DAQ::HPDIdentifier hid(hpd);
        std::ostringstream id;
        id << "PDs/pd-" << hid.number();
        return id.str();
      }

      //-------------------------------------------------------------------------------

    }
  }
}

#endif // RICHRECQC_RICHRECOQC_H
