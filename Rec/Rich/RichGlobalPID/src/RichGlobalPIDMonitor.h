
/** @file RichGlobalPIDMonitor.h
 *
 *  Header file for RICH Global PID algorithm class : RichGlobalPIDMonitor
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDMonitor.h,v 1.2 2004-07-27 10:56:37 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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
