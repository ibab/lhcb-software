
//---------------------------------------------------------------------------
/** @file RichPhotonSignalMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonSignalMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignalMonitor.h,v 1.10 2008-11-30 10:48:12 jonrob Exp $
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

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
        const IExpectedTrackSignal * m_tkSignal;   ///< Pointer to RichExpectedTrackSignal tool
        const IGeomEff * m_geomEffic; ///< Pointer to ray-tracing RichGeomEff tool
        const ITrackEffectiveRefractiveIndex * m_refIndex; ///< Refractive index tool
        const ITrackSelector * m_trSelector;  ///< Track selector

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
