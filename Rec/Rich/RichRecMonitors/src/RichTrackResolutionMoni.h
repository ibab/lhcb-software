
//---------------------------------------------------------------------------
/** @file RichTrackResolutionMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::TrackResolutionMoni
 *
 *  CVS Log :-
 *  $Id: RichTrackResolutionMoni.h,v 1.7 2007-02-02 10:07:13 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
#define RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"
#include "Event/MCRichSegment.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"

// Kernel
#include "RichKernel/RichGeomFunctions.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @namespace MC
     *
     *  General namespace for RICH MC related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   05/12/2006
     */
    //-----------------------------------------------------------------------------
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class TrackResolutionMoni RichTrackResolutionMoni.h
       *
       *  Monitors the resolution of the tracking information, in terms important
       *  for the RICH reconstruction.
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class TrackResolutionMoni : public RichRecTupleAlgBase
      {

      public:

        /// Standard constructor
        TrackResolutionMoni( const std::string& name,
                             ISvcLocator* pSvcLocator );

        virtual ~TrackResolutionMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;   ///< Pointer to RichRecMCTruthTool interface
        const Rich::MC::IMCTrackInfoTool * m_mcTkInfo;       ///< MC track information
        const ITrackSelector * m_trSelector; ///< Track selector

      };

    }
  }
}

#endif // RICHRECMONITOR_RICHTRACKRESOLUTIONMONI_H
