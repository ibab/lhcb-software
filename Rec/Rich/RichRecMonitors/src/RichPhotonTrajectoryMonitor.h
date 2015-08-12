
//---------------------------------------------------------------------------
/** @file RichPhotonTrajectoryMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonTrajectoryMonitor
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_PhotonTrajectoryMonitor_H
#define RICHRECMONITOR_PhotonTrajectoryMonitor_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class PhotonTrajectoryMonitor RichPhotonTrajectoryMonitor.h
       *
       *  Monitor the reconstructed photon trajectories
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class PhotonTrajectoryMonitor : public Rich::Rec::TupleAlgBase
      {

      public:

        /// Standard constructor
        PhotonTrajectoryMonitor( const std::string& name,
                                 ISvcLocator* pSvcLocator );

        virtual ~PhotonTrajectoryMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface

        /// Track selector
        const ITrackSelector * m_trSelector;

      };

    }
  }
}

#endif // RICHRECMONITOR_PhotonTrajectoryMonitor_H
