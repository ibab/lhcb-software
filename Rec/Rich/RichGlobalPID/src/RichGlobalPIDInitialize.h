
//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Initialize
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDInitialize.h,v 1.5 2007-02-02 10:03:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//------------------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rich::Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @namespace GlobalPID
     *
     *  General namespace for Global PID software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   04/12/2006
     */
    //-----------------------------------------------------------------------------
    namespace GlobalPID
    {

      //------------------------------------------------------------------------------------
      /** @class Initialize RichGlobalPIDInitialize.h
       *
       *  Initialises the working objects for the Global PID
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   12/12/2002
       */
      //------------------------------------------------------------------------------------

      class Initialize : public AlgBase
      {

      public:

        /// Standard constructor
        Initialize( const std::string& name,
                    ISvcLocator* pSvcLocator );

        virtual ~Initialize();   ///< Destructor

        virtual StatusCode initialize();      // Algorithm initialization
        virtual StatusCode execute   ();      // Algorithm execution

      };

    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
