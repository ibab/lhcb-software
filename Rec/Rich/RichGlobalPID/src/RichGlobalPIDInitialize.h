
//------------------------------------------------------------------------------------
/** @file RichGlobalPIDInitialize.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::Initialize
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */
//------------------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H
#define RICHGLOBALPID_RICHGLOBALPIDINITIALIZE_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

namespace Rich
{
  namespace Rec
  {
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
