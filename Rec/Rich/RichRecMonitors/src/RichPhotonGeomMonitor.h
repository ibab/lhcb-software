
//---------------------------------------------------------------------------
/** @file RichPhotonGeomMonitor.h
 *
 *  Header file for algorithm class : Rich::rec::MC::PhotonGeomMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPhotonGeomMonitor_H
#define RICHRECMONITOR_RichPhotonGeomMonitor_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRecGeomTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class PhotonGeomMonitor RichPhotonGeomMonitor.h
       *
       *  Monitor the general reconstructed photons properties
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class PhotonGeomMonitor : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        PhotonGeomMonitor( const std::string& name,
                           ISvcLocator* pSvcLocator );

        virtual ~PhotonGeomMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth = nullptr;

        /// Rich Cherenkov angle calculator tool
        const ICherenkovAngle * m_ckAngle = nullptr;         

        /// Simple geometrical questions
        const IGeomTool * m_geomTool = nullptr;         

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr; 

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPhotonGeomMonitor_H
