
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PixelPositionMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelPositionMonitor.h,v 1.8 2007-04-24 12:37:45 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPixelPositionMonitor_H
#define RICHRECMONITOR_RichPixelPositionMonitor_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichDet
#include "RichDAQ/RichDAQDefinitions.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichRecBase/IRichRecGeomTool.h"

// boost
#include "boost/lexical_cast.hpp"

// RichDet
#include "RichDet/DeRichSystem.h"

// Event
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class PixelPositionMonitor RichPixelPositionMonitor.h
       *
       *  Monitor class for general position and geometry of RichRecPixels
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //-----------------------------------------------------------------------------

      class PixelPositionMonitor : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        PixelPositionMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator );

        virtual ~PixelPositionMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth;

        /// Pointer to MC truth tool
        const Rich::MC::IMCTruthTool * m_mcTool;

        /// Pointer to RichSmartID tool
        const ISmartIDTool * m_idTool;

        /// Pointer to RICH system detector element
        const DeRichSystem * m_richSys;

        /// Simple geometrical questions
        const IGeomTool * m_geomTool;   

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPixelPositionMonitor_H
