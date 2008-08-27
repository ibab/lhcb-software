
//---------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.h,v 1.4 2008-08-27 15:08:16 jonrob Exp $
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

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Kernel
#include "RichKernel/RichMap.h"

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

    class EventSelectionAlg : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      EventSelectionAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~EventSelectionAlg();   ///< Destructor

      virtual StatusCode execute();   ///< Algorithm execution

    private:

      int m_minPixels; ///< Minimum number of RICH pixels
      
      int m_minRings;  ///< Minimum number of rings

      int m_minTracks; ///< Minimum number of tracks

      std::string m_ringLoc; ///< Location of RichRecRings in TES

      unsigned int m_minHPDsWithHits; ///< The minimum number of HPDs to have more that 'MinHPDHits' in them
      
      unsigned int m_minHPDHits; ///< The minimum number of hits to have in at least 'MinHPDsWithHits' HPDs

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecInit_H
