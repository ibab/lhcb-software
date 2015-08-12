
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PixelPositionMonitor
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

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichDAQDefinitions.h"
#include "RichKernel/RichSmartIDCnv.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "MCInterfaces/IRichMCTruthTool.h"
#include "RichKernel/IRichSmartIDTool.h"

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

      class PixelPositionMonitor : public Rich::Rec::HistoAlgBase
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

        /// Number of bins in 2D histos
        unsigned int m_n2DBins;

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPixelPositionMonitor_H
