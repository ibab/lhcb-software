
//---------------------------------------------------------------------------
/** @file RichPhotonRecoRayTraceTest.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonRecoRayTraceTest
 *
 *  CVS Log :-
 *  $Id: RichPhotonRecoRayTraceTest.h,v 1.1 2008-01-03 15:33:33 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   03/01/2008
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPhotonRecoRayTraceTest_H
#define RICHRECMONITOR_RichPhotonRecoRayTraceTest_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

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

      private: // data

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle * m_ckAngle;         ///< Rich Cherenkov angle calculator tool
        const ITrackSelector * m_trSelector;       ///< Track selector
        const IRayTracing * m_rayTracing;          ///< Ray tracing tool
        const ISmartIDTool * m_idTool;             ///< Pointer to RichSmartID tool

        /// Ray tracing mode
        LHCb::RichTraceMode m_mode;

        /// Flag to turn on/off the correction for refraction at aerogel C4F10 boundary
        bool m_refractCorr;

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPhotonRecoRayTraceTest_H
