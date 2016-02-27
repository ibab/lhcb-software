
//---------------------------------------------------------------------------
/** @file RichPhotonRayTraceTest.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonRayTraceTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_PhotonRayTraceTest_H
#define RICHRECMONITOR_PhotonRayTraceTest_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
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
      /** @class PhotonRayTraceTest RichPhotonRayTraceTest.h
       *
       *  Monitor the reconstructed photon trajectories
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class PhotonRayTraceTest : public Rich::Rec::TupleAlgBase
      {

      public:

        /// Standard constructor
        PhotonRayTraceTest( const std::string& name,
                            ISvcLocator* pSvcLocator );

        virtual ~PhotonRayTraceTest( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // methods

        /// run a ray tracing test for a given photon 
        void rayTrace( const LHCb::RichRecPhoton * photon,
                       const LHCb::RichTraceMode mode,
                       const std::string & name );

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth = nullptr;

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr;

        /// Pointer RichDetector ray tracing tool
        const IRayTracing * m_rayTrace = nullptr;

        /// RichSmartID manipulation tool
        const ISmartIDTool * m_smartIDTool = nullptr; 

      };

    }
  }
}

#endif // RICHRECMONITOR_PhotonRayTraceTest_H
