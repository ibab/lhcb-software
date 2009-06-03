
//---------------------------------------------------------------------------
/** @file RichTracklessRingMoni.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::TracklessRingMoni
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingMoni.h,v 1.2 2009-06-03 09:20:26 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECQC_RichTracklessRingMoni_H
#define RICHRECQC_RichTracklessRingMoni_H 1

//STD
#include <sstream>

#include <cmath>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Kernel
#include "RichRecBase/FastRingFitter.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class TracklessRingMoni RichTracklessRingMoni.h
       *
       *  Monitor for trackless rings
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class TracklessRingMoni : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        TracklessRingMoni( const std::string& name,
                           ISvcLocator* pSvcLocator );

        virtual ~TracklessRingMoni( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private:

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth )
          {
            acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
          }
          return m_richRecMCTruth;
        }

      private: // data

        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface

        /// Location of Rings in TES
        std::string m_ringLoc;

        std::vector<double> m_ckThetaMax; ///< Max theta limit for histos for each rad
        std::vector<double> m_ckThetaMin; ///< Min theta limit for histos for each rad

        std::vector<double> m_radiiMax;   ///< Max radii limit for histos for each rad
        std::vector<double> m_radiiMin;   ///< Min radii limit for histos for each rad

        // Max fit variance
        std::vector<double> m_maxFitVariance;

        /// Number of histogram bins
        unsigned int m_nBins;

      };

    }
  }
}

#endif // RICHRECQC_RichTracklessRingMoni_H
