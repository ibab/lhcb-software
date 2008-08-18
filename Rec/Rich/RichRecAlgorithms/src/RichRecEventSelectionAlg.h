
//---------------------------------------------------------------------------
/** @file RichRecEventSelectionAlg.h
 *
 *  Header file for algorithm class : Rich::Rec::EventSelectionAlg
 *
 *  CVS Log :-
 *  $Id: RichRecEventSelectionAlg.h,v 1.2 2008-08-18 19:28:37 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecEventSelectionAlg_H
#define RICHRECALGORITHMS_RichRecEventSelectionAlg_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

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
      
    };

  }
}

#endif // RICHRECALGORITHMS_RichRecInit_H
