
//---------------------------------------------------------------------------
/** @file RichRecInit.h
 *
 *  Header file for algorithm class : Rich::Rec::Initialise
 *
 *  CVS Log :-
 *  $Id: RichRecInit.h,v 1.5 2007-03-27 12:44:30 jonrob Exp $
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

    private:

      /// Flag to turn on check for ProcStatus object
      bool m_checkProcStatus;

      /// Flag to turn on creation of RichRecPixels
      bool m_makePixels;

      /// Flag to turn on creation of RichRecTracks
      bool m_makeTracks;

      /// Flag to turn on creation of RichRecPhotons
      bool m_makePhotons;

    };

  }
}

#endif // RICHRECALGORITHMS_RICHRECBACKGROUNDESTI_H
