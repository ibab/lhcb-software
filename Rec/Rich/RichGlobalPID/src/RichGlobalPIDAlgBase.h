
//--------------------------------------------------------------------------
/** @file RichGlobalPIDAlgBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDAlgBase.h,v 1.11 2008-03-25 16:26:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDAlgBase_H
#define RICHGLOBALPID_RichGlobalPIDAlgBase_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"
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

      class AlgBase : public CommonBase<Rich::Rec::AlgBase>
      {

      public:

        ///< Standard constructor
        AlgBase( const std::string& name,
                 ISvcLocator* pSvcLocator )
          : CommonBase<Rich::Rec::AlgBase>( name, pSvcLocator ) { }

        virtual StatusCode sysExecute(); ///< system execute

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDAlgBase_H
