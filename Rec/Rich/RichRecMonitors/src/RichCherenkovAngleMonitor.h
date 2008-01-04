
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::CherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.h,v 1.10 2008-01-04 16:15:14 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_CherenkovAngleMonitor_H
#define RICHRECMONITOR_CherenkovAngleMonitor_H 1

// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class CherenkovAngleMonitor RichCherenkovAngleMonitor.h
       *
       *  Template for monitor algorithms. For cut 'n' paste ...
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class CherenkovAngleMonitor : public RichRecTupleAlgBase
      {

      public:

        /// Standard constructor
        CherenkovAngleMonitor( const std::string& name,
                               ISvcLocator* pSvcLocator );

        virtual ~CherenkovAngleMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // data

        const IParticleProperties * m_richPartProp;   ///< Rich Particle properties
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle     * m_ckAngle;        ///< Rich Cherenkov angle calculator tool

        double m_minBeta;        ///< minimum beta value for 'saturated' tracks

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Number of bins for 1D histograms
        unsigned int m_nBins1D;

        /// Number of bins for 2D histograms
        unsigned int m_nBins2D;

      };

    }
  }
}

#endif // RICHRECMONITOR_CherenkovAngleMonitor_H
