
//---------------------------------------------------------------------------
/** @file RichCherenkovAngleMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::CherenkovAngleMonitor
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

// Event
#include "Event/MCRichOpticalPhoton.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichKernel
#include "RichKernel/RichMap.h"

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
       *  Monitor the photon angles
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class CherenkovAngleMonitor : public Rich::Rec::TupleAlgBase
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

        typedef std::pair<std::string,std::string> PhiDesc;

        /// Returns the phi regions 'description'
        const PhiDesc & phiRegionDesc( const double phi ) const;

      private: // data

        const IParticleProperties * m_richPartProp;   ///< Rich Particle properties
        const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
        const ICherenkovAngle     * m_ckAngle;        ///< Rich Cherenkov angle calculator tool

        double m_minBeta;        ///< minimum beta value for 'saturated' tracks

        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Number of phi regions to use
        unsigned int m_nPhiRegions;

        /// Cahced region descriptions
        mutable Rich::Map<int,PhiDesc> m_rDesc;

      };

    }
  }
}

#endif // RICHRECMONITOR_CherenkovAngleMonitor_H
