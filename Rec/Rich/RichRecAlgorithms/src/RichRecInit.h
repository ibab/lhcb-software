
//---------------------------------------------------------------------------
/** @file RichRecInit.h
 *
 *  Header file for algorithm class : Rich::Rec::Initialise
 *
 *  CVS Log :-
 *  $Id: RichRecInit.h,v 1.4 2007-03-09 22:48:41 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
#define RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/RichRecStatus.h"

// Event
#include "Event/ProcStatus.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class Initialise RichRecInit.h
     *
     *  General initialisation algorithm for RICH reconstruction
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class Initialise : public RichRecAlgBase
    {

    public:

      /// Standard constructor
      Initialise( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~Initialise();   ///< Destructor

      virtual StatusCode execute(); /// Algorithm execution

    };

  }
}

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
