
/** @file RichGlobalPIDMonitor.h
 *
 *  Header file for RICH Global PID algorithm class : Rich::Rec::GlobalPID::MC::Monitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.h,v 1.4 2007-02-02 10:03:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDMONITOR_H
#define RICHGLOBALPID_RICHGLOBALPIDMONITOR_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

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

      //-----------------------------------------------------------------------------
      /** @namespace MC
       *
       *  General namespace for RICH MC related software
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   05/12/2006
       */
      //-----------------------------------------------------------------------------
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
