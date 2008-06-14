
//---------------------------------------------------------------------------
/** @file RichTracklessRingFilterAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TracklessRingFilterAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingFilterAlg.h,v 1.3 2008-06-14 10:42:11 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichTracklessRingFilterAlg_H
#define RICHRECALGORITHMS_RichTracklessRingFilterAlg_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecRing.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class TracklessRingFilterAlg RichTracklessRingFilterAlg.h
     *
     *  Reads in a set of trackless RichRecRing objects and produces a filtered
     *  container of the best rings
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   12/06/2008
     */
    //---------------------------------------------------------------------------

    class TracklessRingFilterAlg : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      TracklessRingFilterAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~TracklessRingFilterAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private:

      /// Input location in TES for rings
      std::string m_inputRings;

      /// Output location in TES for rings
      std::string m_outputRings;

      /// Minimum number of hits on selected rings
      unsigned int m_minNumHits;

      /// Minimum average hit probability for selected rings
      double m_minAvProb;

    };

  }
}

#endif // RICHRECALGORITHMS_RichTracklessRingFilterAlg_H
