
/** @file RichGlobalPIDMonitor.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::MC::Monitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.h,v 1.5 2007-03-09 22:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDMONITOR_H
#define RICHGLOBALPID_RICHGLOBALPIDMONITOR_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

namespace Rich
{
  namespace Rec
  {
    namespace GlobalPID
    {
      namespace MC
      {

        /** @class Monitor RichGlobalPIDMonitor.h
         *
         *  Monitor class for Rich Global PID algorithm
         *
         *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
         *  @date   12/12/2002
         */

        class Monitor : public Rich::Rec::HistoAlgBase
        {

        public:

          /// Standard constructor
          Monitor( const std::string& name, ISvcLocator* pSvcLocator );

          virtual ~Monitor();   ///< Destructor

          virtual StatusCode initialize();    // Algorithm initialization
          virtual StatusCode execute   ();    // Algorithm execution
          virtual StatusCode finalize  ();    // Algorithm finalization

        };

      }
    }
  }
}

#endif // RICHGLOBALPID_RICHGLOBALPIDMONITOR_H
