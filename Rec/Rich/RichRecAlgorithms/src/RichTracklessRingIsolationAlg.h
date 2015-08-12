
//---------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TracklessRingIsolationAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H
#define RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "RichRecBase/FastRingFitter.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class TracklessRingIsolationAlg RichTracklessRingIsolationAlg.h
     *
     *  Reads in a set of trackless RichRecRing objects and tries to determine
     *  those which are 'isolated'
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   12/06/2008
     */
    //---------------------------------------------------------------------------

    class TracklessRingIsolationAlg : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      TracklessRingIsolationAlg( const std::string& name,
                                 ISvcLocator* pSvcLocator );

      virtual ~TracklessRingIsolationAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private:

      /** Test the given cut, and keep a summary tally of the result
       *  @param desc   Cut description
       *  @param rad    RICH radiator
       *  @param result Was the cut passed or failed
       *  @return result
       */
      inline bool testCut( const std::string & desc, 
                           const Rich::RadiatorType rad,
                           const bool result ) const
      {
        if ( !m_abortEarly )
        {
          if ( msgLevel(MSG::VERBOSE) && !result )
          {
            verbose() << "  -> '" << desc << "' FAILED" << endmsg;
          }
          counter(Rich::text(rad)+" '"+desc+"'") += ( result ? 1.0 : 0.0 );
        }
        return result;
      }

    private:

      bool m_abortEarly; ///< Reject tracks as soon as possible

      /// Input location in TES for rings
      std::string m_inputRings;

      /// Output location in TES for rings
      std::string m_outputRings;
        
      /// Minimum separation between ring centres for each radiator
      std::vector<double> m_sizesepcut;

      /// Search window for pixels
      std::vector<double> m_pixelWin;

      /// Minimum fraction of pixels in ring region
      std::vector<double> m_sizeringwidth;
      std::vector<double> m_sizepixelcut;
      std::vector<double> m_ckThetaMax;
      std::vector<double> m_sepGMax;
      std::vector<double> m_scale;
      ///Phi cuts
      std::vector<unsigned int> m_nPhiRegions;
      std::vector<double> m_sizephicut;

      // Max ring refiting variance
      std::vector<double> m_maxFitVariance;
    
    };

  }
}

#endif // RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H
