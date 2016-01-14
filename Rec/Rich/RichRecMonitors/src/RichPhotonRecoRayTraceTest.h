
//---------------------------------------------------------------------------
/** @file RichPhotonRecoRayTraceTest.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonRecoRayTraceTest
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPhotonRecoRayTraceTest_H
#define RICHRECMONITOR_RichPhotonRecoRayTraceTest_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class PhotonRecoRayTraceTest RichPhotonRecoRayTraceTest.h
       *
       *  Monitor that checks the reconstruction by comparing the reconstructed
       *  photon candidates to the results obtained when ray tracing the same
       *  photon parameters (emission point and direction at emission).
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   03/01/2008
       */
      //---------------------------------------------------------------------------

      class PhotonRecoRayTraceTest : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        PhotonRecoRayTraceTest( const std::string& name,
                                ISvcLocator* pSvcLocator );

        virtual ~PhotonRecoRayTraceTest( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // methods

        /// Make the ray tracing test plots
        void makePlots( const std::string & type,
                        const Gaudi::XYZPoint  & emissPoint,
                        const Gaudi::XYZVector & emissDir,
                        const Gaudi::XYZPoint  & detectPoint,
                        const Gaudi::XYZPoint  & primMirrPnt,                        
                        const Gaudi::XYZPoint  & secMirrPnt,
                        const Rich::DetectorType rich,
                        const Rich::RadiatorType rad,
                        const double ckPhi,
                        const double photonEnergy );

      private: // data

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr; ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle * m_ckAngle = nullptr;         ///< Rich Cherenkov angle calculator tool
        const ITrackSelector * m_trSelector = nullptr;       ///< Track selector
        const IRayTracing * m_rayTracing = nullptr;          ///< Ray tracing tool
        const ISmartIDTool * m_idTool = nullptr;             ///< Pointer to RichSmartID tool

        /// Use full HPD treatment in ray tracing
        bool m_fullHPDs;

        /// Ray tracing mode
        LHCb::RichTraceMode m_mode;

        /// Flag to turn on/off the correction for refraction at aerogel C4F10 boundary
        bool m_refractCorr;

        /// Reject ambiguous photons ?
        bool m_rejectAmbigPhots;

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPhotonRecoRayTraceTest_H
