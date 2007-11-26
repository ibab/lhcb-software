
//---------------------------------------------------------------------------
/** @file RichPhotonGeomMonitor.h
 *
 *  Header file for algorithm class : Rich::rec::MC::PhotonGeomMonitor
 *
 *  CVS Log :-
 *  $Id: RichPhotonGeomMonitor.h,v 1.7 2007-11-26 17:14:01 jonrob Exp $
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

      class PhotonGeomMonitor : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        PhotonGeomMonitor( const std::string& name,
                           ISvcLocator* pSvcLocator );

        virtual ~PhotonGeomMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;    ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle * m_ckAngle;          ///< Rich Cherenkov angle calculator tool
        const IGeomTool * m_geomTool;            ///< Simple geometrical questions
        const ITrackSelector * m_trSelector;  ///< Track selector

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
