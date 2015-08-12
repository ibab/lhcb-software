
//---------------------------------------------------------------------------
/** @file RichCherenkovResMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::CherenkovResMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Event-
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class CherenkovResMoni RichCherenkovResMoni.h
       *
       *  Monitors the resolution of the tracking information, in terms important
       *  for the RICH reconstruction.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class CherenkovResMoni : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        CherenkovResMoni( const std::string& name,
                          ISvcLocator* pSvcLocator );

        virtual ~CherenkovResMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle * m_ckAngle;         ///< Rich Cherenkov angle calculator tool
        const ICherenkovResolution * m_ckAngleRes; ///< Cherenkov angle resolution tool
        const ITrackSelector * m_trSelector; ///< Track selector

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
