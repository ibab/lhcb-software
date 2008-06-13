
//---------------------------------------------------------------------------
/** @file RichTracklessRingIsolationAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TracklessRingIsolationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingIsolationAlg.h,v 1.1 2008-06-13 12:46:14 jonrob Exp $
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

    };

  }
}

#endif // RICHRECALGORITHMS_RichTracklessRingIsolationAlg_H
