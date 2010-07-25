
//--------------------------------------------------------------------------
/** @file RichGlobalPIDToolBase.h
 *
 *  Header file for RICH global PID algorithm base class : Rich::Rec::GlobalPID::ToolBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-11-30
 */
//--------------------------------------------------------------------------

#ifndef RICHGLOBALPID_RichGlobalPIDToolBase_H
#define RICHGLOBALPID_RichGlobalPIDToolBase_H 1

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecHistoToolBase.h"
#include "RichGlobalPIDCommonBase.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {

      //--------------------------------------------------------------------------
      /** @class ToolBase RichGlobalPIDToolBase.h
       *
       *  Abstract base class for GlobalPID tools
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2002-11-30
       */
      //--------------------------------------------------------------------------

      class ToolBase : public CommonBase<Rich::Rec::HistoToolBase>,
                       virtual public IIncidentListener
      {

      public:

        ///< Standard constructor
        ToolBase( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
          : CommonBase<Rich::Rec::HistoToolBase>( type, name, parent ) { }

        // Initialization of the algorithm after creation
        virtual StatusCode initialize();

        /** Implement the handle method for the Incident service.
         *  This is used to inform the tool of software incidents.
         *
         *  @param incident The incident identifier
         */
        void handle( const Incident& incident );

      };

    }
  }
}

#endif // RICHGLOBALPID_RichGlobalPIDToolBase_H
