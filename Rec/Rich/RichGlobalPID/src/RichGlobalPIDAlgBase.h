
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDAlgBase_H
#define RICHGLOBALPID_RichGlobalPIDAlgBase_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"
#include "RichGlobalPIDCommonBase.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class AlgBase RichGlobalPIDAlgBase.h
       *
       *  Abstract base class for GlobalPID algorithms
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2002-11-30
       */
      //--------------------------------------------------------------------------

      class AlgBase : public CommonBase<Rich::Rec::HistoAlgBase>
      {

      public:

        ///< Standard constructor
        AlgBase( const std::string& name,
                 ISvcLocator* pSvcLocator )
          : CommonBase<Rich::Rec::HistoAlgBase>( name, pSvcLocator ) { }

        virtual StatusCode sysExecute(); ///< system execute

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDAlgBase_H
