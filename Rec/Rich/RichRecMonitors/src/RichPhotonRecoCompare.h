
//---------------------------------------------------------------------------
/** @file RichPhotonRecoCompare.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PhotonRecoCompare
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2016
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_PhotonRecoCompare_H
#define RICHRECMONITOR_PhotonRecoCompare_H 1

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichKernel/IRichParticleProperties.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichPhotonReconstruction.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class PhotonRecoCompare RichPhotonRecoCompare.h
       *
       *  Compares the active photon reconstruction performance to that for
       *  the baseline (Quartic) method
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   14/01/2016
       */
      //---------------------------------------------------------------------------

      class PhotonRecoCompare : public Rich::Rec::TupleAlgBase
      {

      public:

        /// Standard constructor
        PhotonRecoCompare( const std::string& name,
                           ISvcLocator* pSvcLocator );

        virtual ~PhotonRecoCompare( ); ///< Destructor

        virtual StatusCode initialize();  ///< Algorithm initialization
        virtual StatusCode execute   ();  ///< Algorithm execution

      private: // data

        /// Baseline photon reconstruction tool to compare to
        const IPhotonReconstruction * m_reco = nullptr;

        /// Track selector
        const ITrackSelector * m_trSelector = nullptr;

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth = nullptr;

      };

    }
  }
}

#endif // RICHRECMONITOR_PhotonRecoCompare_H
