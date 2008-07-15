
//---------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TracklessRingIsolationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingIsolationAlg.h,v 1.3 2008-07-15 14:15:42 shaines Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H
#define RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// Kernel
#include "RichKernel/RichMap.h"

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

    class TracklessRingIsolationAlg : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      TracklessRingIsolationAlg( const std::string& name,
                                 ISvcLocator* pSvcLocator );

      virtual ~TracklessRingIsolationAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution


    private:

      /// Input location in TES for rings
      std::string m_inputRings;

      /// Output location in TES for rings
      std::string m_outputRings;
        
      /// Minimum separation between ring centres for each radiator
      std::vector<double> m_sizesepcut;

      /// Minimum fraction of pixels in ring region
      std::vector<double> m_sizeringwidth;
      std::vector<double> m_sizepixelcut;
      std::vector<double> m_ckThetaMax;
      std::vector<double> m_sepGMax;
      std::vector<double> m_scale;
      ///Phi cuts
      std::vector<unsigned int> m_nPhiRegions;
      std::vector<double> m_sizephicut;
    
    };

  }
}

#endif // RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H
