
//---------------------------------------------------------------------------
/** @file RichTrackGeomMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::TrackGeomMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"
#include "Event/MCRichSegment.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "Kernel/RichTraceMode.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "MCInterfaces/IRichMCTrackInfoTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichKernel/IRichSmartIDTool.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class TrackGeomMoni RichTrackGeomMoni.h
       *
       *  Monitors the general geometrical details of the RichRecTrack
       *  and RichRecSegment objects.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class TrackGeomMoni : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        TrackGeomMoni( const std::string& name,
                       ISvcLocator* pSvcLocator );

        virtual ~TrackGeomMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const IRayTracing * m_rayTrace;        ///< Pointer RichDetector ray tracing tool
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
        const IGeomTool * m_geomTool;     ///< Pointer to the Geometry tool
        const IGeomEff * m_geomEffic; ///< Pointer to RichGeomEff tool
        const Rich::MC::IMCTrackInfoTool * m_mcTkInfo; ///< MC track information
        /// Pointer to RichSmartID tool
        const ISmartIDTool * m_idTool;

        /// Track selector
        const ITrackSelector * m_trSelector;

        // working variables for average PD positions
        Rich::Map<LHCb::RichSmartID, double> m_xHits;
        Rich::Map<LHCb::RichSmartID, double> m_yHits;
        Rich::Map<LHCb::RichSmartID, double> m_zHits;
        Rich::Map<LHCb::RichSmartID, int> m_hitCount;

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
