
/** @file RichGlobalPIDMonitor.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDMonitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.h,v 1.3 2005-06-23 14:54:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

#ifndef RICHGLOBALPID_RICHGLOBALPIDMONITOR_H
#define RICHGLOBALPID_RICHGLOBALPIDMONITOR_H 1

// base class
#include "RichGlobalPIDAlgBase.h"

/** @class RichGlobalPIDMonitor RichGlobalPIDMonitor.h
 *
 *  Monitor class for Rich Global PID algorithm
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/12/2002
 */

class RichGlobalPIDMonitor : public RichGlobalPIDAlgBase {

public:

  /// Standard constructor
  RichGlobalPIDMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichGlobalPIDMonitor();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

};

#endif // RICHGLOBALPID_RICHGLOBALPIDMONITOR_H
