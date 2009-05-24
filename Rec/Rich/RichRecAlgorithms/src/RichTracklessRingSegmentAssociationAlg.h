
//---------------------------------------------------------------------------
/** @file RichTracklessRingSegmentAssociationAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::TracklessRingSegmentAssociationAlg
 *
 *  CVS Log :-
 *  $Id: RichTracklessRingSegmentAssociationAlg.h,v 1.2 2009-05-24 16:18:25 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichTracklessRingSegmentAssociationAlg_H
#define RICHRECALGORITHMS_RichTracklessRingSegmentAssociationAlg_H 1

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
    /** @class TracklessRingSegmentAssociationAlg RichTracklessRingSegmentAssociationAlg.h
     *
     *  Reads in a set of trackless RichRecRing objects and attempts to perform
     *  an association to the nearest RichRecSegment
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   12/06/2008
     */
    //---------------------------------------------------------------------------

    class TracklessRingSegmentAssociationAlg : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      TracklessRingSegmentAssociationAlg( const std::string& name,
                                          ISvcLocator* pSvcLocator );

      virtual ~TracklessRingSegmentAssociationAlg();   ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialisation
      virtual StatusCode execute();    ///< Algorithm execution

    private:

      /// Input location in TES for rings
      std::string m_inputRings;

      /// Min ring centre / track seperation distance for association
      std::vector<double> m_maxDist;

    };

  }
}

#endif // RICHRECALGORITHMS_RichTracklessRingSegmentAssociationAlg_H
