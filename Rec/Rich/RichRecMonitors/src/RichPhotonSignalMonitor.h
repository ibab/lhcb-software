
//---------------------------------------------------------------------------
/** @file RichPhotonSignalMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonSignalMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichSellmeirFunc.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class PhotonSignalMonitor RichPhotonSignalMonitor.h
       *
       *  Monitors the photon signals.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class PhotonSignalMonitor : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        PhotonSignalMonitor( const std::string& name,
                             ISvcLocator* pSvcLocator );

        virtual ~PhotonSignalMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr; 

        /// Pointer to RichExpectedTrackSignal tool
        const IExpectedTrackSignal * m_tkSignal = nullptr; 

        /// Pointer to ray-tracing RichGeomEff tool
        const IGeomEff * m_geomEffic = nullptr; 

        /// Refractive index tool
        const ITrackEffectiveRefractiveIndex * m_refIndex = nullptr; 

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr;  

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
