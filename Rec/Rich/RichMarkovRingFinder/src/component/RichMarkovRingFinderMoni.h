
//---------------------------------------------------------------------------
/** @file RichMarkovRingFinderMoni.h
 *
 *  Header file for algorithm class : RichMarkovRingFinderMoni
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderMoni.h,v 1.25 2008-08-28 23:48:10 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHMARKOVRINGFINDER_RichMarkovRingFinderMoni_H
#define RICHMARKOVRINGFINDER_RichMarkovRingFinderMoni_H 1

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
    namespace MarkovRingFinder
    {
      namespace MC
      {

        //---------------------------------------------------------------------------
        /** @class Moni RichMarkovRingFinderMoni.h
         *
         *  Runs various calibration tasks for the markov ring finder
         *
         *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
         *  @date   05/04/2002
         */
        //---------------------------------------------------------------------------

        class Moni : public RichRecHistoAlgBase
        {

        public:

          /// Standard constructor
          Moni( const std::string& name,
                ISvcLocator* pSvcLocator );

          virtual ~Moni( ); ///< Destructor

          virtual StatusCode initialize();    // Algorithm initialization
          virtual StatusCode execute   ();    // Algorithm execution

        private:

          /// access RichRecMCTruthTool tool on demand
          const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
          {
            if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
            return m_richRecMCTruth;
          }

        private: // data

          mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface

          /// Location of Rings in TES
          std::string m_ringLoc;

          std::vector<double> m_ckThetaMax; ///< Max theta limit for histos for each rad
          std::vector<double> m_ckThetaMin; ///< Min theta limit for histos for each rad

          /// Number of histogram bins
          unsigned int m_nBins;

        };

      }
    }
  }
}

#endif // RICHMARKOVRINGFINDER_RichMarkovRingFinderMoni_H
