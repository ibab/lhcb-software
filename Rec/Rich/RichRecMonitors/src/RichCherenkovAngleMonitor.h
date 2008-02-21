
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::CherenkovAngleMonitor
 *
 *  CVS Log :-
 *  $Id: RichCherenkovAngleMonitor.h,v 1.11 2008-02-21 16:44:51 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_CherenkovAngleMonitor_H
#define RICHRECMONITOR_CherenkovAngleMonitor_H 1

// STL
#include <sstream>

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

// boost
#include "boost/lexical_cast.hpp"
#include "boost/format.hpp"

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

      private:

        /// Returns the phi region as an int
        inline int phiRegion( const double phi ) const
        {
          const double phiInc = 2.0 * M_PI / m_nPhiRegions;
          return (int)( (phi+(0.5*phiInc)) / phiInc );
        }

        /// Returns the phi region as a string
        inline std::string phiRegionString( const double phi ) const
        {
          return boost::lexical_cast<std::string>( phiRegion(phi) );
        }

        /// Returns the phi regions 'description'
        std::string phiRegionDesc( const double phi ) const;

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

        /// Number of phi regions to use
        unsigned int m_nPhiRegions;

      };

    }
  }
}

#endif // RICHRECMONITOR_CherenkovAngleMonitor_H
