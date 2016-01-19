
//---------------------------------------------------------------------------
/** @file RichRecRingPeakSearch.h
 *
 *  Header file for algorithm class : Rich::Rec::RingPeakSearch
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   04/09/2009
 */
//---------------------------------------------------------------------------

#ifndef RICHRECQC_RichRecRingPeakSearch_H
#define RICHRECQC_RichRecRingPeakSearch_H 1

// STD
#include <cmath>

// Base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// Kernel
#include "RichRecBase/FastRingFitter.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class RingPeakSearch RichRecRingPeakSearch.h
     *
     *  Uses ring objects as seeds to search for peaks in radius of hits on 
     *  the HPD panel.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   04/09/2009
     */
    //---------------------------------------------------------------------------

    class RingPeakSearch : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      RingPeakSearch( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RingPeakSearch();   ///< Destructor

      virtual StatusCode execute();   ///< Algorithm execution

    protected:
      
      /// Pre-Book all (non-MC) histograms
      virtual StatusCode prebookHistograms();

    private:

      std::string m_ringLoc; ///< Location of RichRecRings in TES

      // Max fit variance
      std::vector<double> m_maxFitVariance;

    };

  }
}

#endif // RICHRECQC_RichRecInit_H
