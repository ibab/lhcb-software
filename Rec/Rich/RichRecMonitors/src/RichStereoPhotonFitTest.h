
//---------------------------------------------------------------------------
/** @file RichStereoPhotonFitTest.h
 *
 *  Header file for algorithm class : Rich::Rec::StereoPhotonFitTest
 *
 *  CVS Log :-
 *  $Id: RichStereoPhotonFitTest.h,v 1.1 2008-05-01 19:42:20 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichStereoPhotonFitTest_H
#define RICHRECMONITOR_RichStereoPhotonFitTest_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Interfaces
#include "RichRecBase/IRichStereoFitter.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------
      /** @class StereoPhotonFitTest RichStereoPhotonFitTest.h
       *
       *  Monitors the performance of the stereographic refitting tool
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //---------------------------------------------------------------------------

      class StereoPhotonFitTest : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        StereoPhotonFitTest( const std::string& name,
                             ISvcLocator* pSvcLocator );

        virtual ~StereoPhotonFitTest( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution

      private: // methods


      private: // data

        /// Pointer to the Stereographic refitting tool
        const IStereoFitter * m_fitter;
  
        /// Track selector
        const ITrackSelector * m_trSelector;

        /// Pointer to RichCherenkovAngle tool
        const ICherenkovAngle * m_ckAngle; 

        /// Number of histogram bins
        unsigned int m_nBins;

        std::vector<double> m_ckThetaMax; ///< Max theta limit for histos for each rad
        std::vector<double> m_ckThetaMin; ///< Min theta limit for histos for each rad
        std::vector<double> m_ckResRange;              ///< Histogram ranges for CK resolution plots

      };

    }
  }
}

#endif // RICHRECMONITOR_RichStereoPhotonFitTest_H
