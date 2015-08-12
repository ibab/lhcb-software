
//---------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecEventSelectionAlg_H
#define RICHRECALGORITHMS_RichRecEventSelectionAlg_H 1

// STD
#include <set>

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Kernel
#include "RichKernel/RichMap.h"

// Boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class EventSelectionAlg RichRecEventSelectionAlg.h
     *
     *  Event selection alg for the RICH
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class EventSelectionAlg : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      EventSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~EventSelectionAlg();   ///< Destructor

      virtual StatusCode execute();   ///< Algorithm execution

    private:

      std::string m_ringLoc; ///< Location of RichRecRings in TES

      unsigned int m_minPixels; ///< Minimum number of RICH pixels
      unsigned int m_minRings;  ///< Minimum number of rings
      unsigned int m_minTracks; ///< Minimum number of tracks
      unsigned int m_minHPDsWithHits; ///< The minimum number of HPDs to have more that 'MinHPDHits' in them      
      unsigned int m_minHPDHits; ///< The minimum number of hits to have in at least 'MinHPDsWithHits' HPDs

      unsigned int m_maxPixels; ///< Maximum number of RICH pixels
      unsigned int m_maxRings;  ///< Maximum number of rings
      unsigned int m_maxTracks; ///< Maximum number of tracks
      unsigned int m_maxHPDsWithHits; ///< The maximum number of HPDs to have more that 'MinHPDHits' in them
      unsigned int m_maxHPDHits; ///< The maximum number of hits to have in at least 'MinHPDsWithHits' HPDs

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecInit_H
