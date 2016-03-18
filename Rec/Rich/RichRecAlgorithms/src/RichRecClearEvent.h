
//---------------------------------------------------------------------------
/** @file RichRecClearEvent.h
 *
 *  Header file for algorithm class : Rich::Rec::ClearEvent
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecClearEvent_H
#define RICHRECALGORITHMS_RichRecClearEvent_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// Event
#include "Event/RichRecStatus.h"

// Event
#include "Event/ProcStatus.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class ClearEvent RichRecClearEvent.h
     *
     *  General initialisation algorithm for RICH reconstruction
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class ClearEvent : public Rich::Rec::AlgBase
    {

    public:

      /// Standard constructor
      ClearEvent( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~ClearEvent();   ///< Destructor

      virtual StatusCode execute(); ///< Algorithm execution

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecClearEvent_H
